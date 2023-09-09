/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "operatorinfo.h"
#include "networkutils.h"
#include <librilutils/textutils.h>
#include <rildef.h>
#include <sstream>

using std::ostringstream;
using std::string;

OperatorInfo OperatorInfo::getOperatorInfo(const std::string &alphaLong,
                                           const std::string &alphaShort, const std::string &numeric) {
    OperatorInfo info;
    info.alphaLong = alphaLong;
    info.alphaShort = alphaShort;
    if (TextUtils::IsDigitsOnly(numeric) &&
        (numeric.length() == 5 || numeric.length() == 6)) {
        info.numeric = numeric;
        info.mcc = std::stoi(numeric.substr(0, 3));
        info.mnc = std::stoi(numeric.substr(3));
    }
    return info;
}

NetworkOperator::NetworkOperator() {
    opInfo = OperatorInfo::getOperatorInfo("", "", "");
    regState = OPERATOR_REG_NOT;
    lac = 0;
}

NetworkOperator::NetworkOperator(const char *_plmn, const char *_longPlmn, const char *_shortPlmn, int _regState, int _lac, const char *_gid1) {
    if (_longPlmn == nullptr && _shortPlmn != nullptr) {
        _longPlmn = _shortPlmn;
    }
    if (_longPlmn != nullptr && _shortPlmn == nullptr) {
        _shortPlmn = _longPlmn;
    }

    SetValues((_plmn == nullptr ? "" : _plmn),
              (_longPlmn == nullptr ? "" : _longPlmn),
              (_shortPlmn == nullptr ? "" : _shortPlmn),
              _regState,
              _lac,
              _gid1);
}

NetworkOperator::NetworkOperator(const string &_plmn, const string &_longPlmn, const string &_shortPlmn, int _regState, int _lac, const string &_gid1) {
    SetValues(_plmn, _longPlmn, _shortPlmn, _regState, _lac, _gid1);
}

void NetworkOperator::SetValues(const string &_plmn, const string &_longPlmn, const string &_shortPlmn, int _regState, int _lac, const string &_gid1) {
    opInfo = OperatorInfo::getOperatorInfo(_longPlmn, _shortPlmn, _plmn);
    regState = _regState;
    lac = _lac;
    gid1 = _gid1;
}

string NetworkOperator::PrintToString() const {
    std::ostringstream oss;
    oss << "PLMN=" << GetNumeric();
    oss << ", Long PLMN=" << GetAlphaLong();
    oss << ", Short PLMN=" << GetAlphaShort();
    oss << ", RegState=" << GetRegState();
    oss << ", lac=" << GetLac();
    oss << ", gid1=" << GetGid1();
    return oss.str();
}

string NetworkOperator::GetAlphaLong() const {
    return opInfo.alphaLong;
}

string NetworkOperator::GetAlphaShort() const {
    return opInfo.alphaShort;
}

string NetworkOperator::GetNumeric() const {
    return opInfo.numeric;
}

int NetworkOperator::GetMcc() const {
    return opInfo.mcc;
}

int NetworkOperator::GetMnc() const {
    return opInfo.mnc;
}

int NetworkOperator::GetRegState() const {
    return regState;
}

int NetworkOperator::GetLac() const {
    return lac;
}

string NetworkOperator::GetGid1() const {
    return gid1;
}

bool NetworkOperator::IsRegNot() const {
    return regState == OPERATOR_REG_NOT;
}

bool NetworkOperator::IsRegHome() const {
    return regState == OPERATOR_REG_HOME;
}

bool NetworkOperator::IsRegStateInRegRange() const {
    return (regState > OPERATOR_REG_NOT && regState <= OPERATOR_REG_UNKNOWN);
}

bool NetworkOperator::IsNumericAvailable() const {
    return !TextUtils::IsEmpty(opInfo.numeric);
}

bool NetworkOperator::IsNumericValid() const {
    if (TextUtils::IsDigitsOnly(opInfo.numeric) == false || !(opInfo.numeric.length() == 5 || opInfo.numeric.length() == 6)) {
        return false;
    }
    return true;
}

bool NetworkOperator::AreBothLongPlmnShortPlmnAvailable() const {
    return IsLongPlmnAvailable() && IsShortPlmnAvailable();
}

int NetworkOperator::GetTypeOfLongEnhancement() const {
    if (IsLongPlmnAvailable()) {
        return NetworkUtils::isLteEnhancedAvailable(opInfo.alphaLong);
    } else {
        return 0;
    }
}

int NetworkOperator::GetTypeOfShortEnhancement() const {
    if (IsShortPlmnAvailable()) {
        return NetworkUtils::isLteEnhancedAvailable(opInfo.alphaShort);
    } else {
        return 0;
    }
}

bool NetworkOperator::IsLongPlmnAvailable() const {
    return !TextUtils::IsEmpty(opInfo.alphaLong);
}

bool NetworkOperator::IsShortPlmnAvailable() const {
    return !TextUtils::IsEmpty(opInfo.alphaShort);
}

SimOperator::SimOperator(const std::string &_plmn, const std::string &_spn, const std::string &_iccid) {
    numericPlmn = _plmn;
    spn = _spn;
    iccid = _iccid;
}

string SimOperator::PrintToString() const {
    ostringstream oss;
    oss << "SIM plmm=" << GetNumeric();
    oss << ", spn=" << GetSpn();
    oss << ", iccid=" << GetIccid();
    return oss.str();
};

string SimOperator::GetNumeric() const {
    return numericPlmn;
}

string SimOperator::GetSpn() const {
    return spn;
}

string SimOperator::GetIccid() const {
    return iccid;
}

bool SimOperator::IsNumericAvailable() const {
    return !TextUtils::IsEmpty(numericPlmn);
}
