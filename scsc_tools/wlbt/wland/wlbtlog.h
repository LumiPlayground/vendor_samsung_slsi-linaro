/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef SLSI_WLBTLOG_H
#define SLSI_WLBTLOG_H

class WlbtLog {
private:
    static bool bt_logging_running;
    static bool mxlog_running;
    static bool udilog_running;
    static void set_cpu_affinity();
    static int set_log_level(const char* file, const char* value);

public:
    static bool start_mxlog(const char* prefix, const char* data);
    static bool start_udilog(const char* prefix, const char* data);
    static bool start_hip4_profile(const char* output_file, const char* data);
    static bool start_bt_normal_log(const char* prefix, const char* data);
    static bool start_bt_audio_log(const char* prefix, const char* data);
    static bool start_bt_custom_log(const char* prefix, const char* data);

    static void setup_wifi_log_filter();
    static void revert_wifi_log_filter();

    static void setup_bt_normal_log_filter();
    static void setup_bt_audio_log_filter();
    static void setup_bt_custom_log_filter(const char* filter);
    static void revert_bt_log_filter();

    static bool stop_mxlog(const char* prefix, const char* data);
    static bool stop_udilog(const char* prefix, const char* data);
    static bool stop_bt_log(const char* prefix, const char* data);
};

#endif
