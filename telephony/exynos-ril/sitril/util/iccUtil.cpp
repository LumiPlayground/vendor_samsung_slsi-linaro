/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "iccUtil.h"
#include "util.h"
#include "rillog.h"
#include "constdef.h"
#include <slsi/radio_v1_6.h>
#include "simautoverifypinhandler.h"

#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/*
 * SimTlv
 */
SimTlv::SimTlv() {
    mRecord = NULL;
    mTlvLength = 0;
    mCurOffset = 0;
    mCurDataOffset = 0;
    mCurDataLength = 0;
    mHasValidTlvObject = false;
}

SimTlv::SimTlv(BYTE *record, int offset, int length) : SimTlv() {
    if(record != NULL && length > 0)
    {
        mRecord = new BYTE[length];
        memcpy(mRecord, record+offset, length);

        mTlvLength = length;
        mHasValidTlvObject = parseCurrentTlvObject();
    }
}

SimTlv::~SimTlv() {
    if(mRecord != NULL) delete[] mRecord;
}

bool SimTlv::nextObject() {
    if (!mHasValidTlvObject) return false;
    mCurOffset = mCurDataOffset + mCurDataLength;
    mHasValidTlvObject = parseCurrentTlvObject();
    return mHasValidTlvObject;
}

/**
 * Returns data associated with current TLV object
 * returns null if !isValidObject()
 */
BYTE *SimTlv::getData() {
    if (!mHasValidTlvObject) return NULL;
    if (mCurDataLength <= 0) return NULL;

    BYTE *ret = new BYTE[mCurDataLength];
    memcpy(ret, mRecord + mCurDataOffset, mCurDataLength);
    return ret;
}

/**
 * Updates curDataLength and curDataOffset
 * @return false on invalid record, true on valid record
*/
bool SimTlv::parseCurrentTlvObject() {

    if (mCurOffset >= mTlvLength) return false;

    // 0x00 and 0xff are invalid tag values
    if (mRecord[mCurOffset] == 0 || (mRecord[mCurOffset] & 0xff) == 0xff) {
        return false;
    }

    if (mCurOffset + 1 < mTlvLength) {
        if ((mRecord[mCurOffset + 1] & 0xff) < 0x80) {
            // one byte length 0 - 0x7f
            mCurDataLength = mRecord[mCurOffset + 1] & 0xff;
            mCurDataOffset = mCurOffset + 2;
        } else if ((mRecord[mCurOffset + 1] & 0xff) == 0x81) {
            // two byte length 0x80 - 0xff
            if ( mCurOffset + 2 <  mTlvLength) {
                mCurDataLength = mRecord[mCurOffset + 2] & 0xff;
                mCurDataOffset = mCurOffset + 3;
            }
            else {
                return false;
            }
        } else {
            return false;
        }
    }
    else {
        return false;
    }

    if (mCurDataLength + mCurDataOffset > mTlvLength) {
        return false;
    }

    return true;
}


/*
 * IccUtil
 */
const char IccUtil::HEX_CHARS[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int IccUtil::hexCharToInt(char ch) {
    if (ch >= '0' && ch <= '9')
        return (int)(ch - '0');
    else if (ch >= 'a' && ch <= 'f')
        return (int)(ch - 'a') + 10;
    else if (ch >= 'A' && ch <= 'F')
        return (int)(ch - 'A') + 10;
    return -1;
}

/**
 * Converts a byte array into a String of hexadecimal characters.
 *
 * @param bytes an array of bytes
 *
 * @return hex string representation of bytes array
 */
std::string IccUtil::bytesToHexString(BYTE *data, int length) {
    if (data == NULL || length <= 0) return std::string("");

    char *buf = new char[length*2 + 1]{};

    int j = 0;
    for (int i = 0; i < length; i++) {
        int b;
        b = 0x0f & (data[i] >> 4);
        buf[j++] = HEX_CHARS[b];

        b = 0x0f & data[i];
        buf[j++] = HEX_CHARS[b];
    }
    std::string ret(buf, j);

    delete [] buf;
    return ret;
}

/**
 * PLMN (MCC/MNC) is encoded as per 24.008 10.5.1.3
 * Returns a concatenated string of MCC+MNC, stripping
 * all invalid character 'F'
 */
std::string IccUtil::bcdPlmnToString(BYTE *data, int offset, int length) {
    if (offset + 3 > length) {
        return NULL;
    }
    BYTE *trans = new BYTE[3];
    trans[0] = (BYTE) ((data[0 + offset] << 4) | ((data[0 + offset] >> 4) & 0xF));
    trans[1] = (BYTE) ((data[1 + offset] << 4) | (data[2 + offset] & 0xF));
    trans[2] = (BYTE) ((data[2 + offset] & 0xF0) | ((data[1 + offset] >> 4) & 0xF));
    std::string ret = bytesToHexString(trans, 3);

    // For a valid plmn we trim all character 'F'
    for (int i = ret.size() - 1; i >= 0; --i) {
        if(ret.at(i) == 'F') ret.erase(i);
        else break;
    }

    delete [] trans;
    return ret;
}

/**
 * Converts a series of bytes to an integer. This method currently only supports positive 32-bit
 * integers.
 *
 * @param src The source bytes.
 * @param offset The position of the first byte of the data to be converted. The data is base
 *     256 with the most significant digit first.
 * @param length The length of the data to be converted. It must be <= 4.
 * @throws IllegalArgumentException If {@code length} is bigger than 4 or {@code src} cannot be
 *     parsed as a positive integer.
 * @throws IndexOutOfBoundsException If the range defined by {@code offset} and {@code length}
 *     exceeds the bounds of {@code src}.
 */
int IccUtil::bytesToInt(BYTE *src, int srcLength, int offset, int length) {
    if (length > 4) {
        // length must be <= 4 (only 32-bit integer supported):
        return 0;
    }

    if (offset < 0 || length < 0 || offset + length > srcLength) {
        //Out of the bounds
        return 0;
    }
    int result = 0;
    for (int i = 0; i < length; i++) {
        result = (result << 8) | (src[offset + i] & 0xFF);
    }
    if (result < 0) {
        //src cannot be parsed as a positive integer
        return 0;
    }
    return result;
}

/**
 * Convert a TS 24.008 Section 10.5.3.5a Network Name field to a string
 * "offset" points to "octet 3", the coding scheme byte
 * empty string returned on decode error
 */
std::string IccUtil::networkNameToString(BYTE *data, int offset, int length) {
    std::string ret = std::string("");
    char dstBuff[MAX_FULL_NAME_LEN] = { 0, };
    int decLen = 0;

    if ((data[offset] & 0x80) != 0x80 || length < 1) {
        return ret;
    }

    int countSeptets;
    int unusedBits =  data[offset] & 7;
    countSeptets = ((((length - 1) * 8) - unusedBits) + 4) / 7;

    switch ((data[offset] >> 4) & 0x7) {
        case 0:
            decLen = DecodingNetworkNameFromSIM(0, data+offset+1, length-1, countSeptets, (unsigned char*)dstBuff, sizeof(dstBuff));
            if (decLen > 0) ret = std::string(dstBuff, decLen);
        break;
        case 1:
            decLen = DecodingNetworkNameFromSIM(1, data+offset+1, length-1, countSeptets, (unsigned char*)dstBuff, sizeof(dstBuff));
            if (decLen > 0) ret = std::string(dstBuff, decLen);
        break;

        // unsupported encoding
        default:
        break;
    }

    // "Add CI"
    // "The MS should add the letters for the Country's Initials and
    //  a separator (e.g. a space) to the text string"

    if ((data[offset] & 0x40) != 0) {
        // FIXME(mkf) add country initials here
    }

    return ret;
}

std::string IccUtil::adnStringToUtf8String(unsigned char *data, int length) {
    std::string ret = std::string("");
    char dstBuff[MAX_FULL_NAME_LEN] = { 0, };

    int decLen = DecodingPbNameFromSIM(UCS2, data, length, (unsigned char *) dstBuff, sizeof(dstBuff));
    if (decLen > 0) ret = std::string(dstBuff, decLen);

    return ret;
}

string IccUtil::covertCardStateToString(RIL_CardState card_state)
{
    switch(card_state) {
        case RIL_CARDSTATE_ABSENT:
            return "RIL_CARDSTATE_ABSENT";
        case RIL_CARDSTATE_PRESENT:
            return "RIL_CARDSTATE_PRESENT";
        case RIL_CARDSTATE_ERROR:
            return "RIL_CARDSTATE_ERROR";
        case RIL_CARDSTATE_RESTRICTED:
            return "RIL_CARDSTATE_RESTRICTED";
        default:
            return "";
    }
}

string IccUtil::covertUiccEnableStateToString(int uicc_enable_state)
{
    switch(uicc_enable_state) {
        case SimService::UICC_APP_STATE_DISABLED:
            return "UICC_APP_STATE_DISABLED";
        case SimService::UICC_APP_STATE_ENABLED:
            return "UICC_APP_STATE_ENABLED";
        default:
            return "";
    }
}

string IccUtil::covertAutoPinStateToString(int auto_pin_state)
{
    switch(auto_pin_state) {
        case SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLED:
            return "AUTO_PIN_STATE_DISABLED";
        case SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLING:
            return "AUTO_PIN_STATE_DISABLING";
        case SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED:
            return "AUTO_PIN_STATE_ENABLED";
        case SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING:
            return "AUTO_PIN_STATE_ENABLING";
        case SimAutoVerifyPinHandler::AUTO_PIN_STATE_CP_CRASH:
            return "AUTO_PIN_STATE_CP_CRASH";
        case SimAutoVerifyPinHandler::AUTO_PIN_STATE_RECOVERY:
            return "AUTO_PIN_STATE_RECOVERY";
        default:
            return "";
    }
}

string IccUtil::covertPersoSubStateToString(RIL_PersoSubstate persosub_state)
{
    switch(persosub_state) {
        case RIL_PERSOSUBSTATE_UNKNOWN:
            return "RIL_PERSOSUBSTATE_UNKNOWN";
        case RIL_PERSOSUBSTATE_IN_PROGRESS:
            return "RIL_PERSOSUBSTATE_IN_PROGRESS";
        case RIL_PERSOSUBSTATE_READY:
            return "RIL_PERSOSUBSTATE_READY";
        case RIL_PERSOSUBSTATE_SIM_NETWORK:
            return "RIL_PERSOSUBSTATE_SIM_NETWORK";
        case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET:
            return "RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET";
        case RIL_PERSOSUBSTATE_SIM_CORPORATE:
            return "RIL_PERSOSUBSTATE_SIM_CORPORATE";
        case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER:
            return "RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER";
        case RIL_PERSOSUBSTATE_SIM_SIM:
            return "RIL_PERSOSUBSTATE_SIM_SIM";
        case RIL_PERSOSUBSTATE_SIM_NETWORK_PUK:
            return "RIL_PERSOSUBSTATE_SIM_NETWORK_PUK";
        case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK:
            return "RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK";
        case RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK:
            return "RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK";
        case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK:
            return "RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK";
        case RIL_PERSOSUBSTATE_SIM_SIM_PUK:
            return "RIL_PERSOSUBSTATE_SIM_SIM_PUK";
        case RIL_PERSOSUBSTATE_RUIM_NETWORK1:
            return "RIL_PERSOSUBSTATE_RUIM_NETWORK1";
        case RIL_PERSOSUBSTATE_RUIM_NETWORK2:
            return "RIL_PERSOSUBSTATE_RUIM_NETWORK2";
        case RIL_PERSOSUBSTATE_RUIM_HRPD:
            return "RIL_PERSOSUBSTATE_RUIM_HRPD";
        case RIL_PERSOSUBSTATE_RUIM_CORPORATE:
            return "RIL_PERSOSUBSTATE_RUIM_CORPORATE";
        case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER:
            return "RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER";
        case RIL_PERSOSUBSTATE_RUIM_RUIM:
            return "RIL_PERSOSUBSTATE_RUIM_RUIM";
        case RIL_PERSOSUBSTATE_RUIM_NETWORK1_PUK:
            return "RIL_PERSOSUBSTATE_RUIM_NETWORK1_PUK";
        case RIL_PERSOSUBSTATE_RUIM_NETWORK2_PUK:
            return "RIL_PERSOSUBSTATE_RUIM_NETWORK2_PUK";
        case RIL_PERSOSUBSTATE_RUIM_HRPD_PUK:
            return "RIL_PERSOSUBSTATE_RUIM_HRPD_PUK";
        case RIL_PERSOSUBSTATE_RUIM_CORPORATE_PUK:
            return "RIL_PERSOSUBSTATE_RUIM_CORPORATE_PUK";
        case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK:
            return "RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK";
        case RIL_PERSOSUBSTATE_RUIM_RUIM_PUK:
            return "RIL_PERSOSUBSTATE_RUIM_RUIM_PUK";
        default:
            return "";
    }
}

string IccUtil::covertAppStateToString(RIL_AppState app_state)
{
    switch(app_state) {
        case RIL_APPSTATE_UNKNOWN:
            return "RIL_APPSTATE_UNKNOWN";
        case RIL_APPSTATE_DETECTED:
            return "RIL_APPSTATE_DETECTED";
        case RIL_APPSTATE_PIN:
            return "RIL_APPSTATE_PIN";
        case RIL_APPSTATE_PUK:
            return "RIL_APPSTATE_PUK";
        case RIL_APPSTATE_SUBSCRIPTION_PERSO:
            return "RIL_APPSTATE_SUBSCRIPTION_PERSO";
        case RIL_APPSTATE_READY:
            return "RIL_APPSTATE_READY";
        default:
            return "";
    }
}

string IccUtil::covertPinStateToString(RIL_PinState pin_state)
{
    switch(pin_state) {
        case RIL_PINSTATE_UNKNOWN:
            return "RIL_PINSTATE_UNKNOWN";
        case RIL_PINSTATE_ENABLED_NOT_VERIFIED:
            return "RIL_PINSTATE_ENABLED_NOT_VERIFIED";
        case RIL_PINSTATE_ENABLED_VERIFIED:
            return "RIL_PINSTATE_ENABLED_VERIFIED";
        case RIL_PINSTATE_DISABLED:
            return "RIL_PINSTATE_DISABLED";
        case RIL_PINSTATE_ENABLED_BLOCKED:
            return "RIL_PINSTATE_ENABLED_BLOCKED";
        case RIL_PINSTATE_ENABLED_PERM_BLOCKED:
            return "RIL_PINSTATE_ENABLED_PERM_BLOCKED";
        default:
            return "";
    }
}

string IccUtil::covertAppTypeToString(RIL_AppType app_type)
{
    switch(app_type) {
        case RIL_APPTYPE_UNKNOWN:
            return "RIL_APPTYPE_UNKNOWN";
        case RIL_APPTYPE_SIM:
            return "RIL_APPTYPE_SIM";
        case RIL_APPTYPE_USIM:
            return "RIL_APPTYPE_USIM";
        case RIL_APPTYPE_RUIM:
            return "RIL_APPTYPE_RUIM";
        case RIL_APPTYPE_CSIM:
            return "RIL_APPTYPE_CSIM";
        case RIL_APPTYPE_ISIM:
            return "RIL_APPTYPE_ISIM";
        default:
            return "";
    }
}

