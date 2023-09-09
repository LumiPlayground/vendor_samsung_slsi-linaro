/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#pragma once

#include <string>

struct OperatorInfo {
    std::string alphaLong;
    std::string alphaShort;
    std::string numeric;
    int mcc;
    int mnc;
    OperatorInfo()
        : alphaLong(""), alphaShort(""), numeric(""),
          mcc(INT_MAX), mnc(INT_MAX) {}
    bool operator==(const OperatorInfo &o) {
        return (alphaLong == o.alphaLong) &&
               (alphaShort == o.alphaShort) &&
               (numeric == o.numeric) &&
               (mcc == o.mcc) &&
               (mnc == o.mnc);
    }
    bool operator!=(const OperatorInfo &o) {
        return !(*this == o);
    }
    bool valid() {
        return *this != OperatorInfo();
    }
    static OperatorInfo getOperatorInfo(const std::string &alphaLong,
                                        const std::string &alphaShort, const std::string &numeric);
};

class NetworkOperator {
  public:
    NetworkOperator();
    NetworkOperator(const char *_plmn, const char *_longPlmn, const char *_shortPlmn, int _regState, int _lac, const char *_gid);
    NetworkOperator(const std::string &_plmn, const std::string &_longPlmn, const std::string &_shortPlmn, int _regState, int _lac, const std::string &_gid);

    void SetValues(const std::string &_plmn, const std::string &_longPlmn, const std::string &_shortPlmn, int _regState, int _lac, const std::string &_gid1);
    std::string PrintToString() const;

    std::string GetAlphaLong() const;
    std::string GetAlphaShort() const;
    std::string GetNumeric() const;
    std::string GetGid1() const;
    int GetMcc() const;
    int GetMnc() const;
    int GetRegState() const;
    int GetLac() const;
    bool IsRegNot() const;
    bool IsRegHome() const;
    bool IsRegStateInRegRange() const;
    bool IsNumericAvailable() const;
    bool IsNumericValid() const;
    bool AreBothLongPlmnShortPlmnAvailable() const;
    int GetTypeOfLongEnhancement() const;
    int GetTypeOfShortEnhancement() const;
    bool IsLongPlmnAvailable() const;
    bool IsShortPlmnAvailable() const;

  private:
    OperatorInfo opInfo;
    int regState;
    int lac;
    std::string gid1;
};

class SimOperator {
  public:
    SimOperator(const std::string &_plmn, const std::string &_spn, const std::string &_iccid);

    std::string PrintToString() const;
    std::string GetNumeric() const;
    std::string GetSpn() const;
    std::string GetIccid() const;
    bool IsNumericAvailable() const;

  private:
    std::string numericPlmn;
    std::string spn;
    std::string iccid;
};
