/* copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __RADIO_V1_6_H__
#define __RADIO_V1_6_H__

#include <slsi/radio_v1_5.h>
#include <vector>
#include <string>
#include <sstream>

enum RIL_Errno_V1_6 {
    RIL_E_SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED = 67,   /* 1X voice and SMS are not allowed simulteneously */
    RIL_E_ACCESS_BARRED = 68,                           /* Access is barred. */
    RIL_E_BLOCKED_DUE_TO_CALL = 69,                     /* SMS is blocked due to call control */
    RIL_E_RF_HARDWARE_ISSUE = 70,                       /* Detect such RFFE problem */
    RIL_E_NO_RF_CALIBRATION_INFO = 71,                  /* No RF calibration */
};

struct RIL_QosBandwidth {
    /** Maximum bit rate possible on the bearer */
    int32_t maxBitrateKbps;
    /** Minimum bit rate that is guaranteed to be provided by the network */
    int32_t guaranteedBitrateKbps;
};

/** LTE/EPS Quality of Service parameters as per 3gpp spec 24.301 sec 9.9.4.3. */
struct RIL_EpsQos {
    /**
     * Quality of Service Class Identifier (QCI), see 3GPP TS 23.203 and 29.212.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t qci;
    RIL_QosBandwidth downlink;
    RIL_QosBandwidth uplink;
};

/** 5G Quality of Service parameters as per 3gpp spec 24.501 sec 9.11.4.12 */
struct RIL_NrQos {
    /**
     * 5G QOS Identifier (5QI), see 3GPP TS 24.501 and 23.501.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t fiveQi;
    RIL_QosBandwidth downlink;
    RIL_QosBandwidth uplink;
    /**
     * QOS flow identifier of the QOS flow description in the
     * range of QosFlowIdRange::MIN to QosFlowIdRange::MAX
     */
    uint8_t qfi;
    uint16_t averagingWindowMs;
};

/** Allowed values for 5G QOS flow identifier */
enum RIL_QosFlowIdRange {
    QOS_FLOW_ID_RANGE_MIN = 1,
    QOS_FLOW_ID_RANGE_MAX = 63
};

/** EPS or NR QOS parameters */
enum RIL_QosType {
    NOT_SET = 0,
    EPS_QOS = 1,
    NR_QOS = 2
};

union RIL_Qos{
    RIL_EpsQos eps;
    RIL_NrQos nr;
};

/**
 * Next header protocol numbers defined by IANA, RFC 5237
 */
enum RIL_QosProtocol {
    /** No protocol specified */
    QOS_PROTOCOL_UNSPECIFIED = -1,
    /** Transmission Control Protocol */
    QOS_PROTOCOL_TCP = 6,
    /** User Datagram Protocol */
    QOS_PROTOCOL_UDP = 17,
    /** Encapsulating Security Payload Protocol */
    QOS_PROTOCOL_ESP = 50,
    /** Authentication Header */
    QOS_PROTOCOL_AH = 51,
};

enum RIL_QosFilterDirection {
    DOWNLINK = 0,
    UPLINK = 1,
    BIDIRECTIONAL = 2,
};

/** Allowed port numbers */
enum RIL_QosPortRange {
    QOS_PORT_RANGE_MIN = 20,
    QOS_PORT_RANGE_MAX = 65535
};

enum RIL_RadioError_v1_6 {
    /** 1X voice and SMS are not allowed simulteneously. */
    SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED = 67,

    /** Access is barred. */
    ACCESS_BARRED = 68,

    /**
     * SMS is blocked due to call control, e.g., resource unavailable
     * in the SMR entity.
     */
    BLOCKED_DUE_TO_CALL = 69,

    /**
     * Returned from setRadioPowerResponse when detecting RF HW issues. Some RF
     * Front-End(RFFE) components like antenna are considered critical for modem
     * to provide telephony service. This RadioError is used when modem detect
     * such RFFE problem.
     */
    RF_HARDWARE_ISSUE = 70,

    /**
     * Returned from setRadioPowerResponse when detecting no RF calibration
     * issue. Unlike RF_HARDWARE_ISSUE, this is a SW problem and no HW repair is
     * needed.
     */
    NO_RF_CALIBRATION_INFO = 71,
};

/**
 * Defines range of ports. start and end are the first and last port numbers
 * (inclusive) in the range. Both start and end are in QosPortRange.MIN to
 * QosPortRange.MAX range. A single port shall be represented by the same
 * start and end value.
 */
struct RIL_PortRange {
    int32_t start;
    int32_t end;
};

/** Port is optional, contains either single port or range of ports */
enum RIL_MaybePortFlag {
    MAYBE_PORT_INVALID = 0,
    MAYBE_PORT_VALID = 1
};

union RIL_MaybePort {
    RIL_PortRange range;
};

/** Type of service value or mask as defined in RFC 1349 */
enum RIL_TypeOfServiceFlag {
    TYPE_OF_SERVICE_INVALID = 0,
    TYPE_OF_SERVICE_VALID = 1
};

union RIL_TypeOfService {
     uint8_t value;
};

/** IPv6 flow label as defined in RFC 6437 */
enum RIL_Ipv6FlowLabelFlag {
    IPV6_FLOW_LABEL_INVALID = 0,
    IPV6_FLOW_LABEL_VALID = 1
};

union RIL_Ipv6FlowLabel {
     uint32_t value;
};

/** IPSec security parameter index */
enum RIL_IpsecSpiFlag {
    IP_SEC_SPI_INVALID = 0,
    IP_SEC_SPI_VALID = 1
};

union RIL_IpsecSpi {
     uint32_t value;
};

/** See 3gpp 24.008 10.5.6.12 and 3gpp 24.501 9.11.4.13 */
struct RIL_QosFilter {
    /**
     * Local and remote IP addresses, typically one IPv4 or one IPv6
     * or one of each. Addresses could be with optional "/" prefix
     * length, e.g.,"192.0.1.3" or "192.0.1.11/16 2001:db8::1/64".
     * If the prefix length is absent the addresses are assumed to be
     * point to point with IPv4 having a prefix length of 32 and
     * IPv6 128.
     */
    std::vector<std::string> localAddresses;
    std::vector<std::string> remoteAddresses;

    /** Local and remote port/ranges */
    RIL_MaybePortFlag localPortFlag;
    RIL_MaybePort localPort;
    RIL_MaybePortFlag remotePortFlag;
    RIL_MaybePort remotePort;

    /** QoS protocol */
    RIL_QosProtocol protocol;

    /** Type of service value or mask as defined in RFC 1349 */
    RIL_TypeOfServiceFlag tosFlag;
    int8_t tos;

    /** IPv6 flow label as defined in RFC 6437 */
    RIL_Ipv6FlowLabelFlag flowLabelFlag;
    RIL_Ipv6FlowLabel flowLabel;

    /** IPSec security parameter index */
    RIL_IpsecSpiFlag spiFlag;
    RIL_IpsecSpi spi;

    /** Filter direction */
    RIL_QosFilterDirection direction;

    /**
     * Specified the order in which the filter needs to be matched.
     * A lower numerical(positive) value has a higher precedence.
     * Set -1 when unspecified.
     */
    int32_t precedence;
};

/** QOS session associated with a dedicated bearer */
struct RIL_QosSession {
    /** Unique ID of the QoS session within the data call */
    int32_t qosSessionId;

    /** QOS attributes */
    RIL_QosType qosType;
    RIL_Qos qos;

    /** List of QOS filters associated with this session */
    int32_t lenQosFilters;
    std::vector<RIL_QosFilter> qosFilters;
};

/** The allowed failure modes on an IWLAN handover failure. */
enum RIL_HandoverFailureMode {
    /**
     * On data handover failure, fallback to the source data transport when the
     * fail cause is due to a hand off preference change.
     */
    LEGACY = 0,

    /** On data handover failure, fallback to the source data transport. */
    DO_FALLBACK = 1,

    /**
     * On data handover failure, retry the handover instead of falling back to
     * the source data transport.
     */
    NO_FALLBACK_RETRY_HANDOVER = 2,

    /**
     * On data handover failure, setup a new data connection by sending a normal
     * request to the underlying data service.
     */
    NO_FALLBACK_RETRY_SETUP_NORMAL = 3
};

/**
 * Using enum instead of mono_initstate
 */
enum RIL_OptionalInfoFlag {
    RIL_OPT_INFO_INVALID = 0,
    RIL_OPT_INFO_VALID = 1,
};

/**
 * Slice/Service Type as defined in 3GPP TS 23.501.
 */
enum RIL_SliceServiceType : uint8_t {
    /* Not specified */
    SST_NONE = 0,

    /* Slice suitable for the handling of 5G enhanced Mobile Broadband */
    EMBB = 1,

    /**
     * Slice suitable for the handling of ultra-reliable low latency
     * communications
     */
    URLLC = 2,

    /* Slice suitable for the handling of massive IoT */
    MIOT = 3,
};

enum RIL_SliceStatus  : uint8_t{
    TEMP_UNKNOWN = 0,
    CONFIGURED = 1,
    ALLOWED = 2,
    REJECTED_NOT_AVAILABLE_IN_PLMN = 3,
    REJECTED_NOE_AVAILABLE_IN_REG_AREA = 4,
    DEFAULT_CONFIGURED = 5,
};


/**
 * This struct represents a S-NSSAI as defined in 3GPP TS 24.501.
 */
struct RIL_SliceInfo {
    /**
     * The type of service provided by the slice.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    RIL_SliceServiceType sst;

    /**
     * Slice differentiator is the identifier of a slice that has
     * SliceServiceType as SST. A value of -1 indicates that there is
     * no corresponding SliceInfo of the HPLMN.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    int32_t sliceDifferentiator;

    /**
     * This SST corresponds to a SliceInfo (S-NSSAI) of the HPLMN; the SST is
     * mapped to this value.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    RIL_SliceServiceType mappedHplmnSst;

    /**
     * Present only if both sliceDifferentiator and mappedHplmnSst are also
     * present. This SD corresponds to a SliceInfo (S-NSSAI) of the HPLMN;
     * sliceDifferentiator is mapped to this value. A value of -1 indicates that
     * there is no corresponding SliceInfo of the HPLMN.
     *
     * see: 3GPP TS 24.501 Section 9.11.2.8.
     */
    int32_t mappedHplmnSD;

    RIL_SliceStatus status;
};


/**
 * This struct represents the OSId + OSAppId as defined in TS 24.526 Section 5.2
 */
#define MAX_OS_ID_LENGTH 16
#define MAX_OS_APP_ID_LENGTH 255
struct RIL_OptionalOSAppId {
    /**
     * Byte array representing OSId + OSAppId. The minimum length of the array is
     * 18 and maximum length is 272 (16 bytes for OSId + 1 byte for OSAppId length
     * + up to 255 bytes for OSAppId).
     */
    uint8_t value[MAX_OS_ID_LENGTH + MAX_OS_APP_ID_LENGTH + 1];
};


/**
 * This safe_union represents an optional DNN. DNN stands for Data Network Name
 * and represents an APN as defined in 3GPP TS 23.003.
 */
#define MAX_DNN_LENGTH 101
struct RIL_OptionalDNN {
    char value[MAX_DNN_LENGTH];
};

/**
 * This is extended structure definition which is not included in AOSP Radio HAL 1.6
 * This information can be delivered from Modem through updated SIT IPC
 * and addtional information can be delivered upto framework using vendor::radio APIs
 * Original requirements came from specific carrier
 */
struct RIL_IpDescriptors {
    std::vector<std::string> ipv4 = {};
    std::vector<std::string> maskV4 = {};
    std::vector<std::string> ipv6 = {};
    std::vector<int32_t> prefixLength = {};
    std::vector<int32_t> port = {};
    std::vector<int32_t> portStartRange = {};
    std::vector<int32_t> portEndRange = {};
    std::vector<int32_t> protocolId = {};
    void Clear() {
        ipv4.clear();
        maskV4.clear();
        ipv6.clear();
        prefixLength.clear();
        port.clear();
        portStartRange.clear();
        portEndRange.clear();
        protocolId.clear();
    }
    virtual ~RIL_IpDescriptors() {
        Clear();
    }
    RIL_IpDescriptors& operator=(const RIL_IpDescriptors &rhs) {
        if (this == &rhs)  return *this;

        ipv4 = rhs.ipv4;
        maskV4 = rhs.maskV4;
        ipv6 = rhs.ipv6;
        prefixLength = rhs.prefixLength;
        port = rhs.port;
        portStartRange = rhs.portStartRange;
        portEndRange = rhs.portEndRange;
        protocolId = rhs.protocolId;

        return *this;
    }
    bool operator==(const RIL_IpDescriptors &o) const {
        if (this == &o) return true;
        return (ipv4 == o.ipv4) &&
               (maskV4 == o.ipv4) &&
               (ipv6 == o.ipv6) &&
               (prefixLength == o.prefixLength) &&
               (port == o.port) &&
               (portStartRange == o.portStartRange) &&
               (protocolId == o.protocolId);
    }
};

struct RIL_DomainDescriptors {
    std::string value;
    bool operator==(const RIL_DomainDescriptors &o) const {
        if (this == &o) return true;
        return value == o.value;
    }
};


struct RIL_ConnectionCapabilities {
    std::string value;
    bool operator==(const RIL_ConnectionCapabilities &o) const {
        if (this == &o) return true;
        return value == o.value;
    }
};

struct RIL_VerifyDescriptor {
    std::string appCert;
    std::string tdHash;
    bool operator==(const RIL_VerifyDescriptor &o) const {
        if (this == &o) return true;
        return (appCert == o.appCert) &&(tdHash == o.tdHash);
    }
};

/*************/


/**
 * This struct represents a traffic descriptor. A valid struct must have at least
 * one of the optional values present. This is based on the definition of traffic
 * descriptor in TS 24.526 Section 5.2.
 */
struct RIL_TrafficDescriptor_aosp {
    /**
     * DNN stands for Data Network Name and represents an APN as defined in
     * 3GPP TS 23.003.
     */
    RIL_OptionalInfoFlag dnnFlag;
    // For backward compatibility to original AOSP, only index 0 will be used
    std::vector<RIL_OptionalDNN> dnn;
    /**
     * Indicates the OSId + OSAppId (used as category in Android).
     */
    RIL_OptionalInfoFlag osAppIdFlag;
    // For backward compatibility to original AOSP, only index 0 will be used
    std::vector<RIL_OptionalOSAppId> osAppId;
    RIL_TrafficDescriptor_aosp() :
        dnnFlag(RIL_OPT_INFO_INVALID), dnn{}, osAppIdFlag(RIL_OPT_INFO_INVALID), osAppId{} {
    }
    virtual ~RIL_TrafficDescriptor_aosp() {
        dnn.clear();
        osAppId.clear();
    }
    bool isEmpty() const {
        return (dnnFlag == RIL_OPT_INFO_INVALID) && (osAppIdFlag == RIL_OPT_INFO_INVALID);
    }

public:
    inline std::string toString() const{
        std::stringstream ss;
        ss << "RIL_TrafficDescriptor:{";
        for(size_t i =0;i<dnn.size();i++){
            ss << " dnn[" <<i <<"]: "<< dnn[i].value;
        }
        ss << "}";
        return ss.str();
    }

};

struct RIL_FlowLabel {
    char value[3];
    bool operator==(const RIL_FlowLabel &o) const {
        if (this == &o) return true;
        return memcmp(value, o.value, sizeof(value)) == 0;
    }
};


struct RIL_MacAddress {
    char value[6];
    bool operator==(const RIL_MacAddress &o) const {
        if (this == &o) return true;
        return memcmp(value, o.value, sizeof(value)) == 0;
    }
};

struct RIL_TrafficDescriptor : RIL_TrafficDescriptor_aosp {
    /* Extended for specific carrier requirement */
    // This will be filled with HAL value then used when filling SIT payload
    // just when IpDescriptor and it's bitmasks are present only
    uint32_t tdMask;

    // Separate IpDescriptors;
    RIL_OptionalInfoFlag ipDescriptorsFlag;
    RIL_IpDescriptors ipDescriptors;

    // Ip3Tuple
    RIL_OptionalInfoFlag ip3TupleFlag;
    std::vector<RIL_IpDescriptors> ip3Tuple;

    RIL_OptionalInfoFlag domainDescriptorsFlag;
    std::vector<RIL_DomainDescriptors> domainDescriptors;

    RIL_OptionalInfoFlag connectionCapabilitiesFlag;
    std::vector<RIL_ConnectionCapabilities> connectionCapabilities;

    /* Extended for full spec. This is used for GetSlicingConfig Response */
    // IPDescriptors => Ipv4RemoteAddress[4], Ipv4RemoteAddressMask[4], Ipv6RemoteAddress[16], PrefixLength[1]
    //                  SingleRemotePort[2], RemotePortRangeLow[2], RemotePortRangeHigh[2], ProtocolIdentifier[1]
    // DestinationFQDN[255] -> domainDescriptors
    // CONNECTION_CAPABILITIES[1] -> connectionCapabilities

    // SecurityParameterIndex[4]
    RIL_OptionalInfoFlag securityParameterIndexFlag;
    std::vector<uint32_t> securityParameterIndex;
    // TOS_TC[1]
    RIL_OptionalInfoFlag tosTcFlag;
    std::vector<uint8_t> tosTc;
    // TOS_TCM[1]
    std::vector<uint8_t> tosTcm;
    // FlowLabel[3]
    RIL_OptionalInfoFlag flowLabelFlag;
    std::vector<RIL_FlowLabel> flowLabel;
    // DestMacAddress[6]
    RIL_OptionalInfoFlag destMacAddressFlag;
    std::vector<RIL_MacAddress> destMacAddress;
    // CTAGVID[2]
    RIL_OptionalInfoFlag cTagVidFlag;
    std::vector<uint16_t> cTagVid;
    // STAGVID[2]
    RIL_OptionalInfoFlag sTagVidFlag;
    std::vector<uint16_t> sTagVid;
    // CTAGPCPDEI[1]
    RIL_OptionalInfoFlag cTagPcpDeiFlag;
    std::vector<uint8_t> cTagPcpDei;
    // STAGPCPDEI[1]
    RIL_OptionalInfoFlag sTagPcpDeiFlag;
    std::vector<uint8_t> sTagPcpDei;
    // ETHERTYPE[2]
    RIL_OptionalInfoFlag etherTypeFlag;
    std::vector<uint16_t> etherType;
    // MatchAllType[1]
    uint8_t matchAllType;
    void Clear() {
        ipDescriptorsFlag = RIL_OPT_INFO_INVALID;
        ip3TupleFlag = RIL_OPT_INFO_INVALID;
        domainDescriptorsFlag = RIL_OPT_INFO_INVALID;
        connectionCapabilitiesFlag = RIL_OPT_INFO_INVALID;
        securityParameterIndexFlag = RIL_OPT_INFO_INVALID;
        tosTcFlag = RIL_OPT_INFO_INVALID;
        flowLabelFlag = RIL_OPT_INFO_INVALID;
        destMacAddressFlag = RIL_OPT_INFO_INVALID;
        cTagVidFlag = RIL_OPT_INFO_INVALID;
        sTagVidFlag = RIL_OPT_INFO_INVALID;
        cTagPcpDeiFlag = RIL_OPT_INFO_INVALID;
        sTagPcpDeiFlag = RIL_OPT_INFO_INVALID;
        etherTypeFlag = RIL_OPT_INFO_INVALID;
        matchAllType = 0;
        ipDescriptors.Clear();
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
        Clear();
    }
};

struct RIL_SetupDataCallInfo_V1_6 : RIL_SetupDataCallInfo_V1_5 {
    int32_t pduSessionId;
    RIL_OptionalInfoFlag OptionalSliceinfoFlag;
    RIL_SliceInfo sliceInfo;
    RIL_OptionalInfoFlag OptionalTrafficDescriptorFlag;
    RIL_TrafficDescriptor tdInfo;
    bool matchAllRuleAllowed;
    /* Extended */
    RIL_OptionalInfoFlag OptionalVerifyDescriptorFlag;
    RIL_VerifyDescriptor vdInfo;
};

/**
 * Overwritten from @1.5::SetupDataCallResult in order to change the suggestedRetryTime
 * to 64-bit value. In the future, this must be extended instead of overwritten.
 * Also added defaultQos, qosSessions, and handoverFailureMode in this version.
 */
struct RIL_SetupDataCallResult_V1_6 {
    //@1.5::SetupDataCallResult base;

    /** Data call fail cause. DataCallFailCause.NONE if no error. */
    RIL_DataCallFailCause cause;

    /**
     * If status != DataCallFailCause.NONE, this field indicates the suggested retry back-off timer
     * value RIL wants to override the one pre-configured in FW. The unit is milliseconds.
     * The value < 0 means no value is suggested.
     * The value 0 means retry must be done ASAP.
     * The value of INT_MAX(0x7fffffff) means no retry.
     */
    int64_t suggestedRetryTime;

    /** Context ID, uniquely identifies this call. */
    int32_t cid;

    /** Data connection active status. */
    RIL_DataConnActiveStatus active;

    /**
     * PDP_type values. If cause is DataCallFailCause.ONLY_SINGLE_BEARER_ALLOWED, this is the type
     * supported such as "IP" or "IPV6".
     */
    PdpProtocolType type;

    /** The network interface name. */
    char* ifname;

    /**
     * List of link address. Legacy concated addresses is used for addresses only
     */
    //int32_t leng_addresses;
    char* addresses;

    /**
     * List of link address.
     */
    std::vector<RIL_LinkAddress> linkaddresses;

    /**     * List of DNS server addresses, e.g.,
    "192.0.1.3" or "192.0.1.11 2001:db8::1". Empty if no dns     *
    server addresses returned.     */
    std::vector<std::string> dnses;

    /**     * List of default gateway addresses, e.g.,
    "192.0.1.3" or "192.0.1.11 2001:db8::1".     *
    When empty, the addresses represent point to point connections.     */
    std::vector<std::string> gateways;

    /**     * List of default gateway addresses, e.g.,
    "192.0.1.3" or "192.0.1.11 2001:db8::1".     *
    When empty, the addresses represent point to point connections.     */
    std::vector<std::string> pcscf;

    /**
     * MTU received from network for IPv4.
     * Value <= 0 means network has either not sent a value or sent an invalid value.
     */
    int32_t mtuV4;

    /**
     * MTU received from network for IPv6.
     * Value <= 0 means network has either not sent a value or sent an invalid value.
     */
    int32_t mtuV6;

    /** Default bearer QoS. Applicable to LTE and NR */
    RIL_QosType defaultQosType;
    RIL_Qos defaultQos;

    /**
     * Active QOS sessions of the dedicated bearers. Applicable to
     * PDNs that support dedicated bearers.
     */
    int32_t lenQosSessions;
    std::vector<RIL_QosSession> qosSessions;

    /** Specifies the fallback mode on an IWLAN handover failure. */
    RIL_HandoverFailureMode handoverFailureMode;


    /**
     * The allocated pdu session id for this data call.
     * A value of -1 means no pdu session id was attached to this call.
     *
     * Reference: 3GPP TS 24.007 section 11.2.3.1b
     */
    int32_t pduSessionId;


    /**
     * Slice used for this data call. It is valid only when this data call is on
     * AccessNetwork:NGRAN.
     */
    RIL_OptionalInfoFlag sliceInfoFlag;
    RIL_SliceInfo sliceInfo;

    /**
     * TrafficDescriptors for which this data call must be used. It only includes
     * the TDs for which a data call has been requested so far; it is not an
     * exhaustive list.
     */
    std::vector<RIL_TrafficDescriptor> trafficDescriptors;
};

/**
 * Public key type from carrier certificate.
 */
enum PublicKeyType : int32_t {
    EPDG    = 1,                   // Key type to be used for ePDG
    WLAN    = 2,                   // Key type to be used for WLAN
};


typedef struct {
  char * mcc;                         /* MCC of the Carrier. */
  char * mnc ;                        /* MNC of the Carrier. */
  uint8_t * carrierKey;               /* Public Key from the Carrier used to encrypt the
                                       * IMSI/IMPI.
                                       */
  int32_t carrierKeyLength;            /* Length of the Public Key. */
  char * keyIdentifier;               /* The keyIdentifier Attribute value pair that helps
                                       * a server locate the private key to decrypt the
                                       * permanent identity.
                                       */
  int64_t expirationTime;             /* Date-Time (in UTC) when the key will expire. */
  PublicKeyType keyType;           /* Public key type */
} RIL_CarrierInfoForImsiEncryption_1_6;


/**
 * NR Dual connectivity state
 */
enum RIL_NrDualConnectivityState {
    /**
     * Enable NR dual connectivity. Enabled state does not mean dual connectivity
     * is active. It means device is allowed to connect to both primary and secondary.
     */
    ENABLE = 1,
    /**
     * Disable NR dual connectivity. Disabled state does not mean secondary cell is released.
     * Modem will release it only if current bearer is released to avoid radio link failure.
     */
    DISABLE = 2,
    /**
     * Disable NR dual connectivity and force secondary cell to be released if dual connectivity
     * was active. This may result in radio link failure.
     */
    DISABLE_IMMEDIATE= 3,
};

/**
* Overwritten from @1.2::LinkCapacityEstimate to update LinkCapacityEstimate to 1.6 version.
*/
struct RIL_LinkCapacityEstimate_V1_6 {

   /**
    * Estimated downlink capacity in kbps. This bandwidth estimate shall be the estimated
    * maximum sustainable link bandwidth (as would be measured at the Upper PDCP or SNDCP SAP).
    * If the DL Aggregate Maximum Bit Rate is known, this value shall not exceed the DL-AMBR
    * for the Internet PDN connection.
    */
   uint32_t downlinkCapacityKbps;

   /**
    * Estimated uplink capacity in kbps. This bandwidth estimate shall be the estimated=
    * maximum sustainable link bandwidth (as would be measured at the Upper PDCP or SNDCP SAP).
    * If the UL Aggregate Maximum Bit Rate is known, this value shall not exceed the UL-AMBR
    * for the Internet PDN connection.
    */
   uint32_t uplinkCapacityKbps;

   /**
    * Estimated downlink capacity of secondary carrier in a dual connected NR mode in kbps. This bandwidth estimate shall be the estimated
    * maximum sustainable link bandwidth (as would be measured at the Upper PDCP or SNDCP SAP).
    */
   uint32_t DCNRSecondaryDownlinkCapacityKbps;

   /**
    * Estimated uplink capacity secondary carrier in a dual connected NR mode in kbps.. This bandwidth estimate shall be the estimated
    * maximum sustainable link bandwidth (as would be measured at the Upper PDCP or SNDCP SAP).
    */
   uint32_t DCNRSecondaryUplinkCapacityKbps;

};

enum RIL_DataThrottlingAction {
    /* Clear all existing data throttling. */
    NO_DATA_THROTTLING = 0,

    /**
     * Enact secondary carrier data throttling and remove any existing data
     * throttling on anchor carrier.
     */
    THROTTLE_SECONDARY_CARRIER = 1,

    /**
     * Enact anchor carrier data throttling and disable data on secondary
     * carrier if currently enabled.
     */
    THROTTLE_ANCHOR_CARRIER = 2,

    /**
     * Immediately hold on to current level of throttling.
     */
    HOLD = 3
};

struct RIL_DataThrottling {
    RIL_DataThrottlingAction dataThrottlingAction;
    int64_t completionDurationMillis;
};

struct RIL_PhysicalChannelConfig_V1_6 {
    /** Connection status for cell. Valid values are PRIMARY_SERVING and SECONDARY_SERVING */
    RIL_CellConnectionStatus status;

    /** The radio technology for this physical channel */
    RIL_RadioTechnology rat;

    /** Downlink Absolute Radio Frequency Channel Number */
    int32_t downlinkChannelNumber;

    /** Uplink Absolute Radio Frequency Channel Number */
    int32_t uplinkChannelNumber;

    /** Downlink cell bandwidth, in kHz */
    int32_t cellBandwidthDownlink;

    /** Uplink cell bandwidth, in kHz */
    int32_t cellBandwidthUplink;

    /**
     * A list of data calls mapped to this physical channel. The context id must match the cid of
     * @1.5::SetupDataCallResult. An empty list means the physical channel has no data call mapped
     * to it.
     */
    int len_contextIds;
    int *contextIds;

    /**
     * The physical cell identifier for this cell.
     */
    uint32_t physicalCellId;

    /**
     * The frequency band to scan.
     */
    union {
        /** Valid only if radioAccessNetwork = GERAN. */
        RIL_GeranBands geranBand;
        /** Valid only if radioAccessNetwork = UTRAN. */
        RIL_UtranBands utranBand;
        /** Valid only if radioAccessNetwork = EUTRAN. */
        RIL_EutranBands eutranBand;
        /** Valid only if radioAccessNetwork = NGRAN. */
        RIL_NgranBands ngranBand;
    } band;
};

typedef struct {
    RIL_CallState   state;
    int             index;      /* Connection Index for use with, eg, AT+CHLD */
    int             toa;        /* type of address, eg 145 = intl */
    char            isMpty;     /* nonzero if is mpty call */
    char            isMT;       /* nonzero if call is mobile terminated */
    char            als;        /* ALS line indicator if available
                                   (0 = line 1) */
    char            isVoice;    /* nonzero if this is is a voice call */
    char            isVoicePrivacy;     /* nonzero if CDMA voice privacy mode is active */
    char *          number;     /* Remote party number */
    int             numberPresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
    char *          name;       /* Remote party name */
    int             namePresentation; /* 0=Allowed, 1=Restricted, 2=Not Specified/Unknown 3=Payphone */
    RIL_UUS_Info *  uusInfo;    /* NULL or Pointer to User-User Signaling Information */
    RIL_AudioQuality audioQuality;
    char *          forwardedNumber;
} RIL_Call_V1_6;

typedef enum {
    /** IMS voice over PS session not supported */
    VOPS_NOT_SUPPORTED = 0,
    /** IMS voice over PS session supported over 3GPP access */
    VOPS_OVER_3GPP = 1,
    /** IMS voice over PS session supported over non-3GPP access */
    VOPS_OVER_NON_3GPP = 2,
} RIL_VopsIndicator;

typedef enum {
    /** Emergency services not supported */
    EMC_NOT_SUPPORTED = 0,
    /** Emergency services supported in NR connected to 5GCN only */
    EMC_NR_CONNECTED_TO_5GCN = 1,
    /** Emergency services supported in E-UTRA connected to 5GCN only */
    EMC_EUTRA_CONNECTED_TO_5GCN = 2,
    /** Emergency services supported in NR connected to 5GCN and E-UTRA connected to 5GCN */
    EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN = 3
} RIL_EmcIndicator;

typedef enum {
    /** Emergency services fallback not supported */
    EMF_NOT_SUPPORTED = 0,
    /** Emergency services fallback supported in NR connected to 5GCN only */
    EMF_NR_CONNECTED_TO_5GCN = 1,
    /** Emergency services fallback supported in E-UTRA connected to 5GCN only */
    EMF_EUTRA_CONNECTED_TO_5GCN = 2,
    /**
     * Emergency services fallback supported in NR connected to 5GCN and E-UTRA
     * connected to 5GCN.
     */
    EMF_BOTH_NR_EUTRA_CONNECTED_TO_5GCN = 3
} RIL_EmfIndicator;

typedef struct {
    RIL_RegState regState;
    RIL_RadioTechnology rat;
    RIL_RegistrationFailCause reasonForDenial;
    RIL_CellIdentity_V1_5 cellIdentity;
    char registeredPlmn[MAX_REG_PLMN_LEN];
    union {
        struct {
            bool cssSupported;
            int32_t roamingIndicator;
            RIL_PrlIndicator systemIsInPrl;
            int32_t defaultRoamingIndicator;
        } cdmaInfo;
        struct {
            RIL_LteVopsInfo lteVopsInfo;
            RIL_NrIndicators nrIndicators;
        } eutranInfo;
        struct {
            RIL_VopsIndicator vopsSupported;
            RIL_EmcIndicator emcSupported;
            RIL_EmfIndicator emfSupported;
        } ngranInfo;
    } accessTechnologySpecificInfo;
} RIL_RegStateResult_V1_6;

struct RIL_LTE_SignalStrength_V1_6 : RIL_LTE_SignalStrength_v8 {
    uint32_t cqiTableIndex;
};

#define MAX_NR_BAND_INDEX 20
struct RIL_NR_SignalStrength_V1_6 : RIL_NR_SignalStrength_V1_4 {
    uint32_t csiCqiTableIndex;
    int csiCqiReportLen;
    unsigned int csiCqiReport[MAX_NR_BAND_INDEX];
};

typedef struct {
    RIL_GSM_SignalStrength_v12  GSM_SignalStrength;
    RIL_CDMA_SignalStrength     CDMA_SignalStrength;
    RIL_EVDO_SignalStrength     EVDO_SignalStrength;
    RIL_LTE_SignalStrength_V1_6   LTE_SignalStrength;
    RIL_TD_SCDMA_SignalStrength_V1_2 TD_SCDMA_SignalStrength;
    RIL_WCDMA_SignalStrength_V1_2 WCDMA_SignalStrength;
    RIL_NR_SignalStrength_V1_6  NR_SignalStrength;
} RIL_SignalStrength_V1_6;

typedef struct {
    RIL_CellIdentityLte_V1_5 cellIdentityLte;
    RIL_LTE_SignalStrength_V1_6 signalStrengthLte;
    bool isEndcAvailable;
} RIL_CellInfoLte_V1_6;

typedef struct {
    RIL_CellIdentityNr_V1_5 cellIdentityNr;
    RIL_NR_SignalStrength_V1_6 signalStrengthNr;
} RIL_CellInfoNr_V1_6;

typedef struct {
    RIL_CellInfoType  cellInfoType;

    /**
     * True if this cell is registered false if not registered.
     */
    bool registered;
    /**
     * Type of time stamp represented by timeStamp.
     */
    RIL_TimeStampType timeStampType;
    /**
     * Connection status for the cell.
     */
    RIL_CellConnectionStatus connectionStatus;

    union {
        RIL_CellInfoGsm_V1_5 gsm;
        RIL_CellInfoWcdma_V1_5 wcdma;
        RIL_CellInfoTdscdma_V1_5 tdscdma;
        RIL_CellInfoLte_V1_6 lte;
        RIL_CellInfoNr_V1_6 nr;
        /**
         * 3gpp2 CellInfo types;
         */
        RIL_CellInfoCdma_V1_2 cdma;
    } CellInfo;
} RIL_CellInfo_V1_6;

typedef struct {
    RIL_ScanStatus status;
    uint32_t network_infos_length;
    RIL_CellInfo_V1_6* network_infos;
    RIL_Errno error;
} RIL_NetworkScanResult_V1_6;

///////////////////////////////////////////////////////////////////


/**
 * This struct represents the OSId + OSAppId as defined in TS 24.526 Section 5.2
 */
typedef struct{
    std::vector<uint8_t> osAppId;////uint8_t vector in union?
}RIL_OSAppId;


/**
 * This safe_union represents an optional OSAppId.
 */
union OptionalOSAppId {
     RIL_OSAppId value;
};


///////////////////////////////////////////

/**
 * This safe_union represents an optional PdpProtocolType.
 */
union OptionalPdpProtocolType {
    PdpProtocolType value;
};

/**
 * Enum representing session and service continuity mode as defined in
 * 3GPP TS 23.501.
 */
enum SscMode {
    MODE_1 = 1,
    MODE_2 = 2,
    MODE_3 = 3,
};

/**
 * This safe_union represents an optional SscMode.
 */
union OptionalSscMode {
    SscMode value;
};


/* 7 octets, subclause 9.3.1.9 of 3GPP TS 38.413 */
struct RIL_EutraCellIdentities {
    uint8_t value[7];
};

/* 8 octets, subclause 9.3.1.7 of 3GPP TS 38.413 */
struct RIL_NrCellIdentities {
    uint8_t value[8];
};

/* 7 octets, subclause 9.3.1.6 of 3GPP TS 38.413 */
struct RIL_GlobalRanNodeIdentities {
    uint8_t value[7];
};

/**
 * This struct represents a single route selection descriptor as defined in
 * 3GPP TS 24.526.
 */
typedef struct{
    /**
     * Precedence value in the range of 0 to 255. Higher value has lower
     * precedence.
     */
    uint8_t precedence;
    /**
     * Parameters defining this RouteSelectionDescriptor. The length of the vector
     * must be >= 1.
     */

    /**
     * Valid values are IP, IPV6 and IPV4V6.
     */
    OptionalPdpProtocolType sessionType;
    OptionalSscMode sscMode;

    /**
     * Added parameters for RSD from Specification
     */
    uint8_t preferredAccessType;
    uint8_t multiAccessPreferenceType;
    uint8_t nonSeamlessNon3gppOffloadIndication;
    uint8_t timeWindowType[16]; // Starttime, Stoptime, NTP timestamp encoded with IETF RFC5905
    std::vector<RIL_EutraCellIdentities> eutraCellIdentities;
    std::vector<RIL_NrCellIdentities> nrCellIdentities;
    std::vector<RIL_GlobalRanNodeIdentities> globalRanNodeIdentities;
    /**
     * 9.11.3.9 of GPP TS 24.501, length is 0 or 8~113 bytes (except IEI and length of contents)
     */
    std::vector<uint8_t> taiList;

    /**
     * There can be 0 or more SliceInfo specified in a route descriptor.
     */
    int sliceNum;
    std::vector<RIL_SliceInfo> sliceInfo;
    /**
     * DNN stands for Data Network Name and represents an APN as defined in
     * 3GPP TS 23.003. There can be 0 or more DNNs specified in a route
     * descriptor.
     */
    int dnnNum;
    std::vector<RIL_OptionalDNN> dnn;
}RIL_RouteSelectionDescriptor;

typedef struct{
    /**
     * Precedence value in the range of 0 to 255. Higher value has lower
     * precedence.
     */
    uint8_t precedence;
    /**
     * Used as a matcher for network requests.
     */
    std::vector<RIL_TrafficDescriptor_aosp> trafficDescriptors;
    /**
     * List of routes (connection parameters) that must be used for requests
     * matching a trafficDescriptor.
     */
    int RSDNum;
    std::vector<RIL_RouteSelectionDescriptor> routeSelectionDescriptor;

} RIL_UrspRule_aosp;

/*
* This struct represents the current slicing configuration.
*/
typedef struct {
    /**
     * This vector contains the current URSP rules. Empty vector represents that no
     * rules are configured.
     */
    int urspNum;
    std::vector<RIL_UrspRule_aosp> urspRules;

    /**
     * List of all slices.
     */
    int sliceInfoNum;
    std::vector<RIL_SliceInfo> sliceInfo;
} RIL_SlicingConfig_aosp;

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

#define MAX_NAME_LEN 241
#define MAX_NUM_LEN 20
#define EFEXT_LEN 10
#define MAX_EF_SET 4
#define MAX_ANR_LEN 10
#define MAX_PB_COUNT 2000

/**
 * Sim PhoneBook
 */
typedef enum : int32_t {
    PB_INSERT = 1,
    PB_DELETE,
    PB_UPDATE
} PB_OPERATION;

typedef enum {
    PB_EN,      /* 0x0 Emergency Number */
    PB_ADN_2G,  /* 0x1 2G phone book */
    PB_FDN,     /* 0x2 Fixed Dialing Number */
    PB_BDN,     /* 0x3 Barred Dialing Number */
    PB_SDN,     /* 0x4 Service Dialing Number */
    PB_LDN,     /* 0x5 Last Dialed Number */
    PB_ICI,     /* 0x6 Incoming Call INformation */
    PB_OCI,     /* 0x7 Outgoing Call Information */
    PB_MSISDN,  /* 0x8 Own numbers (MSISDMs) list */
    PB_ADN_3G,  /* 0x9 3G phone book */
    PB_AAS,     /* 0xA Additional NUmber Alpha String */
    PB_GAS,     /* 0xB Grouping Information Alpha String */
    PB_MBDN,    /* 0xC Mailbox Dialing Numbers */
}PB_TYPE;

typedef enum {
    UNKNOWN_PLAN = 0,
    TELEPHONY_NUMBERING_PLAN,
    DATA_NUMBERING_PLAN = 3,
    TELEX_NUMBERING_PLAN,
    NATIONAL_NUMBERING_PLAN,
    PRIVATE_NUMBERING_PLAN,
} PB_PLAN_ID;

typedef enum {
    UNKNOWN_TYPE,
    INTERNATIONAL,
    NATIONAL,
    NETWORK,
    DEDICATE,
}PB_NUMBER_TYPE;

typedef enum {
    ASCII = 1,
    GSM7BIT,
    UCS2,
    HEX,
}PB_TEXT_TYPE;

typedef enum {
    NAME = 1,
    NUMBER,
    ANR,
    EMAIL,
    SNE,
    GRP,
    PBC,
    ANRA,
    ANRB,
    ANRC,
    EMAILA,
    EMAILB,
    EMAILC,
    DATA_TIME,
    CALL_DURATION,
    CALL_STATUS,
    LINK_ENTRY,
    CC_PARAM,
    CMI_ALPHA,
    CMI_ID,
    EMS_CAT,
}PB_TYPE_TAG;

enum PbReceivedStatus : int32_t {
    PB_RECEIVED_OK = 1,
    PB_RECEIVED_ERROR = 2,
    PB_RECEIVED_ABORT = 3,
    PB_RECEIVED_FINAL = 4,
};

typedef enum {
    PB_NAME = 0x01,
    PB_NUMBER = 0x02,
    PB_ANR = 0x03,
    PB_EMAIL = 0x04,
    PB_ANRA = 0x08,
    PB_ANRB = 0x09,
    PB_ANRC = 0x0A,
    PB_EMAILA = 0x0B,
    PB_EMAILB = 0x0C,
    PB_EMAILC = 0x0D
} RIL_PhonebookRecordTag;

typedef struct {
    int recordId;
    int emailSetCount;
    int anrSetCount;
    int anrType;
    char name[MAX_NAME_LEN];
    char number[MAX_NUM_LEN + EFEXT_LEN];
    char emails[MAX_EF_SET][MAX_NAME_LEN];
    char additionalNumbers[MAX_EF_SET][MAX_ANR_LEN];
} RIL_PhonebookRecordInfo;

typedef struct {
    int loadedCount;
    int receivedStatus;
    RIL_PhonebookRecordInfo *mPbRec;
} RIL_PhonebookRecInfoResult;

typedef struct {
    int32_t maxAdnRecords;
    int32_t usedAdnRecords;
    int32_t maxEmailRecords;
    int32_t usedEmailRecords;
    int32_t maxAdditionalNumberRecords;
    int32_t usedAdditionalNumberRecords;
    int32_t maxNameLen;
    int32_t maxNumberLen;
    int32_t maxEmailLen;
    int32_t maxAdditionalNumberLen;
} RIL_PhonebookCapacity;

typedef struct {
    int type3g;             /* refer to pb_type_tag */
    int data_len;
    int data_type;
    char *data;
} RIL_3GPb;

typedef struct {
    int type;               /* refer to PB_TYPE */
    int recordId;           /* Lodation where the entry is stored in the storage */
    RIL_3GPb *pb3g;
} RIL_ReadPbEntry;

typedef struct {
    int mode;               /* 0x1: Add  / 0x2: Delete  / 0x3: Edit / 0x4: Delete All */
    int type;               /* refer to PB_TYPE */
    int recordId;
    int length;
    int pb3gCnt;
    RIL_3GPb *pb3g;
} RIL_UpdatePbEntry;

typedef struct {
    int mode;
    int type;
    int recordId;
} RIL_UpdatePbRsp;

typedef struct {
    int pb_type;           /* refer to PB_TYPE */
    int index_min;         /* Minimum index of phonebook entries */
    int index_max;         /* Maximum index of phonebook entries */
    int num_max;           /* Maximum length of number can be saved in phonebook entry */
    int text_max;          /* Maximum character length of test can be saved in phonebook entry */
} RIL_PbEntryInfo;

/***********************************************************************/

///////////////////////////////////////////////////////////////////////////
// legacy RIL requests
///////////////////////////////////////////////////////////////////////////
/**
 * Requests current call list
 *
 * @param serial Serial number of request
 *
 * Response function is IRadioResponse.getCurrentCallsResponse_1_6()
 *
 * Valid errors returned:
 *  SUCCESS
 *  NO_MEMORY
 *  INTERNAL_ERR
 *  SYSTEM_ERR
 *  INVALID_ARGUMENTS
 *  REQUEST_NOT_SUPPORTED
 *  NO_RESOURCES
 *  CANCELLED
 */
#define RIL_REQUEST_GET_CURRENT_CALLS 9

/**
 * Toggle radio on and off (for "airplane" mode)
 * If the radio is turned off/on the radio modem subsystem
 * is expected return to an initialized state. For instance,
 * any voice and data calls must be terminated and all associated
 * lists emptied.
 *
 * When setting radio power on to exit from airplane mode to place an emergency call on this
 * logical modem, powerOn, forEmergencyCall and preferredForEmergencyCall must be true. In
 * this case, this modem is optimized to scan only emergency call bands, until:
 * 1) Emergency call is completed; or
 * 2) Another setRadioPower_1_5 is issued with forEmergencyCall being false or
 * preferredForEmergencyCall being false; or
 * 3) Timeout after 30 seconds if dial or emergencyDial is not called.
 * Once one of these conditions is reached, the modem should move into normal operation.
 *
 * @param serial Serial number of request.
 * @param powerOn To turn on radio -> on = true, to turn off radio -> on = false.
 * @param forEmergencyCall To indication to radio if this request is due to emergency call.
 *      No effect if powerOn is false.
 * @param preferredForEmergencyCall indicate whether the following emergency call will be sent
 *      on this modem or not. No effect if forEmergencyCall is false, or powerOn is false.
 *
 * Response callback is IRadioConfigResponse. setRadioPowerResponse_1_6.
 *
 * Note this API is the same as the 1.5
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   INTERNAL_ERR
 */
#define RIL_REQUEST_RADIO_POWER 23

/**
 * Send an SMS message
 *
 * @param serial Serial number of request.
 * @param message GsmSmsMessage as defined in types.hal
 *
 * Response function is IRadioResponse.sendSmsResponse_1_6()
 *
 * Note this API is the same as the 1.0
 *
 * Based on the return error, caller decides to resend if sending sms
 * fails. RadioError:SMS_SEND_FAIL_RETRY means retry (i.e. error cause is 332)
 * and RadioError:GENERIC_FAILURE means no retry (i.e. error cause is 500)
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   INVALID_ARGUMENTS
 *   INVALID_STATE
 *   SIM_ABSENT
 */
#define RIL_REQUEST_SEND_SMS 25

/**
 * Send an SMS message. Identical to sendSms_1_6,
 * except that more messages are expected to be sent soon. If possible,
 * keep SMS relay protocol link open (eg TS 27.005 AT+CMMS command)
 *
 * @param serial Serial number of request.
 * @param message GsmSmsMessage as defined in types.hal
 *
 * Response function is
 * IRadioResponse.sendSmsExpectMoreResponse_1_6()
 *
 * Note this API is the same as the 1.0
 *
 * Based on the return error, caller decides to resend if sending sms
 * fails. RadioError:SMS_SEND_FAIL_RETRY means retry (i.e. error cause is 332)
 * and RadioError:GENERIC_FAILURE means no retry (i.e. error cause is 500)
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   INVALID_ARGUMENTS
 *   INVALID_STATE
 *   SIM_ABSENT
 */
#define RIL_REQUEST_SEND_SMS_EXPECT_MORE 26

/**
 * Setup a packet data connection. If DataCallResponse.status returns DataCallFailCause:NONE,
 * the data connection must be added to data calls and a unsolDataCallListChanged() must be
 * sent. The call remains until removed by subsequent unsolDataCallIstChanged(). It may be
 * lost due to many factors, including deactivateDataCall() being issued, the radio powered
 * off, reception lost or even transient factors like congestion. This data call list is
 * returned by getDataCallList() and dataCallListChanged().
 *
 * The Radio is expected to:
 *   - Create one data call context.
 *   - Create and configure a dedicated interface for the context.
 *   - The interface must be point to point.
 *   - The interface is configured with one or more addresses and is capable of sending and
 *     receiving packets. The format is IP address with optional "/" prefix length
 *     (The format is defined in RFC-4291 section 2.3). For example, "192.0.1.3",
 *     "192.0.1.11/16", or "2001:db8::1/64". Typically one IPv4 or one IPv6 or one of each. If
 *     the prefix length is absent, then the addresses are assumed to be point to point with
 *     IPv4 with prefix length 32 or IPv6 with prefix length 128.
 *   - Must not modify routing configuration related to this interface; routing management is
 *     exclusively within the purview of the Android OS.
 *   - Support simultaneous data call contexts up to DataRegStateResult.maxDataCalls specified
 *     in the response of getDataRegistrationState.
 *
 * @param serial Serial number of request.
 * @param accessNetwork The access network to setup the data call. If the data connection cannot
 *     be established on the specified access network then it should be responded with an error.
 * @param dataProfileInfo Data profile info.
 * @param roamingAllowed Indicates whether or not data roaming is allowed by the user.
 * @param reason The request reason. Must be DataRequestReason:NORMAL or
 *     DataRequestReason:HANDOVER.
 * @param addresses If the reason is DataRequestReason:HANDOVER, this indicates the list of link
 *     addresses of the existing data connection. This parameter must be ignored unless reason
 *     is DataRequestReason:HANDOVER.
 * @param dnses If the reason is DataRequestReason:HANDOVER, this indicates the list of DNS
 *     addresses of the existing data connection. The format is defined in RFC-4291 section 2.2.
 *     For example, "192.0.1.3" or "2001:db8::1". This parameter must be ignored unless reason
 *     is DataRequestReason:HANDOVER.
 *
 * Response function is IRadioResponse.setupDataCallResponse_1_6()
 *
 * Note this API is the same as the 1.5
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   OP_NOT_ALLOWED_BEFORE_REG_TO_NW
 *   SIM_ABSENT
 */
#define RIL_REQUEST_SETUP_DATA_CALL 27

/**
 * Returns the data call list. An entry is added when a setupDataCall() is issued and removed
 * on a deactivateDataCall(). The list is emptied when setRadioPower()  off/on issued or when
 * the vendor HAL or modem crashes.
 *
 * @param serial Serial number of request.
 *
 * Response function is IRadioResponse.getDataCallListResponse_1_6()
 */
#define RIL_REQUEST_DATA_CALL_LIST 57

/**
 * Send a CDMA SMS message
 *
 * @param serial Serial number of request.
 * @param sms Cdma Sms to be sent described by CdmaSmsMessage in types.hal
 *
 * Response callback is IRadioResponse.sendCdmaSmsResponse_1_6()
 *
 * Note this API is the same as the 1.0
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   INVALID_ARGUMENTS
 *   INVALID_STATE
 *   SIM_ABSENT
 */
#define RIL_REQUEST_CDMA_SEND_SMS 87

/**
 * Set SIM card power state.
 * Request is used to power off or power on the card. It should not generate
 * a CardState.CARDSTATE_ABSENT indication, since the SIM is still physically
 * inserted.
 *
 * @param serial Serial number of request
 * @param powerUp POWER_DOWN if powering down the SIM card,
 *                POWER_UP if powering up the SIM card,
 *                POWER_UP_PASS_THROUGH if powering up the SIM card in
 *                                      pass through mode.
 *
 * When SIM card is in POWER_UP_PASS_THROUGH, the modem does not send
 * any command to it (for example SELECT of MF, or TERMINAL
 * CAPABILITY), and the SIM card is controlled completely by Telephony
 * sending APDUs directly. The SIM card state must be
 * RIL_CARDSTATE_PRESENT and the number of card apps will be 0.
 * No new error code is generated. Emergency calls are supported in
 * the same way as if the SIM card is absent.
 * Pass-through mode is valid only for the specific card session where
 * it is activated, and normal behavior occurs at the next SIM
 * initialization, unless POWER_UP_PASS_THROUGH is requested again.
 *
 * The device is required to power down the SIM card before it can
 * switch the mode between POWER_UP and POWER_UP_PASS_THROUGH.
 * At device power up, the SIM interface is powered up automatically.
 * Each subsequent request to this method is processed only after the
 * completion of the previous one.
 *
 * Response callback is IRadioResponse.setSimCardPowerResponse_1_6().
 * Note that this differs from setSimCardPower_1_1 in that the response
 * callback should only be sent once the device has finished executing
 * the request (the SIM has finished powering on or off).
 */
#define RIL_REQUEST_SET_SIM_CARD_POWER 140

/**
 * Send an SMS message. Identical to sendCdmaSms_1_6,
 * except that more messages are expected to be sent soon.
 *
 * @param serial Serial number of request.
 * @param sms Cdma Sms to be sent described by CdmaSmsMessage in types.hal
 *
 * Response callback is IRadioResponse.sendCdmaSMSExpectMoreResponse_1_6()
 *
 * Note this API is the same as the 1.5
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   INVALID_ARGUMENTS
 *   INVALID_STATE
 *   SIM_ABSENT
 */
#define RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE 148

///////////////////////////////////////////////////////////////////////////
// new RIL requests
///////////////////////////////////////////////////////////////////////////
/**
 * Enable or disable E-UTRA-NR dual connectivity. If disabled then UE will not connect
 * to secondary carrier.
 *
 * @param serial Serial number of request.
 * @param nrDualConnectivityState expected NR dual connectivity state.
 * 1. Enable NR dual connectivity {NrDualConnectivityState:ENABLE}
 * 2. Disable NR dual connectivity {NrDualConnectivityState:DISABLE}
 * 3. Disable NR dual connectivity and force secondary cell to be released
 * {NrDualConnectivityState:DISABLE_IMMEDIATE}

 * Response callback is IRadioResponse.enableNRDualConnectivityResponse()
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   INTERNAL_ERR
 */
#define RIL_REQUEST_ENABLE_NR_DUAL_CONNECTIVITY 213

/**
 * Is E-UTRA-NR Dual Connectivity enabled
 *
 * @param serial Serial number of request.
 * Response callback is IRadioResponse.isNRDualConnectivityEnabledResponse()
 *
 * Valid errors returned:
 *   SUCCESS
 *   RADIO_NOT_AVAILABLE
 *   INTERNAL_ERR
 */
#define RIL_REQUEST_IS_NR_DUAL_CONNECTIVITY_ENABLED 214

/**
 * Reserves an unallocated pdu session id from the pool of ids.
 *
 * The allocated id is returned in the response.
 *
 * When the id is no longer needed, call releasePduSessionId to
 * return it to the pool.
 *
 * Reference: 3GPP TS 24.007 section 11.2.3.1b
 *
 * @param serial Serial number of request.
 *
 * Response function is IRadioResponse.allocatePduSessionIdResponse()
 */
#define RIL_REQUEST_ALLOCATE_PDU_SESSION_ID 215

/**
 * Releases a pdu session id that was previously allocated using
 * allocatePduSessionId.
 *
 * Reference: 3GPP TS 24.007 section 11.2.3.1b
 * @param serial Serial number of request.
 * @param id Pdu session id to release.
 *
 * Response function is IRadioResponse.releasePduSessionIdResponse()
 */
#define RIL_REQUEST_RELEASE_PDU_SESSION_ID  216

/**
 * Indicates that a handover to the IWLAN transport has begun.
 *
 * Any resources being transferred to the IWlan transport cannot be released while a
 * handover is underway. For example, if a pdu session id needs to be
 * transferred to IWlan, then, the modem should not release the id while
 * the handover is in progress.
 *
 * If a handover was unsuccessful, then the framework calls IRadio::cancelHandover.
 * The modem retains ownership over any of the resources being transferred to IWlan.
 *
 * If a handover was successful, the framework calls IRadio::deactivateDataCall with reason
 * HANDOVER. The IWlan transport now owns the transferred resources and is responsible for
 * releasing them.
 *
 * @param serial Serial number of request.
 * @param id callId The identifier of the data call which is provided in SetupDataCallResult
 *
 * Response function is IRadioResponse.beginHandoverResponse()
 */
#define RIL_REQUEST_START_HANDOVER  217

/**
 * Indicates that a handover was cancelled after a call to IRadio::beginHandover.
 *
 * Since the handover was unsuccessful, the modem retains ownership over any of the resources
 * being transferred and is still responsible for releasing them.
 *
 * @param serial Serial number of request.
 * @param id callId The identifier of the data call which is provided in SetupDataCallResult
 *
 * Response function is IRadioResponse.cancelHandoverResponse()
 */
#define RIL_REQUEST_CANCEL_HANDOVER 218

/**
 * Get which bands the modem's background scan is acting on.
 *
 * @param serial Serial number of request.
 *
 * Response callback is IRadioResponse.getSystemSelectionChannelsResponse()
 */
#define RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS 219

/**
 * Control data throttling at modem.
 *   - DataThrottlingAction:NO_DATA_THROTTLING should clear any existing
 *     data throttling within the requested completion window.
 *   - DataThrottlingAction:THROTTLE_SECONDARY_CARRIER: Remove any existing
 *     throttling on anchor carrier and achieve maximum data throttling on
 *     secondary carrier within the requested completion window.
 *   - DataThrottlingAction:THROTTLE_ANCHOR_CARRIER: disable secondary
 *     carrier and achieve maximum data throttling on anchor carrier by
 *     requested completion window.
 *   - DataThrottlingAction:HOLD: Immediately hold on to current level of
 *     throttling.
 *
 * @param serial Serial number of request.
 * @param dataThrottlingAction DataThrottlingAction as defined in types.hal
 * @param completionWindowSecs window, in seconds, in which the requested
 *     throttling action has to be achieved. This must be 0 when
 *     dataThrottlingAction is DataThrottlingAction:HOLD.
 *
 * Response function is IRadioResponse.setDataThrottlingResponse()
 */
#define RIL_REQUEST_SET_DATA_THROTTLING 221

/**
 * Requests to set the network type for searching and registering.
 *
 * Instruct the radio to *only* accept the types of network provided. This
 * is stronger than setPreferredNetworkType which is a suggestion.
 *
 * @param serial Serial number of request.
 * @param networkTypeBitmap a 32-bit bearer bitmap of RadioAccessFamily
 *
 * Response callbask is IRadioResponse.setNetworkTypeBitmapResponse()
 */
#define RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP 222

/**
 * Requests bitmap representing the currently allowed network types.
 *
 * Requests the bitmap set by the corresponding method
 * setAllowedNetworkTypesBitmap, which sets a strict set of RATs for the
 * radio to use. Differs from getPreferredNetworkType and getPreferredNetworkTypeBitmap
 * in that those request *preferences*.
 *
 * @param serial Serial number of request.
 *
 * Response callback is IRadioResponse.getNetworkTypeBitmapResponse()
 */
#define RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP 223

/**
 * Get the local and global phonebook records from the SIM card.
 * This should be called again after a simPhonebookChanged notification is received.
 *
 * The phonebook records are received via IRadioIndication.simPhonebookRecordsReceived()
 *
 * @param serial Serial number of request.
 *
 * Response callback is IRadioResponse.getSimPhonebookRecordsResponse()
 */
#define RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS 170

/**
 * Get the phone book capacity
 *
 * @param serial Serial number of request.
 *
 * Response function is defined from IRadioResponse.getSimPhonebookCapacityResponse()
 */
#define RIL_REQUEST_GET_SIM_PHONEBOOK_CAPACITY 171

/**
 * Insert, delete or update a phonebook record on the SIM card.
 * If the index of recordInfo is 0, the phonebook record will be added to global or
 * local phonebook, and global phonebook has higher priority than local phonebook.
 *
 * If the fields in the recordInfo are all empty except for the index, the phonebook
 * record specified by the index will be deleted.
 *
 * The indication simPhonebookChanged will be called after every successful call of
 * updateSimPhonebookRecords.
 *
 * @param serial Serial number of request.
 * @param recordInfo Details of the record to insert, delete or update.
 *
 * Response callback is IRadioResponse.updateSimPhonebookRecordsResponse()
 */
#define RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS 172

/**
 * Request to get the current slicing configuration including URSP rules and
 * NSSAIs (configured, allowed and rejected).
 * URSP stands for UE route selection policy and is defined in 3GPP TS 24.526
 * Section 4.2.
 * An NSSAI is a collection of network slices. Each network slice is identified by
 * an S-NSSAI and is represented by the struct SliceInfo. NSSAI and S-NSSAI
 * are defined in 3GPP TS 24.501.
 *
 * @param serial Serial number of request.
 *
 * Response function is IRadioResponse.getSlicingConfigResponse()
 */

#define RIL_REQUEST_SLICING_CONFIG 224



/***********************************************************************/

/**
 * Indicates data call contexts have changed.
 *
 * This indication is updated from IRadioIndication@1.5 to report the @1.6 version of
 * SetupDataCallResult.
 *
 * @param type Type of radio indication
 * @param dcList Array of SetupDataCallResult identical to that returned by
 *        IRadio.getDataCallList(). It is the complete list of current data contexts including
 *        new contexts that have been activated. A data call is only removed from this list
 *        when any of the below conditions is matched.
 *        1. The framework sends a IRadio.deactivateDataCall().
 *        2. The radio is powered off/on.
 *        3. Unsolicited disconnect from either modem or network side.
 */
#define RIL_UNSOL_DATA_CALL_LIST_CHANGED 1010

/**
 * RIL_UNSOL_UNTHROTTLE_APN
 *
 * The modem can explicitly set SetupDataCallResult::suggestedRetryTime after a failure in
 * IRadio@1.6::SetupDataCall. During that time, no new calls are allowed to
 * IRadio@1.6::SetupDataCall that use the same APN.
 *
 * When IRadioIndication@1.6::unthrottleApn is sent, AOSP will no longer throttle calls
 * to IRadio@1.6::SetupDataCall for the given APN.
 *
 * @param type Type of radio indication
 * @param apn Apn to unthrottle
 */
#define RIL_UNSOL_UNTHROTTLE_APN 1052

/**
 * RIL_UNSOL_RESPONSE_PHONEBOOK_INIT_DONE
 *
 * "data" is NULL
 * "response" is a NULL *
 */
#define RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED 1053

/**
 * RIL_UNSOL_PHONEBOOK_RECORDS_RECEIVED
 *
 * "data" is NULL
 * "response" is a const PhoneBookRecordInfo *
 */
#define RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_RECORDS_RECEIVED 1054

#endif // __RADIO_V1_6_H__
