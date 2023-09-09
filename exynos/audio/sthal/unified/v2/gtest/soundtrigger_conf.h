/*
 * Copyright (C) 2017 The Android Open Source Project
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
#ifndef __EXYNOS_SOUNDTRIGGERCONF_H__
#define __EXYNOS_SOUNDTRIGGERCONF_H__

/********************************************************************/
/** ALSA Framework Sound Card & Sound Device Information            */
/**                                                                 */
/** You can find Sound Device Name from /dev/snd.                   */
/** Sound Device Name consist of Card Number & Device Number.       */
/**                                                                 */
/********************************************************************/

/* Sound Card and Mixer card Numbers based on Target Device */
/* You have to match this number with real kernel information */
#define VTS_SOUND_CARD          0
#define VTS_MIXER_CARD          0

/* PCM Nodes number for seamless and normal recording*/
#define VTS_TRICAP_DEVICE_NODE         24
#define VTS_RECORD_DEVICE_NODE         25

/* Google model binary size is used, as this is greater then SVoice model size */
#define VTSDRV_MISC_MODEL_BIN_MAXSZ             0x10800

#define AUDIO_PRIMARY_HAL_LIBRARY_RELPATH       "libaudioproxy2.so"

// VTS Capture(Input) PCM Configuration
#define DEFAULT_VTS_CHANNELS          1       // Mono
#define DEFAULT_VTS_SAMPLING_RATE     16000   // 16KHz

#define PRIMARY_VTS_PERIOD_SIZE           160     // 160 frames, 10ms in case of 16KHz Stream
#define PRIMARY_VTS_PERIOD_COUNT          1024       // Buffer count => Total 327680 Bytes = 160 * 1(Mono) * 2(16bit PCM) * 1024(Buffer count)

/* VTS mixer controls */
#define VTS_ACTIVE_KEYPHRASE_CTL_NAME "VTS Active Keyphrase"    /* default: 0-"SVOICE", 1-"GOOGLE", 2-"SENSORY"  */
#define VTS_EXECUTION_MODE_CTL_NAME "VTS Execution Mode"        /* 0-"OFF-MODE", 1-"VOICE-TRIG-MODE", 2-"SOUND-DECTECT-MODE", 3-"VT-ALWAYS-MODE" 4-"GOOGLE-TRI-MODE */
#define VTS_VOICERECOGNIZE_START_CTL_NAME "VTS VoiceRecognize Start" /* 0-"Off", 1-On" */
#define VTS_VOICETRIGGER_VALUE_CTL_NAME "VTS VoiceTrigger Value"  /* 0 ~ 2000 ms*/

#endif  // __EXYNOS_SOUNDTRIGGERCONF_H__
