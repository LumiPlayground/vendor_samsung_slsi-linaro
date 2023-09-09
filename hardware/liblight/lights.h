/*
 * Copyright (C) 2008 The Android Open Source Project
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

#ifndef ANDROID_LIGHTS_INTERFACE_H
#define ANDROID_LIGHTS_INTERFACE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <hardware/hardware.h>

__BEGIN_DECLS

/**
 * The id of this module
 */
#define LIGHTS_HARDWARE_MODULE_ID "lights"

/**
 * Header file version.
 */
#define LIGHTS_HEADER_VERSION   1

/**
 * Device API version 0.0-1.0
 *
 * Base version for the device API in the lights HAL: all versions less than
 * 2.0 are treated as being this version.
 */
#define LIGHTS_DEVICE_API_VERSION_1_0   HARDWARE_DEVICE_API_VERSION_2(1, 0, LIGHTS_HEADER_VERSION)

/**
 * Device API version 2.0
 *
 * Devices reporting this version or higher may additionally support the
 * following modes:
 * - BRIGHTNESS_MODE_LOW_PERSISTENCE
 */
#define LIGHTS_DEVICE_API_VERSION_2_0   HARDWARE_DEVICE_API_VERSION_2(2, 0, LIGHTS_HEADER_VERSION)

/*
 * These light IDs correspond to logical lights, not physical.
 * So for example, if your INDICATOR light is in line with your
 * BUTTONS, it might make sense to also light the INDICATOR
 * light to a reasonable color when the BUTTONS are lit.
 */
#define LIGHT_ID_BACKLIGHT          "backlight"
//+ Dual Screen
#define LIGHT_ID_BACKLIGHT_SUB      "backlight_sub"
//- Dual Screen
#define LIGHT_ID_KEYBOARD           "keyboard"
#define LIGHT_ID_BUTTONS            "buttons"
//+ Dual Screen
#define LIGHT_ID_BUTTONS_SUB      "buttons_sub"
//- Dual Screen
#define LIGHT_ID_BATTERY            "battery"
#define LIGHT_ID_NOTIFICATIONS      "notifications"
#define LIGHT_ID_ATTENTION          "attention"
#define LIGHT_ID_LED_SERVICE        "ledservice"
#define LIGHT_ID_SMART_GLOW_SERVICE "smartglowservice"

/*
 * These lights aren't currently supported by the higher
 * layers, but could be someday, so we have the constants
 * here now.
 */
#define LIGHT_ID_BLUETOOTH          "bluetooth"
#define LIGHT_ID_WIFI               "wifi"

/**
 * The parameters that can be set for a given light.
 *
 * Not all lights must support all parameters.  If you
 * can do something backward-compatible, you should.
 */
struct light_state_t {
    /**
     * The color of the LED in ARGB.
     *
     * Do your best here.
     *   - If your light can only do red or green, if they ask for blue,
     *     you should do green.
     *   - If you can only do a brightness ramp, then use this formula:
     *      unsigned char brightness = ((77*((color>>16)&0x00ff))
     *              + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
     *   - If you can only do on or off, 0 is off, anything else is on.
     *
     * The high byte should be ignored.  Callers will set it to 0xff (which
     * would correspond to 255 alpha).
     */
    unsigned int color;
};

struct light_device_t {
    struct hw_device_t common;

    /**
     * Set the provided lights to the provided values.
     *
     * Returns: 0 on succes, error code on failure.
     */
    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);
};

__END_DECLS

#endif  // ANDROID_LIGHTS_INTERFACE_H

