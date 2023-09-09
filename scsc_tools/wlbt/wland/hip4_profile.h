/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef SLSI_HIP4PROFILE_H
#define SLSI_HIP4PROFILE_H

#include <stdio.h>
#include <functional>
#include <tuple>
#include <queue>
#include <map>

#define CPU_MAX (8)

struct idx_value {
    int16_t read;
    int16_t write;
    int16_t service;
};
enum hip4_hip_q_conf {
    HIP4_MIF_Q_FH_CTRL,
    HIP4_MIF_Q_FH_DAT,
    HIP4_MIF_Q_FH_RFB,
    HIP4_MIF_Q_TH_CTRL,
    HIP4_MIF_Q_TH_DAT,
    HIP4_MIF_Q_TH_RFB,
    HIP4_MIF_Q_MAX
};

#define HIP4_SAMPLER_BOT_QMOD_RX (0x30)
#define HIP4_SAMPLER_BOT_QMOD_TX (0x31)

enum mbulk_class_t {
    MBULK_CLASS_CONTROL = 0,
    MBULK_CLASS_HOSTIO = 1,
    MBULK_CLASS_DEBUG = 2,

    MBULK_CLASS_FROM_HOST_DAT = 3,
    MBULK_CLASS_FROM_HOST_CTL = 4,
    MBULK_CLASS_FROM_RADIO = 5,
    MBULK_CLASS_DPLP = 6,
    MBULK_CLASS_OTHERS = 7,
    MBULK_CLASS_FROM_RADIO_FORWARDED = 8,
    MBULK_CLASS_MAX
};
#define HIP4_SAMPLER_MBULK (0xaa)

static char IDX_TO_COLOR[][16] = {
    "\"Black\"",
    "\"Red\"",
    "\"Maroon\"",
    "\"Yellow\"",
    "\"Olive\"",
    "\"Lime\"",
    "\"Green\"",
    "\"Aqua\"",
    "\"Teal\"",
    "\"Blue\"",
    "\"Navy\"",
    "\"Fuchsia\"",
    "\"Red\"",
    "\"Purple\""
};

enum hip4_source_type_t {
    DEV_TYPE = 0,
    SABLE_TYPE = 1
};

class Hip4Profile {
private:
    static FILE* input;
    static FILE* output;
    static std::queue<std::tuple<long long int, uint16_t>> cpu_record_list[CPU_MAX];
    static idx_value q_value[HIP4_MIF_Q_MAX];
    static std::queue<std::tuple<long long int, uint8_t, uint8_t>> q_record_list[HIP4_MIF_Q_MAX];

    static std::map< uint32_t, std::queue<std::tuple<long long int, uint16_t, uint16_t>>> qmcod_rx_record_list;
    static std::map< uint32_t, std::queue<std::tuple<long long int, uint16_t, uint16_t>>> qmcod_tx_record_list;

    static std::queue<std::tuple<long long int, uint16_t>> mbulk_record_list[MBULK_CLASS_MAX];

    static bool parse_sable_header(FILE* f, struct sable_commen_hdr* buffer);
    static bool parse_sable_version_specific_header(FILE* f, struct sable_commen_hdr* cmn_hdr,
                                                    struct sable_version_specific_hdr* buffer);
    static bool find_hip4_chunk(FILE* f, struct sable_commen_hdr* cmn_hdr, int* end_offset);
    static void update_cpu_freq_record(struct hip4_record* record);
    static bool update_irq_record(struct hip4_record* record);
    static void update_fh_queue(struct hip4_record* record);
    static void update_th_queue(struct hip4_record* record);
    static bool update_q_record(struct hip4_record* record);
    static bool update_qmcod_record(struct hip4_record* record);
    static bool update_mbulk_record(struct hip4_record* record);
    static bool read_hip4_record(FILE* f, struct hip4_record* record, int offset_end);
    static inline void json_open_array(FILE* output) {write(fileno(output), "[", 1);}
    static inline void json_close_array(FILE* output, const bool has_next) {
        if (!has_next)
            write(fileno(output), "]", 1);
        else
            write(fileno(output), "],", 2);
    }
    static inline void json_open_object(FILE* output) {write(fileno(output), "{", 1);}
    static inline void json_close_object(FILE* output, const bool has_next) {
        if (!has_next)
            write(fileno(output), "}", 1);
        else
            write(fileno(output), "},", 2);
    }
    static inline void json_write_field(FILE* output, const char* field) {
        char field_name_str[128] = {0};
        size_t ret;
        ret = snprintf(field_name_str, sizeof(field_name_str), "%s:", field);
        write(fileno(output), field_name_str, strlen(field_name_str));
    }
    static inline void json_write_value(FILE* output, const char* value, const bool has_next) {
        char value_str[128] = {0};
        size_t ret;
        if (!has_next)
            ret = snprintf(value_str, sizeof(value_str), "%s", value);
        else
            ret = snprintf(value_str, sizeof(value_str), "%s,", value);
        write(fileno(output), value_str, ret);
    }
    static inline void json_open_chart(FILE* output, const char* chart_name) {
        char chart_name_str[128] = {0};
        snprintf(chart_name_str, sizeof(chart_name_str), "\"%s\"", chart_name);
        json_open_object(output);
        json_write_field(output, "\"ChartName\"");
        json_write_value(output, chart_name_str, true);
        json_write_field(output, "\"ChartData\"");
        json_open_object(output);
        json_write_field(output, "\"type\"");
        json_write_value(output, "\"scatter\"", true);
        json_write_field(output, "\"data\"");
        json_open_object(output);
        json_write_field(output, "\"datasets\"");
        json_open_array(output);
    }
    static inline void json_close_chart(FILE* output, const bool has_next) {
        json_close_array(output, false);
        json_close_object(output, false);
        json_close_object(output, false);
        json_close_object(output, has_next);
    }
    static inline void json_open_label(FILE* output, const char* label_name, const uint32_t label_idx) {
        char label_name_str[128] = {0};
        json_open_object(output);
        json_write_field(output, "\"label\"");
        snprintf(label_name_str, sizeof(label_name_str), "\"%s\"", label_name);
        json_write_value(output, label_name_str, true);
        json_write_field(output, "\"borderWidth\"");
        json_write_value(output, "0.5", true);
        json_write_field(output, "\"pointRadius\"");
        json_write_value(output, "0.5", true);
        json_write_field(output, "\"fill\"");
        json_write_value(output, "\"false\"", true);
        json_write_field(output, "\"showLine\"");
        json_write_value(output, "\"true\"", true);
        json_write_field(output, "\"backgroundColor\"");
        json_write_value(output, IDX_TO_COLOR[label_idx%14], true);
        json_write_field(output, "\"borderColor\"");
        json_write_value(output, IDX_TO_COLOR[label_idx%14], true);
        json_write_field(output, "\"data\"");
        json_open_array(output);
    }
    static inline void json_close_label(FILE* output, const bool has_next) {
        json_close_array(output, false);
        json_close_object(output, has_next);
    }
    static inline void json_add_data(FILE* output, const char* x_value, const char* y_value, const bool has_next) {
        json_open_object(output);
        json_write_field(output, "\"x\"");
        json_write_value(output, x_value, true);
        json_write_field(output, "\"y\"");
        json_write_value(output, y_value, false);
        json_close_object(output, has_next);
    }
public:
    static int start(const char* source, const char* output_file, hip4_source_type_t type);
};

#endif
