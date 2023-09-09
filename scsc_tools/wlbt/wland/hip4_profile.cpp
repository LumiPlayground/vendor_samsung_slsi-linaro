/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sstream>
#include <string>
#include <arpa/inet.h>
#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>

#include "filedir.h"
#include "hip4_profile.h"

static const char* SABLE_MAGIC = "SCSC";

struct sable_commen_hdr {
    char magic_string[4];
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t num_of_chunks;
    uint8_t trigger_cause;
    uint16_t offset_data;
} __attribute__((packed));

struct sable_version_specific_hdr {
    union {
        struct {
            char fw_version[64];
            char host_version[64];
            char fapi_version[64];
            uint16_t reason_code;
            uint32_t reserved;
            uint8_t supported_chunks[48];
        } __attribute__((packed)) v_0_1;
#define HEADER_LENGTH_V_0_1 (246)
        struct {
            char fw_version[128];
            char host_version[64];
            char fapi_version[64];
            uint16_t reason_code;
            uint32_t reserved;
            uint8_t supported_chunks[48];
        } __attribute__((packed)) v_2_21_3;
#define HEADER_LENGTH_V_2_21_3 (310)
    }hdr;
} __attribute__((packed));

enum sable_trigger_t {
    SCSC_LOG_UNKNOWN = 0,
    SCSC_LOG_FW_PANIC,
    SCSC_LOG_USER,
    SCSC_LOG_FW,
    SCSC_LOG_DUMPSTATE,
    SCSC_LOG_HOST_WLAN,
    SCSC_LOG_HOST_BT,
    SCSC_LOG_HOST_COMMON,
    SCSC_LOG_SYS_ERROR
};

static const char* CHUNK_MAGIC = "CHK";

struct chunk_hdr {
    char magic_string[3];
    uint8_t type;
    uint32_t chunk_payload_size;
} __attribute__((packed));

struct hip4_record {
    uint32_t record_number : 16;
    uint32_t freq : 12;
    uint32_t cpu : 4;
    long long int ts;
    union {
        struct { // okay
            uint8_t rw;
            uint8_t value;
            uint8_t idx_rw;
            uint8_t queue;
            uint32_t record2;
        } __attribute__((packed)) q_record; // okay
        struct {
            uint8_t pri;
            uint8_t peer_index;
            uint8_t vif;
            uint8_t type;
            uint16_t qcod;
            uint16_t qmod;
        } __attribute__((packed)) qmcod_record;
        struct {
            uint16_t free_cnt;
            uint8_t clas;
            uint8_t mbulk_type;
            uint32_t record2;
        } __attribute__((packed)) mbulk_record;
        // To-Do: Add more record types.
    } record;
} __attribute__((packed));

/*<<<<<<<<<<<<<<<<<<<<< Queue Profile >>>>>>>>>>>>>>>>>>>>>>*/
static char Q_INDEX_TO_NAME[][32] = {
    "FH_CTRL",
    "FH_DAT ",
    "FH_RFB ",
    "TH_CTRL",
    "TH_DAT ",
    "TH_RFB ",
    "MAX    "
};

enum action_t {
    UPDATE_ACTION = 0,
    READ_ACTION = 1
};

enum idx_t {
    WRITE_IDX = 0,
    READ_IDX
};
/*<<<<<<<<<<<<<<<<<<<<< Queue Profile >>>>>>>>>>>>>>>>>>>>>>*/

/*<<<<<<<<<<<<<<<<<<<<< Mbulk >>>>>>>>>>>>>>>>>>>>>>*/
static char MBULK_CLASS_TO_NAME[][33] = {
    "CTRL",
    "HOSTIO",
    "DEBUG ",
    "FH_DAT",
    "FH_CTL ",
    "FRdo",
    "DPLP",
    "OTHERS",
    "FRdo_FWD",
    "MAX"
};
/*<<<<<<<<<<<<<<<<<<<<< Mbulk >>>>>>>>>>>>>>>>>>>>>>*/

FILE* Hip4Profile::input = nullptr;
FILE* Hip4Profile::output = nullptr;
std::queue<std::tuple<long long int, uint16_t>> Hip4Profile::cpu_record_list[CPU_MAX];
idx_value Hip4Profile::q_value[HIP4_MIF_Q_MAX];
std::queue<std::tuple<long long int, uint8_t, uint8_t>> Hip4Profile::q_record_list[HIP4_MIF_Q_MAX];

std::map< uint32_t, std::queue<std::tuple<long long int, uint16_t, uint16_t>>> Hip4Profile::qmcod_rx_record_list;
std::map< uint32_t, std::queue<std::tuple<long long int, uint16_t, uint16_t>>> Hip4Profile::qmcod_tx_record_list;

std::queue<std::tuple<long long int, uint16_t>> Hip4Profile::mbulk_record_list[MBULK_CLASS_MAX];

bool Hip4Profile::parse_sable_header(FILE* f, struct sable_commen_hdr* buffer) {
    if (sizeof(struct sable_commen_hdr) != read(fileno(f), (void*)buffer, sizeof(struct sable_commen_hdr))) {
        return false;
    }
    if (memcmp(SABLE_MAGIC, buffer->magic_string, 4) != 0) {
        return false;
    }
    return true;
}

bool Hip4Profile::parse_sable_version_specific_header(FILE* f, struct sable_commen_hdr* cmn_hdr,
                                                                     struct sable_version_specific_hdr* buffer) {
    switch (cmn_hdr->major_version) {
        case 0:
        case 1:
        if (HEADER_LENGTH_V_0_1 != read(fileno(f), (void*)buffer, HEADER_LENGTH_V_0_1)) {
            return false;
        }
        return true;
        break;
        case 2:
        case 3:
        if (HEADER_LENGTH_V_2_21_3 != read(fileno(f), (void*)buffer, HEADER_LENGTH_V_2_21_3)) {
            return false;
        }
        return true;
        break;
        default:
        return false;
        break;
    }
}

bool Hip4Profile::find_hip4_chunk(FILE* f, struct sable_commen_hdr* cmn_hdr, int* end_offset) {
    struct chunk_hdr chunk;

    lseek(fileno(f), cmn_hdr->offset_data, SEEK_SET);
    for (uint32_t chk = 0 ; chk < cmn_hdr->num_of_chunks ; chk++) {
        if (sizeof(struct chunk_hdr) != read(fileno(f), (void*)&chunk, sizeof(struct chunk_hdr))) {
            return false;
        }
        if (memcmp(CHUNK_MAGIC, chunk.magic_string, 3) != 0) {
            return false;
        }
        switch (cmn_hdr->major_version) {
            case 0:
                if (6 == chunk.type) {
                    (*end_offset) = (int)lseek(fileno(f), 0, SEEK_CUR) + (int)chunk.chunk_payload_size;
                    return true;
                }
            break;

            case 1:
            case 2:
                if (6 == chunk.type) {
                    (*end_offset) = (int)lseek(fileno(f), 0, SEEK_CUR) + (int)chunk.chunk_payload_size;
                    return true;
                }
            break;

            case 3:
                if (132 == chunk.type) {
                    (*end_offset) = (int)lseek(fileno(f), 0, SEEK_CUR) + (int)chunk.chunk_payload_size;
                    return true;
                }
            break;

            default:
                return false;
            break;
        }
        lseek(fileno(f), chunk.chunk_payload_size, SEEK_CUR);
    }
    return false;
}

void Hip4Profile::update_cpu_freq_record(struct hip4_record* record) {
    if (record->freq != 0 && record->cpu < CPU_MAX) {
        cpu_record_list[record->cpu].push(std::make_tuple((long long int)record->ts, (uint16_t)record->freq));
    }
}

void Hip4Profile::update_fh_queue(struct hip4_record* record) {
    const uint8_t queue = record->record.q_record.queue;
    const uint8_t idx_rw = record->record.q_record.idx_rw;
    const uint8_t value = record->record.q_record.value;
    const uint8_t action = record->record.q_record.rw;

    if (action == READ_ACTION) {
        if (idx_rw == WRITE_IDX) {
            // Sampler always get write idx first for FH queues
            q_value[queue].read = -1;
            q_value[queue].write = -1;
            q_value[queue].service = -1;

            q_value[queue].write = value;
        } else {
            q_value[queue].read = value;
        }
    } else { /* UPDATE_ACTION */
        q_value[queue].service = value;
        if (q_value[queue].read != -1 && q_value[queue].write != -1 && q_value[queue].service != -1) {
            q_record_list[queue].push(std::make_tuple((long long int)record->ts,
                                      (uint8_t)q_value[queue].write - (uint8_t)q_value[queue].read,
                                      (uint8_t)q_value[queue].service - (uint8_t)q_value[queue].write));
        }
    }
}

void Hip4Profile::update_th_queue(struct hip4_record* record) {
    const uint8_t queue = record->record.q_record.queue;
    const uint8_t idx_rw = record->record.q_record.idx_rw;
    const uint8_t value = record->record.q_record.value;
    const uint8_t action = record->record.q_record.rw;

    if (action == READ_ACTION) {
        if (idx_rw == WRITE_IDX) {
            q_value[queue].write = value;
        } else {
            // Sampler always get read idx first for TH queues
            q_value[queue].read = -1;
            q_value[queue].write = -1;
            q_value[queue].service = -1;

            q_value[queue].read = value;
        }
    } else { /* UPDATE_ACTION */
        q_value[queue].service = value;
        if ((queue == HIP4_MIF_Q_TH_CTRL) || (queue == HIP4_MIF_Q_FH_RFB)) {
            /*
             * For HIP4_MIF_Q_TH_CTRL and HIP4_MIF_Q_FH_RFB, we always drain all packets in buffer.
             */
            if (q_value[queue].read != -1 && q_value[queue].write != -1 &&
                q_value[queue].service == q_value[queue].write) {
                q_record_list[queue].push(std::make_tuple((long long int)record->ts,
                                          (uint8_t)q_value[queue].write - (uint8_t)q_value[queue].read,
                                          (uint8_t)q_value[queue].service - (uint8_t)q_value[queue].read));
            }
        } else { /* queue ==  HIP4_MIF_Q_TH_DAT */
            if (q_value[queue].read != -1 && q_value[queue].write != -1 &&
                (uint8_t)q_value[queue].service != -1) {
                q_record_list[queue].push(std::make_tuple((long long int)record->ts,
                                         (uint8_t)q_value[queue].write - (uint8_t)q_value[queue].read,
                                         (uint8_t)q_value[queue].service - (uint8_t)q_value[queue].read));
            }
        }
    }
}

bool Hip4Profile::update_q_record(struct hip4_record* record) {
    if (record->record.q_record.queue >= HIP4_MIF_Q_MAX) {
        return false;
    }
    switch (record->record.q_record.queue) {
        case HIP4_MIF_Q_FH_CTRL:
        case HIP4_MIF_Q_FH_DAT:
        case HIP4_MIF_Q_TH_RFB:
        update_fh_queue(record);
        break;
        case HIP4_MIF_Q_TH_CTRL:
        case HIP4_MIF_Q_TH_DAT:
        case HIP4_MIF_Q_FH_RFB:
        update_th_queue(record);
        break;
        default:
        break;
    }
    return true;
}

bool Hip4Profile::update_qmcod_record(struct hip4_record* record) {
    uint32_t key = 0;
    if (record->record.qmcod_record.type != HIP4_SAMPLER_BOT_QMOD_TX &&
        record->record.qmcod_record.type != HIP4_SAMPLER_BOT_QMOD_RX) {
        return false;
    }
    key = (record->record.qmcod_record.vif << 16 | record->record.qmcod_record.peer_index << 8 |
           record->record.qmcod_record.pri);
    if (record->record.qmcod_record.type == HIP4_SAMPLER_BOT_QMOD_TX) {
        qmcod_tx_record_list[key].push(std::make_tuple(
            (long long int)record->ts,
            (uint16_t)record->record.qmcod_record.qmod,
            (uint16_t)record->record.qmcod_record.qcod));
    } else {
        qmcod_rx_record_list[key].push(std::make_tuple(
            (long long int)record->ts,
            (uint16_t)record->record.qmcod_record.qmod,
            (uint16_t)record->record.qmcod_record.qcod));
    }
    return true;
}

bool Hip4Profile::update_mbulk_record(struct hip4_record* record) {
    if (record->record.mbulk_record.mbulk_type != HIP4_SAMPLER_MBULK) {
        return false;
    }
    if (record->record.mbulk_record.clas >= MBULK_CLASS_MAX) {
        return false;
    }
    mbulk_record_list[record->record.mbulk_record.clas].push(std::make_tuple((long long int)record->ts,
                                                             (uint16_t)record->record.mbulk_record.free_cnt));
    return true;
}

FILE * open_hip4_sample_log(const char * filename) {
    return  fopen (filename, "r");
}

bool Hip4Profile::read_hip4_record(FILE* f, struct hip4_record* record, int offset_end) {
    if (offset_end) {
        if (lseek(fileno(f), 0, SEEK_CUR) >= offset_end) {
            return false;
        }
    }
    return sizeof(struct hip4_record) == read(fileno(f), (void*)record, sizeof(struct hip4_record));
}

int Hip4Profile::start(const char* input_file, const char* output_file, hip4_source_type_t type) {
    struct sable_commen_hdr sable_common_header;
    struct sable_version_specific_hdr sable_version_specific_header;
    struct hip4_record record;
    int end_offset = 0;
    uint8_t idx = 0;

    input = open_hip4_sample_log(input_file);
    if (!input) {
        ALOGE("Fail to open file: %s", input_file);
        return -1;
    }

    if (type == SABLE_TYPE) {
        if (parse_sable_header(input, &sable_common_header)) {
            if (!parse_sable_version_specific_header(input, &sable_common_header, &sable_version_specific_header)) {
                fclose(input);
                return -1;
            }
            if (!find_hip4_chunk(input, &sable_common_header, &end_offset)) {
                fclose(input);
                return -1;
            }
        }
    }

    output = create_file(output_file);
    if (!output) {
        ALOGE("Fail to create file: %s", output_file);
        fclose(input);
        return -1;
    }

    for (uint8_t q = 0 ; q < HIP4_MIF_Q_MAX ; q++) {
        q_value[q].read = -1;
        q_value[q].write = -1;
        q_value[q].service = -1;
    }
    while (read_hip4_record(input, &record, end_offset)) {
        update_cpu_freq_record(&record);
        if (update_q_record(&record)) {
            continue;
        } else if (update_qmcod_record(&record)) {
            continue;
        } else if (update_mbulk_record(&record)) {
            continue;
        }
    }

    // Output results.
    json_open_array(output);

    // >>> Start CPU
    json_open_chart(output, "Freq");
    for (uint8_t cpu = 0 ; cpu < CPU_MAX ; cpu++) {
        json_open_label(output, (std::string("CPU")+std::to_string(cpu)).c_str(), cpu);
        while (!cpu_record_list[cpu].empty()) {
            const std::tuple<long long int, uint16_t> entry = cpu_record_list[cpu].front();
            cpu_record_list[cpu].pop();
            json_add_data(output, std::to_string((double)std::get<0>(entry)/(double)1000000000L).c_str(),
                          std::to_string((std::get<1>(entry))*1000).c_str(), !cpu_record_list[cpu].empty());
        }
        json_close_label(output, (cpu != CPU_MAX -1));
    }
    json_close_chart(output, true);
    // <<< End CPU

    // >>> Start Queue
    json_open_chart(output, "Queue");
    for (uint8_t q = 0 ; q < HIP4_MIF_Q_MAX ; q++) {
        json_open_label(output, Q_INDEX_TO_NAME[q], q);
        while (!q_record_list[q].empty()) {
            const std::tuple<long long int, uint8_t, uint8_t> entry = q_record_list[q].front();
            q_record_list[q].pop();
            json_add_data(output, std::to_string((double)std::get<0>(entry)/(double)1000000000L).c_str(),
                          std::to_string(std::get<1>(entry)).c_str(), !q_record_list[q].empty());
        }
        json_close_label(output, q < (HIP4_MIF_Q_MAX - 1));
    }
    json_close_chart(output, true);
    // <<< End Queue

    // >>> Start QMODTX
    json_open_chart(output, "QMODTx");
    idx = 0;
    for (std::map<uint32_t,
         std::queue<std::tuple<long long int, uint16_t, uint16_t>>>::iterator it = qmcod_tx_record_list.begin();
         it != qmcod_tx_record_list.end(); ++it) {
        const uint32_t key = it->first;
        const uint8_t vif = ((key>>16) & 0xff);
        const uint8_t peer_index = ((key>>8) & 0xff);
        const uint8_t pri = ((key) & 0xff);

        json_open_label(output,
            (std::string("Vif")+std::to_string(vif)+
                std::string("Peer")+std::to_string(peer_index)+
                std::string("Pri")+std::to_string(pri)).c_str(), idx);
        while (!it->second.empty()) {
            const std::tuple<long long int, uint16_t, uint16_t> entry = it->second.front();
            it->second.pop();
            json_add_data(output, std::to_string((double)std::get<0>(entry)/(double)1000000000L).c_str(),
                          std::to_string(std::get<2>(entry)).c_str(), !it->second.empty());
        }
        json_close_label(output, ++it != qmcod_tx_record_list.end());
        --it;
        idx++;
    }
    json_close_chart(output, true);
    // <<< End QMODTX

    // >>> Start QMODRX
    json_open_chart(output, "QMODRx");
    idx = 0;

    std::map<uint32_t, std::queue<std::tuple<long long int, uint16_t, uint16_t>>>::iterator
        it = qmcod_rx_record_list.begin();
    for ( ; it != qmcod_rx_record_list.end(); ++it) {
        const uint32_t key = it->first;
        const uint8_t vif = ((key>>16) & 0xff);
        const uint8_t peer_index = ((key>>8) & 0xff);
        const uint8_t pri = ((key) & 0xff);

        json_open_label(output,
            (std::string("Vif")+std::to_string(vif)+
                std::string("Peer")+std::to_string(peer_index)+
                std::string("Pri")+std::to_string(pri)).c_str(), idx);
        while (!it->second.empty()) {
            const std::tuple<long long int, uint16_t, uint16_t> entry = it->second.front();
            it->second.pop();
            json_add_data(output, std::to_string((double)std::get<0>(entry)/(double)1000000000L).c_str(),
                          std::to_string(std::get<2>(entry)).c_str(), !it->second.empty());
        }
        json_close_label(output, ++it != qmcod_rx_record_list.end());
        --it;
        idx++;
    }
    json_close_chart(output, true);
    // <<< End QMODRX

    // >>> Start MBULK
    json_open_chart(output, "Mbulk");
    for (uint8_t clas = 0 ; clas < MBULK_CLASS_MAX ; clas++) {
        json_open_label(output, MBULK_CLASS_TO_NAME[clas], clas);
        while (!mbulk_record_list[clas].empty()) {
            const std::tuple<long long int, uint16_t> entry = mbulk_record_list[clas].front();
            mbulk_record_list[clas].pop();
            json_add_data(output, std::to_string((double)std::get<0>(entry)/(double)1000000000L).c_str(),
                          std::to_string(std::get<1>(entry)).c_str(), !mbulk_record_list[clas].empty());
        }
        json_close_label(output, clas < (MBULK_CLASS_MAX - 1));
    }
    json_close_chart(output, false);
    // <<< End MBULK
    json_close_array(output, false);
    fclose(input);
    fclose(output);
    return 0;
}
