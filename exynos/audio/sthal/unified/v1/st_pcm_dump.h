/*
 * Copyright (C) 2020 The Android Open Source Project
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

#ifndef _ST_PCM_DUMP_H_
#define _ST_PCM_DUMP_H_

#define ST_PCM_DUMP_PATH "/data/vendor/log/vts/"

void st_pcm_dump_init(struct pcm_config config, char *file_name);
void st_pcm_dump_write(void *buffer, unsigned int size);
void st_pcm_dump_close(struct pcm *pcm);

#endif  // _ST_PCM_DUMP_H_

