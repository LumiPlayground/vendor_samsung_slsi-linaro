/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#pragma once

#include <slsi/radio_v1_6.h>
#include <optional>
#include <string>
#include <sstream>
#include <vector>
#include <librilutils/textutils.h>

#define RIL_REQUEST_ENABLE_VONR             225
#define RIL_REQUEST_IS_VONR_ENABLED         226
#define RIL_REQUEST_SET_USAGE_SETTING       227
#define RIL_REQUEST_GET_USAGE_SETTING       228

namespace android::hardware::radio::impl::modem::V2_0 {

struct RIL_ActivityStatsTechSpecificInfo {
    RIL_RadioAccessNetworks rat;
    int32_t frequencyRange;
    std::vector<int32_t> txmModetimeMs;
    int32_t rxModeTimeMs;

    inline bool operator==(const RIL_ActivityStatsTechSpecificInfo& o) const {
        return std::tie(rat, frequencyRange, txmModetimeMs, rxModeTimeMs) ==
                std::tie(o.rat, o.frequencyRange, o.txmModetimeMs, o.rxModeTimeMs);
    }
};

struct RIL_ActivityStatsInfo {
    int32_t sleepModeTimeMs;
    int32_t idleModeTimeMs;
    std::vector<RIL_ActivityStatsTechSpecificInfo> techSpecificInfo;

    inline bool operator==(const RIL_ActivityStatsInfo& o) const {
        return std::tie(sleepModeTimeMs, idleModeTimeMs, techSpecificInfo) ==
                std::tie(o.sleepModeTimeMs, o.idleModeTimeMs, o.techSpecificInfo);
    }
};

}

namespace android::hardware::radio::impl::sim::V2_0 {

struct RIL_CarrierRestrictions : RIL_CarrierRestrictionsWithPriority {
    /**
     * Whether this is a carrier restriction with priority or not.
     * If this is false, allowedCarriersPrioritized is not applicable.
     */
    bool priority;
};

struct RIL_CardStatus : RIL_CardStatus_V1_4 {
    int portId;
};

}

namespace android::hardware::radio::impl::network::V2_0 {

struct RIL_NitzTime {
    std::string nitzTime;
    int64_t age;
    int64_t timestamp;
};

struct RIL_SignalThresholdInfo {
    int32_t signalMeasurement;
    int32_t hysteresisMs;
    int32_t hysteresisDb;
    std::vector<int32_t> thresholds;
    bool isEnabled;
    int32_t ran;

    inline bool operator==(const RIL_SignalThresholdInfo& o) const {
        return std::tie(signalMeasurement, hysteresisMs, hysteresisDb,
                        thresholds, isEnabled, ran) ==
               std::tie(o.signalMeasurement, o.hysteresisMs, o.hysteresisDb,
                        o.thresholds, o.isEnabled, o.ran);
    }

    inline std::string toString() const {
        std::ostringstream os;
        os << "RIL_SignalThresholdInfo{";
        os << "signalMeasurement: " << signalMeasurement;
        os << ", hysteresisMs: " << hysteresisMs;
        os << ", hysteresisDb: " << hysteresisDb;
        os << ", thresholds: [";
        for (auto th : thresholds) {
            os << th << ",";
        }
        os << "]";
        os << ", isEnabled: " << isEnabled;
        os << ", ran: " << ran;
        os << "}";
        return os.str();
    }

    inline bool isValid() const {
        size_t num = thresholds.size();
        if (num > 1) {
            for (size_t i = 0; i < num - 1; i++) {
                if (hysteresisDb < 0 ||
                    thresholds[i] > thresholds[i+1] ||
                    thresholds[i+1] - thresholds[i] < hysteresisDb) {
                    return false;
                }
            }
        }
        return true;
    }
};

struct RIL_UsageSetting {
    enum class Value : int32_t {
        VOICE_CENTRIC = 1,
        DATA_CENTRIC = 2,
    };

    RIL_UsageSetting() = default;
    constexpr RIL_UsageSetting(int32_t usageSetting) : mUsageSetting(Value(usageSetting)) {}
    constexpr RIL_UsageSetting(Value usageSetting) : mUsageSetting(usageSetting) {}
    constexpr bool valid() const {
        return (mUsageSetting == Value::VOICE_CENTRIC) ||
                (mUsageSetting == Value::DATA_CENTRIC);
    }
    Value value() const { return mUsageSetting; }
private:
    Value mUsageSetting;
};

}

namespace android::hardware::radio::impl::voice::V2_0 {

}

namespace android::hardware::radio::impl::data::V2_0 {

/**
 * This struct represents the OSId + OSAppId as defined in TS 24.526 Section 5.2
 */
#define MAX_OS_ID_LENGTH 16
#define MAX_OS_APP_ID_LENGTH 255
struct RIL_OSAppId {
    /**
     * Byte array representing OSId + OSAppId. The minimum length of the array is
     * 18 and maximum length is 272 (16 bytes for OSId + 1 byte for OSAppId length
     * + up to 255 bytes for OSAppId).
     */
    uint8_t value[MAX_OS_ID_LENGTH + 1 + MAX_OS_APP_ID_LENGTH];
    inline bool operator==(const RIL_OSAppId &rhs) const {
        if (this == &rhs) return true;
        return memcmp(value, rhs.value, sizeof(value)) == 0;
    }
};

/**
 * This struct represents a traffic descriptor. A valid struct must have at least
 * one of the optional values present. This is based on the definition of traffic
 * descriptor in TS 24.526 Section 5.2.
 */
struct RIL_TrafficDescriptor {
    /**
     * DNN stands for Data Network Name and represents an APN as defined in
     * 3GPP TS 23.003.
     */
    // For backward compatibility to original AOSP, only index 0 will be used
    std::vector<std::string> dnn;
    /**
     * Indicates the OSId + OSAppId (used as category in Android).
     */
    // For backward compatibility to original AOSP, only index 0 will be used
    std::vector<RIL_OSAppId> osAppIds;
    RIL_TrafficDescriptor() {
        clear();
    }
    bool empty() const {
        return dnn.empty() && osAppIds.empty();
    }

public:
    inline std::string toString() const {
        std::stringstream ss;
        ss << "RIL_TrafficDescriptor:{";
        for (size_t i = 0; i < dnn.size(); i++) {
            ss << " dnn[" << i << "]: " << dnn[i];
        }
        ss << "}";
        return ss.str();
    }

    /* Extended for specific carrier requirement */
    // This will be filled with HAL value then used when filling SIT payload
    // just when IpDescriptor and it's bitmasks are present only
    uint32_t tdMask;

    std::optional<RIL_IpDescriptors> ipDescriptors;

    std::vector<RIL_IpDescriptors> ip3Tuple;

    std::vector<RIL_DomainDescriptors> domainDescriptors;

    std::vector<RIL_ConnectionCapabilities> connectionCapabilities;

    /* Extended for full spec. This is used for GetSlicingConfig Response */
    // IPDescriptors => Ipv4RemoteAddress[4], Ipv4RemoteAddressMask[4], Ipv6RemoteAddress[16], PrefixLength[1]
    //                  SingleRemotePort[2], RemotePortRangeLow[2], RemotePortRangeHigh[2], ProtocolIdentifier[1]
    // DestinationFQDN[255] -> domainDescriptors
    // CONNECTION_CAPABILITIES[1] -> connectionCapabilities

    // SecurityParameterIndex[4]
    std::vector<uint32_t> securityParameterIndex;
    // TOS_TC[1]
    std::vector<uint8_t> tosTc;
    // TOS_TCM[1]
    std::vector<uint8_t> tosTcm;
    // FlowLabel[3]
    std::vector<RIL_FlowLabel> flowLabel;
    // DestMacAddress[6]
    std::vector<RIL_MacAddress> destMacAddress;
    // CTAGVID[2]
    std::vector<uint16_t> cTagVid;
    // STAGVID[2]
    std::vector<uint16_t> sTagVid;
    // CTAGPCPDEI[1]
    std::vector<uint8_t> cTagPcpDei;
    // STAGPCPDEI[1]
    std::vector<uint8_t> sTagPcpDei;
    // ETHERTYPE[2]
    std::vector<uint16_t> etherType;
    // MatchAllType[1]
    uint8_t matchAllType;
    void clear() {
        dnn.clear();
        osAppIds.clear();
        matchAllType = 0;
        tdMask = 0;
        ipDescriptors.reset();
        ip3Tuple.clear();
        domainDescriptors.clear();
        connectionCapabilities.clear();
        securityParameterIndex.clear();
        tosTc.clear();
        tosTcm.clear();
        flowLabel.clear();
        destMacAddress.clear();
        cTagVid.clear();
        sTagVid.clear();
        cTagPcpDei.clear();
        sTagPcpDei.clear();
        etherType.clear();
    }
    virtual ~RIL_TrafficDescriptor() {
        clear();
    }
    RIL_IpDescriptors &getIpDescriptorsWithInit() {
        if (!ipDescriptors.has_value()) {
            ipDescriptors = RIL_IpDescriptors{ };
        }
        return ipDescriptors.value();
    }
    RIL_TrafficDescriptor& operator=(const RIL_TrafficDescriptor& other) {
        if (this == &other) return *this;

        this->dnn = other.dnn;
        if (!other.osAppIds.empty()) {
            RIL_OSAppId osappid{};
            memcpy(osappid.value, other.osAppIds[0].value, sizeof(other.osAppIds[0].value));
            this->osAppIds.push_back(osappid);
        }
        matchAllType = other.matchAllType;
        tdMask = other.tdMask;
        ipDescriptors = other.ipDescriptors;
        ip3Tuple = other.ip3Tuple;
        domainDescriptors = other.domainDescriptors;
        connectionCapabilities = other.connectionCapabilities;
        securityParameterIndex = other.securityParameterIndex;
        tosTc = other.tosTc;
        tosTcm = other.tosTcm;
        flowLabel = other.flowLabel;
        destMacAddress = other.destMacAddress;
        cTagVid = other.cTagVid;
        sTagVid = other.sTagVid;
        cTagPcpDei = other.cTagPcpDei;
        sTagPcpDei = other.sTagPcpDei;
        etherType = other.etherType;

        return *this;
    }
    bool operator==(const RIL_TrafficDescriptor& other) const {
        if (this == &other) return true;

        return (dnn == other.dnn) &&
            (osAppIds == other.osAppIds) &&
            (matchAllType == other.matchAllType) &&
            (tdMask == other.tdMask) &&
            (ipDescriptors == other.ipDescriptors) &&
            (ip3Tuple == other.ip3Tuple) &&
            (domainDescriptors == other.domainDescriptors) &&
            (connectionCapabilities == other.connectionCapabilities) &&
            (securityParameterIndex == other.securityParameterIndex) &&
            (tosTc == other.tosTc) &&
            (tosTcm == other.tosTcm) &&
            (flowLabel == other.flowLabel) &&
            (destMacAddress == other.destMacAddress) &&
            (cTagVid == other.cTagVid) &&
            (sTagVid == other.sTagVid) &&
            (cTagPcpDei == other.cTagPcpDei) &&
            (sTagPcpDei == other.sTagPcpDei) &&
            (etherType == other.etherType);
    }
};

struct RIL_UrspRule {
    /**
     * Precedence value in the range of 0 to 255. Higher value has lower
     * precedence.
     */
    uint8_t precedence;
    /**
     * Used as a matcher for network requests.
     */
    std::vector<RIL_TrafficDescriptor> trafficDescriptors;
    /**
     * List of routes (connection parameters) that must be used for requests
     * matching a trafficDescriptor.
     */
    int RSDNum;
    std::vector<RIL_RouteSelectionDescriptor> routeSelectionDescriptor;

};

/*
* This struct represents the current slicing configuration.
* includes extended TrafficDescriptor in UrspRule
*/
struct RIL_SlicingConfig {
    /**
     * This vector contains the current URSP rules. Empty vector represents that no
     * rules are configured.
     */
    int urspNum;
    std::vector<RIL_UrspRule> urspRules;

    /**
     * List of all slices.
     */
    int sliceInfoNum;
    std::vector<RIL_SliceInfo> sliceInfo;
};

struct RIL_DataProfileInfo {
   /** ID of the data profile. */
    RIL_DataProfileId profileId;

    /** The APN name. */
    std::string apn;

    /** PDP_type values. */
    PdpProtocolType protocol;

    /** PDP_type values used on roaming network. */
    PdpProtocolType roamingProtocol;

    /** APN authentication type. */
    RIL_ApnAuthType authType;

    /** The username for APN, or empty string. */
    std::string user;

    /** The password for APN, or empty string. */
    std::string password;

    /** Data profile technology type. */
    RIL_DataProfileInfoType type;

    /** The period in seconds to limit the maximum connections. */
    int32_t maxConnsTime;

    /** The maximum connections during maxConnsTime. */
    int32_t maxConns;

    /**
     * The required wait time in seconds after a successful UE initiated disconnect of a given PDN
     * connection before the device can send a new PDN connection request for that given PDN.
     */
    int32_t waitTime;

    /** True to enable the profile, false to disable. */
    bool enabled;

    /** Supported APN types bitmap. See ApnTypes for the value of each bit. */
    uint32_t supportedApnTypesBitmap;

    /** The bearer bitmap. See RadioAccessFamily for the value of each bit. */
    uint32_t bearerBitmap;

    /** Maximum transmission unit (MTU) size in bytes for IPv4. */
    int32_t mtuV4;

    /** Maximum transmission unit (MTU) size in bytes for IPv6. */
    int32_t mtuV6;

    /**
     * True if this data profile was used to bring up the last default (i.e internet) data
     * connection successfully.
     */
    bool preferred;

    /**
     * If true, modem must persist this data profile and profileId must not be
     * set to DataProfileId.INVALID. If the same data profile exists, this data profile must
     * overwrite it.
     */
    bool persistent;
    bool alwaysOn;
    std::optional<RIL_TrafficDescriptor> tdInfo;

public:
    inline std::string toString() const{
        std::stringstream ss;
        ss << "RIL_DataProfileInfo_V2_0:{";
        ss << " apn=" << apn;
        ss << " supportedApnTypesBitmap=" << supportedApnTypesBitmap;
        ss << " mtuV4=" << mtuV4;
        ss << " mtuV6=" << mtuV6;
        ss << " tdInfo=" << tdInfo->toString();
        ss << "}";
        return ss.str();
    }

    RIL_DataProfileInfo() {
        this->apn = "";
        this->user = "";
        this->password = "";
        this->profileId = DEFAULT;
        this->protocol = PDP_PROTOCOL_TYPE_UNKNOWN;
        this->roamingProtocol = PDP_PROTOCOL_TYPE_UNKNOWN;
        this->authType = NO_PAP_NO_CHAP;
        this->type = COMMON;
        this->maxConnsTime = 0;
        this->maxConns = 0;
        this->waitTime = 0;
        this->enabled = false;
        this->supportedApnTypesBitmap = 0;
        this->bearerBitmap = 0;
        this->mtuV4 = 0;
        this->mtuV6 = 0;
        this->preferred = false;
        this->persistent = false;
        this->alwaysOn = false;

        this->tdInfo = { };
    }

    RIL_DataProfileInfo(const RIL_DataProfileInfo & other){
        this->apn = other.apn;
        this->user = other.user;
        this->password = other.password;
        this->profileId = other.profileId;
        this->protocol = other.protocol;
        this->roamingProtocol = other.roamingProtocol;
        this->authType = other.authType;
        this->type = other.type;
        this->maxConnsTime = other.maxConnsTime;
        this->maxConns = other.maxConns;
        this->waitTime = other.waitTime;
        this->enabled = other.enabled;
        this->supportedApnTypesBitmap = other.supportedApnTypesBitmap;
        this->bearerBitmap = other.bearerBitmap;
        this->mtuV4 = other.mtuV4;
        this->mtuV6 = other.mtuV6;
        this->preferred = other.preferred;
        this->persistent = other.persistent;
        this->alwaysOn = other.alwaysOn;

        this->tdInfo = other.tdInfo;
    }

    RIL_DataProfileInfo& operator=(const RIL_DataProfileInfo& other) {
        if (this == &other) return *this;

        this->apn = other.apn;
        this->user = other.user;
        this->password = other.password;
        this->profileId = other.profileId;
        this->protocol = other.protocol;
        this->roamingProtocol = other.roamingProtocol;
        this->authType = other.authType;
        this->type = other.type;
        this->maxConnsTime = other.maxConnsTime;
        this->maxConns = other.maxConns;
        this->waitTime = other.waitTime;
        this->enabled = other.enabled;
        this->supportedApnTypesBitmap = other.supportedApnTypesBitmap;
        this->bearerBitmap = other.bearerBitmap;
        this->mtuV4 = other.mtuV4;
        this->mtuV6 = other.mtuV6;
        this->preferred = other.preferred;
        this->persistent = other.persistent;
        this->alwaysOn = other.alwaysOn;

        this->tdInfo = other.tdInfo;

        return *this;
    }
};

struct RIL_LinkAddress {
    /**
     * The format is IP address with optional "/"
     * prefix length (The format is defined in RFC-4291 section 2.3). For example, "192.0.1.3",
     * "192.0.1.11/16", or "2001:db8::1/64". Typically one IPv4 or one IPv6 or one of each. If
     * the prefix length is absent, then the addresses are assumed to be point to point with
     * IPv4 with prefix length 32 or IPv6 with prefix length 128.
     */
    std::string address;

    /**
     * The properties of the link address
     */
    int32_t properties;

    /**
     * The UTC time that this link address will be deprecated. 0 indicates this information is not
     * available.
     */
    uint64_t deprecationTime;

    /**
     * The UTC time that this link address will expire and no longer valid. 0 indicates this
     * information is not available.
     */
    uint64_t expirationTime;

    RIL_LinkAddress() = default;

    RIL_LinkAddress(const RIL_LinkAddress& other) :
        address(other.address),
        properties(other.properties),
        deprecationTime(other.deprecationTime),
        expirationTime(other.expirationTime) {
    }

    RIL_LinkAddress(const std::string &addr, int32_t prop, uint64_t depT, uint64_t expT) :
        address(addr), properties(prop), deprecationTime(depT), expirationTime(expT) {
    }

    RIL_LinkAddress& operator=(const RIL_LinkAddress& other) {
        if (this == &other) return *this;

        address = other.address;
        properties = other.properties;
        deprecationTime = other.deprecationTime;
        expirationTime = other.expirationTime;

        return *this;
    }

};

struct RIL_SetupDataCallInfo {
    // V1.5
    int accessNetwork;
    RIL_DataProfileInfo dataProfileInfo; // extended at V2.0
    bool roamingAllow;
    int reason;
    std::vector<RIL_LinkAddress> linkAddresses;
    std::vector<std::string> dnses;
    // V1.6
    int32_t pduSessionId;
    std::optional<RIL_SliceInfo> sliceInfo;
    std::optional<RIL_TrafficDescriptor> tdInfo;
    bool matchAllRuleAllowed;
    /* Extended */
    std::optional<RIL_VerifyDescriptor> vdInfo;
    // V2.0
    bool mAlwaysOnPdu;
};

}

namespace DataV2_0 = android::hardware::radio::impl::data::V2_0;
