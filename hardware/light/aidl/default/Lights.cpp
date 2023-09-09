/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Lights.h"

#include <android-base/logging.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <log/log.h>

#define LOGD ALOGD
#define LOGE ALOGE

namespace aidl {
namespace android {
namespace hardware {
namespace light {

char const*const LCD_FILE_PATHS[] = {
    "/sys/devices/virtual/backlight/panel0-backlight/brightness",
    "/sys/class/backlight/panel0-backlight/brightness",
    NULL
};

enum {
    LCD = 0,
    LIGHTS_FD_COUNT
};

static const char *const* g_lights_file_paths[] = {
    LCD_FILE_PATHS,
};

static int g_lights_fds[LIGHTS_FD_COUNT];

static void initialize_fds() {
    int i, j = 0;

    for (i = 0 ; i < LIGHTS_FD_COUNT ; i++) {
        int j = 0;
        while(1) {
            LOGD("!@ initialize_fds : g_lights_file_paths[%d][%d] : %s \n", i, j, g_lights_file_paths[i][j]);
            if (g_lights_file_paths[i][j] == NULL) {
                break;
            } else {
                g_lights_fds[i] = open(g_lights_file_paths[i][j], O_RDWR);

                if (g_lights_fds[i] >= 0) {
                    LOGD("!@ initialize_fds : FOUND! : 0x%x\n", g_lights_fds[i]);
                    break;
                } else {
                    g_lights_fds[i] = -errno;
                    LOGD("!@ initialize_fds : g_lights_fds Open error! : [%d]\n", errno);
                }
                j++;
            }
        }
    }
}

static void init_globals(void) {
	initialize_fds();
}

static int write_int(int fd, int value) {
	if (fd >= 0) {
		char buffer[20];
		int bytes = sprintf(buffer, "%d\n", value);

		LOGD("lcd : %d\n", value);
		int amt = write(fd, buffer, bytes);

		return amt == -1 ? -errno : 0;
	} else {
		LOGE("write_int failed to open %d\n", fd);
		return -errno;
	}
}

static int rgb_to_brightness(int rgb) {
	int color = rgb & 0x00ffffff;
	return ((77 * ((color >> 16) & 0x00ff))
		+ (150 * ((color >> 8) & 0x00ff)) + (29 * (color & 0x00ff))) >> 8;
}

ndk::ScopedAStatus Lights::setLightState(int id, const HwLightState& state) {
    /*LOG(INFO) << "Lights setting state for id=" << id << " to color " << std::hex << state.color;*/

    int brightness = rgb_to_brightness(state.color);

    if(id != 0)
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    /*LOG(INFO) << "lcd: " << brightness;*/

    write_int(g_lights_fds[LCD], brightness);

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::getLights(std::vector<HwLight>* lights) {
    LOG(INFO) << "Lights reporting supported lights";

    init_globals();

    HwLight light{};
    light.id = 0;
    light.type = LightType::BACKLIGHT;
    light.ordinal = 0;
    lights->push_back(light);

    return ndk::ScopedAStatus::ok();
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
