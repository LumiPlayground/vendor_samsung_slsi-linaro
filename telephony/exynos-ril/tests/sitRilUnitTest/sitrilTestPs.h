/*
 *  SIT RIL Unit test
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

#ifndef __SITRILl_TEST_PS_H__
#define __SITRILl_TEST_PS_H__

#include "sitrilTest.h"
#include <gtest/gtest.h>
#include <sitdef.h>
#include <modemdata.h>

class SitrilTestPs : public ::testing::TestWithParam<int> {
  protected:
      int mPhoneIndex;
      SitrilTest *mSitRilTest;

      /* static */
      static void SetUpTestSuite() {}
      static void TearDownTestSuite() {}

  public:
      virtual void SetUp() override;
      virtual void TearDown() override;

      SitrilTest *getSitrilTest() { return mSitRilTest; }

      template <typename T>
      void testRilRequest(const int testRilCmdId, T &testSetupDataReq);
      void testRilRequest(const int testRilCmdId, char *(&testDeactDataReq)[2]);
      void testRilRequest(const int testRilCmdId, nullptr_t);
      void validateRilRequest(UINT &sitToken, const unsigned int sitId, const unsigned int sizeOfModemData);
      void testRilResponse(const int testRilCmdId, const unsigned int sitId, const UINT sitToken);
      void validateRilResponse(int halVersion);
      void validateNullRilResponse();
};
#endif
