/*
 *  SIT RIL network Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#ifndef __SITRILl_EXTERN_TEST_AUDIO_H__
#define __SITRILl_EXTERN_TEST_AUDIO_H__

#include "sitrilExternTest.h"
#include <gtest/gtest.h>


class SitrilExternApiAudio : public SitrilExternApi {
public:
    int (*ril_register_callback)(void *, int *);
    int (*ril_set_audio_volume)(int);
    int (*ril_set_audio_path)(int);
    int (*ril_set_multi_mic)(int);
    int (*ril_set_mute)(int);
    int (*ril_set_audio_clock)(int);
    int (*ril_set_audio_loopback)(int, int);
    int (*ril_set_tty_mode)(int);

    SitrilExternApiAudio();
};

class SitrilExternTestAudio : public ::testing::TestWithParam<int> {
  protected:
      int mPhoneIndex;
      SitrilExternApiAudio mApi;
      SitrilExternTest *mSitRilExternTest;

      /* static */
      static void SetUpTestSuite() {}
      static void TearDownTestSuite() {}

  public:
      virtual void SetUp() override;
      virtual void TearDown() override;

      SitrilExternTest *getSitrilExternTest() { return mSitRilExternTest; }
};
#endif
