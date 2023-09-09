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

#ifndef __SITRILl_EXTERN_TEST_GPS_H__
#define __SITRILl_EXTERN_TEST_GPS_H__

#include "sitrilExternTest.h"
#include <gtest/gtest.h>


class SitrilExternApiGps : public SitrilExternApi {
public:
    int (*ril_register_callback)(int *);
    int (*RequestAGPS)(unsigned int, void*, int, void*, int);
    int (*GpsSetFreqAiding)(void*, int, void*, int);
    int (*GpsGetLppSuplEcidInfo)(void*, int, void*, int);
    int (*GpsGetRrlpSuplEcidInfo)(void*, int, void*, int);
    int (*IndGpsMeasurePositionRsp)(void*, int, void*, int);
    int (*IndReleaseGps)(void*, int, void*, int);
    int (*GpsMoLocationReq)(void*, int, void*, int);
    int (*IndGpsMTLocationReq)(void*, int, void*, int);
    int (*GpsGetLppReqServingCellInfo)(void*, int, void*, int);
    int (*LppProvideCapabilitiesInd)(void*, int, void*, int);
    int (*IndLppRequestAssistData)(void*, int, void*, int);
    int (*LppProvideLocationInfoInd)(void*, int, void*, int);
    int (*LppGpsErrorInd)(void*, int, void*, int);
    int (*IndSuplLppDataInfo)(void*, int, void*, int);
    int (*IndSuplNiMessage)(void*, int, void*, int);
    int (*SetGpsSuplNiReady)(void*, int, void*, int);
    int (*SetGanssMeasPosRsp)(void*, int, void*, int);
    int (*SetGpsLockMode)(void*, int, void*, int);
    int (*GetRefLocation)(void*, int, void*, int);
    int (*SetPseudoRangeMeas)(void*, int, void*, int);
    int (*GetCdmaPreciseTimeAidingInfo)(void*, int, void*, int);
    int (*IndGanssApPosCapRsp)(void*, int, void*, int);
    int (*GetGsmExtInfoMsg)(void*, int, void*, int);
    int (*GpsControlPlaneEnable)(void*, int, void*, int);
    int (*GnssLppProfileSet)(void*, int, void*, int);
    int (*GetGpsCdmaFreqAiding)(void*, int, void*, int);
    int (*GpsRetrieveLocInfo)(void*, int, void*, int);

    SitrilExternApiGps();
};

class SitrilExternTestGps : public ::testing::TestWithParam<int> {
  protected:
      int mPhoneIndex;
      SitrilExternApiGps mApi;
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
