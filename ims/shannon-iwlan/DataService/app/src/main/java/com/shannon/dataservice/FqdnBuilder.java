/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import android.telephony.CellIdentityGsm;
import android.telephony.CellIdentityLte;
import android.telephony.CellIdentityNr;
import android.telephony.CellIdentityTdscdma;
import android.telephony.CellIdentityWcdma;
import android.telephony.CellInfo;
import android.telephony.CellInfoGsm;
import android.telephony.CellInfoLte;
import android.telephony.CellInfoNr;
import android.telephony.CellInfoTdscdma;
import android.telephony.CellInfoWcdma;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.text.TextUtils;

import java.util.LinkedList;
import java.util.List;
import java.util.Locale;

import static com.shannon.dataservice.ConfigurationParameters.MCCMNC_EHPLMN;
import static com.shannon.dataservice.ConfigurationParameters.MCCMNC_OPLMNACT;
import static com.shannon.dataservice.ConfigurationParameters.MCCMNC_PLMN;
import static com.shannon.dataservice.ConfigurationParameters.MCCMNC_SIM;
import static com.shannon.dataservice.ConfigurationParameters.MCCMNC_WPLMN;
import static com.shannon.dataservice.ConfigurationParameters.SERVER_3GPP_5G_LOCATION;
import static com.shannon.dataservice.ConfigurationParameters.SERVER_3GPP_LOCATION;
import static com.shannon.dataservice.ConfigurationParameters.SERVER_3GPP_OPERATOR;
import static com.shannon.dataservice.ConfigurationParameters.SERVER_3GPP_VISITED;
import static com.shannon.dataservice.ConfigurationParameters.SERVER_IMS_OPERATOR;
import static com.shannon.dataservice.ConfigurationParameters.SERVER_SIM;

public class FqdnBuilder {
    private final ConfigurationParameters mParameters;
    private final TelephonyManager mTelephonyManager;
    private final DnsResolver mDnsResolver;
    private final SimRecords mSimRecords;

    public FqdnBuilder(ConfigurationParameters parameters, TelephonyManager tm, DnsResolver resolver, SimRecords records) {
        mParameters = parameters;
        mTelephonyManager = tm;
        mDnsResolver = resolver;
        mSimRecords = records;
    }

    public LinkedList<String> build(int type, boolean isRoaming) {
        if ((type & ApnSetting.TYPE_EMERGENCY) != 0) {
            return buildFqdnForEmergency(isRoaming);
        }
        return buildFqdn(isRoaming);
    }

    private LinkedList<String> buildFqdn(boolean isRoaming) {
        LinkedList<String> list;
        LinkedList<String> response = new LinkedList<>();

        /* roaming first */
        if (isRoaming) {
            list = processFqdnRecord(mParameters.mRoamingFqdnIms, null, false);
            if (list != null && !list.isEmpty()) {
                response.addAll(list);
            }
        }
        list = processFqdnRecord(mParameters.mHomeFqdnIms, null, false);
        if (list != null && !list.isEmpty()) {
            response.addAll(list);
        }
        return response;
    }

    private LinkedList<String> buildFqdnForEmergency(boolean isRoaming) {
        LinkedList<String> list;
        LinkedList<String> response = new LinkedList<>();

        /* roaming first */
        if (isRoaming) {
            list = processFqdnRecord(mParameters.mRoamingFqdnEmc, "sos", true);
            if (list != null && !list.isEmpty()) {
                response.addAll(list);
            }
        }
        list = processFqdnRecord(mParameters.mHomeFqdnEmc, "sos", true);
        if (list != null && !list.isEmpty()) {
            response.addAll(list);
        }

        /*
         *   no specific requirements for emergency
         *   we can use IMS FQDN configuration
         */
        if (response.isEmpty()) {
            if (isRoaming) {
                list = processFqdnRecord(mParameters.mRoamingFqdnIms, null, true);
                if (list != null && !list.isEmpty()) {
                    response.addAll(list);
                }
            }
            list = processFqdnRecord(mParameters.mHomeFqdnIms, null, true);
            if (list != null && !list.isEmpty()) {
                response.addAll(list);
            }
        }
        return response;
    }

    private MccMnc getMccMnc(String source) {
        if ((source == null) || source.isEmpty()) {
            return getMccMnc(MCCMNC_SIM);
        }
        if (MCCMNC_SIM.equalsIgnoreCase(source)) {
            return new MccMnc(mTelephonyManager.getSimOperator());
        } else if (MCCMNC_EHPLMN.equalsIgnoreCase(source)) {
            String[] efEHPLMN = mSimRecords.getEfEhplmn();
            if (efEHPLMN != null && efEHPLMN.length > 0) {
                return new MccMnc(efEHPLMN[0]);
            }
        } else if (MCCMNC_WPLMN.equalsIgnoreCase(source)) {
            String plmn = mSimRecords.getIwlanPlmn();
            if (plmn != null && !plmn.isEmpty()) {
                return new MccMnc(plmn);
            }
        } else if (MCCMNC_OPLMNACT.equalsIgnoreCase(source)) {
            String operator = mTelephonyManager.getNetworkOperator();
            if (operator != null && !operator.isEmpty()) {
                String[] efOpmln = mSimRecords.getEfOplmn();
                if (efOpmln != null && efOpmln.length > 0) {
                    for (String plmn : efOpmln) {
                        if (operator.equalsIgnoreCase(plmn)) {
                            return new MccMnc(operator);
                        }
                    }
                }
            }
            /* special case here, try EHPLMN first  */
            return getMccMnc(MCCMNC_EHPLMN);
        } else if (MCCMNC_PLMN.equalsIgnoreCase(source)) {
            String operator = mTelephonyManager.getNetworkOperator();
            if (operator != null && !operator.isEmpty()) {
                return new MccMnc(operator);
            }
        } else {
            /* for selection EF, PLMN could be provided from SIM */
            if (TextUtils.isDigitsOnly(source)) {
                return new MccMnc(source);
            }
        }
        return getMccMnc(MCCMNC_SIM);
    }

    private int getNetworkType() {
        try {
            if (mTelephonyManager.getDataState() == TelephonyManager.DATA_CONNECTED) {
                return mTelephonyManager.getDataNetworkType();
            }
            return mTelephonyManager.getVoiceNetworkType();
        } catch (Exception e) {
            return TelephonyManager.NETWORK_TYPE_UNKNOWN;
        }
    }

    private String buildLocationFqdnWithLac(int lac, String prefix) {
        /* lac<LAC>.epdg.epc.mnc<MNC>.mcc<MCC>.pub.3gppnetwork.org */
        String operator = mTelephonyManager.getNetworkOperator();
        if (operator != null && !operator.isEmpty()) {
            /* PLMN will be always used */
            MccMnc mccMnc = new MccMnc(operator);

            String res = String.format(Locale.US, "lac%04x.", lac);
            if (prefix != null) {
                res += prefix + ".";
            }
            res += String.format(Locale.US, "epdg.epc.mnc%03d.mcc%03d.pub.3gppnetwork.org",
                    mccMnc.getMnc(), mccMnc.getMcc());
            return res;
        }
        return null;
    }

    private String buildLocationFqdnWithTac(int tac, String prefix) {
        /* tac-lb<TAC-low-byte>.tac-hb<TAC-high-byte>.tac.epdg.epc.mnc<MNC>.mcc<MCC>.pub.3gppnetwork.org */
        String operator = mTelephonyManager.getNetworkOperator();
        if (operator != null && !operator.isEmpty()) {
            /* PLMN will be always used */
            MccMnc mccMnc = new MccMnc(operator);

            String res = String.format(Locale.US, "tac-lb%02x.tac-hb%02x.", tac & 0xFF, (tac & 0xFF00) >> 8);
            if (prefix != null) {
                res += prefix + ".";
            }
            res += String.format(Locale.US, "epdg.epc.mnc%03d.mcc%03d.pub.3gppnetwork.org",
                    mccMnc.getMnc(), mccMnc.getMcc());
            return res;
        }
        return null;
    }

    private String buildLocationFqdnWith5gsTac(int tac, String prefix) {
        /* tac-lb<TAC-low-byte>.tac-mb<TAC-middle-byte>.tac-hb<TAC-high-byte>.5gstac.epdg.epc.mnc<MNC>.mcc<MCC>.pub.3gppnetwork.org */
        String operator = mTelephonyManager.getNetworkOperator();
        if (operator != null && !operator.isEmpty()) {
            /* PLMN will be always used */
            MccMnc mccMnc = new MccMnc(operator);

            String res = String.format(Locale.US, "tac-lb%02x.tac-mb%02x.tac-hb%02x.5gstac.",
                    tac & 0xFF, (tac & 0xFF00) >> 8, (tac & 0xFF0000) >> 16);
            if (prefix != null) {
                res += prefix + ".";
            }
            res += String.format(Locale.US, "epdg.epc.mnc%03d.mcc%03d.pub.3gppnetwork.org",
                    mccMnc.getMnc(), mccMnc.getMcc());
            return res;
        }
        return null;
    }

    private LinkedList<String> processFqdnRecord(String records, String prefix, boolean isEmergency) {
        if ((records == null) || records.isEmpty()) {
            return null;
        }
        LinkedList<String> result = new LinkedList<>();

        /* record could contain multiple FQDN formats: 3gpp-visited, sim, 3gpp-operator:plmn */
        String[] recordsArray = records.split(",");
        for (String record : recordsArray) {
            String[] formatArray = record.trim().split(":");

            String format = formatArray[0].trim();
            String source = null;
            if (formatArray.length > 1) {
                source = formatArray[1].trim();
            }
            if (SERVER_3GPP_OPERATOR.equalsIgnoreCase(format)) {
                /* epdg.epc.mnc<MNC>.mcc<MCC>.pub.3gppnetwork.org */
                try {
                    MccMnc mccMnc = getMccMnc(source);

                    String s = "";
                    if (prefix != null) {
                        s += prefix + ".";
                    }
                    s += String.format(Locale.US, "epdg.epc.mnc%03d.mcc%03d.pub.3gppnetwork.org",
                            mccMnc.getMnc(), mccMnc.getMcc());
                    result.add(s);
                } catch (Exception e) {
                    // nothing
                }
            } else if (SERVER_3GPP_LOCATION.equalsIgnoreCase(format)) {
                int networkType = getNetworkType();
                if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                    try {
                        List<CellInfo> cellInfoList = mTelephonyManager.getAllCellInfo();
                        for (CellInfo cellInfo : cellInfoList) {
                            if (cellInfo.isRegistered()) {
                                String fqdn = null;
                                if (cellInfo instanceof CellInfoGsm) {
                                    CellIdentityGsm cellId = ((CellInfoGsm)cellInfo).getCellIdentity();
                                    fqdn = buildLocationFqdnWithLac(cellId.getLac(), prefix);
                                } else if (cellInfo instanceof CellInfoWcdma) {
                                    CellIdentityWcdma cellId = ((CellInfoWcdma)cellInfo).getCellIdentity();
                                    fqdn = buildLocationFqdnWithLac(cellId.getLac(), prefix);
                                } else if (cellInfo instanceof CellInfoLte) {
                                    CellIdentityLte cellId = ((CellInfoLte)cellInfo).getCellIdentity();
                                    fqdn = buildLocationFqdnWithTac(cellId.getTac(), prefix);
                                } else if (cellInfo instanceof CellInfoNr) {
                                    CellIdentityNr cellId = (CellIdentityNr)(((CellInfoNr)cellInfo).getCellIdentity());
                                    fqdn = buildLocationFqdnWithTac(cellId.getTac(), prefix);
                                } else if (cellInfo instanceof CellInfoTdscdma) {
                                    CellIdentityTdscdma cellId = ((CellInfoTdscdma)cellInfo).getCellIdentity();
                                    fqdn = buildLocationFqdnWithLac(cellId.getLac(), prefix);
                                }
                                if (fqdn != null) {
                                    result.add(fqdn);
                                }
                            }
                        }
                    } catch (Exception e) {
                        // nothing
                    }
                }
            } else if (SERVER_3GPP_5G_LOCATION.equalsIgnoreCase(format)) {
                int networkType = getNetworkType();
                if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                    try {
                        List<CellInfo> cellInfoList = mTelephonyManager.getAllCellInfo();
                        for (CellInfo cellInfo : cellInfoList) {
                            if (cellInfo.isRegistered()) {
                                String fqdn = null;
                                if (cellInfo instanceof CellInfoLte) {
                                    CellIdentityLte cellId = ((CellInfoLte)cellInfo).getCellIdentity();
                                    fqdn = buildLocationFqdnWith5gsTac(cellId.getTac(), prefix);
                                } else if (cellInfo instanceof CellInfoNr) {
                                    CellIdentityNr cellId = (CellIdentityNr)(((CellInfoNr)cellInfo).getCellIdentity());
                                    fqdn = buildLocationFqdnWith5gsTac(cellId.getTac(), prefix);
                                }
                                if (fqdn != null) {
                                    result.add(fqdn);
                                }
                            }
                        }
                    } catch (Exception e) {
                        // nothing
                    }
                }
            } else if (SERVER_3GPP_VISITED.equalsIgnoreCase(format)) {
                /* epdg.epc.mcc<MCC>.visited-country.pub.3gppnetwork.org */
                if (mTelephonyManager.isNetworkRoaming()) {
                    try {
                        String operator = mTelephonyManager.getNetworkOperator();
                        if (operator != null && !operator.isEmpty()) {
                            MccMnc mccMnc = new MccMnc(operator);

                            String s = "";
                            if (prefix != null) {
                                s += prefix + ".";
                            }
                            s += String.format(Locale.US,
                                    "epdg.epc.mcc%03d.visited-country.pub.3gppnetwork.org", mccMnc.getMcc());

                            LinkedList<String> fqdnList = mDnsResolver.doReplacementLookup(s);
                            if (fqdnList != null && !fqdnList.isEmpty()) {
                                result.addAll(fqdnList);
                            }
                        }
                    } catch (Exception e) {
                        // noting
                    }
                }
            } else if (SERVER_IMS_OPERATOR.equalsIgnoreCase(format)) {
                /* epdg.ims.mnc<MNC>.mcc<MCC>.pub.3gppnetwork.org */
                try {
                    MccMnc mccMnc = getMccMnc(source);
                    String s = String.format(Locale.US, "epdg.ims.mnc%03d.mcc%03d.pub.3gppnetwork.org",
                            mccMnc.getMnc(), mccMnc.getMcc());
                    result.add(s);
                } catch (Exception e) {
                    // nothing
                }
            } else if (SERVER_SIM.equalsIgnoreCase(format)) {
                try {
                    String[] efSim;
                    if (isEmergency) {
                        efSim = mSimRecords.getEfEpdgIdEmergency();
                        if (efSim != null && efSim.length > 0) {
                            for (String fqdn : efSim) {
                                if (fqdn != null && !fqdn.isEmpty()) {
                                    result.add(fqdn);
                                }
                            }
                        }
                        efSim = mSimRecords.getEfEpdgSelectionEmergency();
                        if (efSim != null && efSim.length > 0) {
                            for (String recordSim : efSim) {
                                LinkedList<String> fqdnList = processFqdnRecord(recordSim, prefix, true);
                                if (fqdnList != null && !fqdnList.isEmpty()) {
                                    result.addAll(fqdnList);
                                }
                            }
                        }
                    }
                    efSim = mSimRecords.getEfEpdgId();
                    if (efSim != null && efSim.length > 0) {
                        for (String fqdn : efSim) {
                            if (fqdn != null && !fqdn.isEmpty()) {
                                result.add(fqdn);
                            }
                        }
                    }
                    efSim = mSimRecords.getEfEpdgSelection();
                    if (efSim != null && efSim.length > 0) {
                        for (String recordSim : efSim) {
                            LinkedList<String> fqdnList = processFqdnRecord(recordSim, prefix, isEmergency);
                            if (fqdnList != null && !fqdnList.isEmpty()) {
                                result.addAll(fqdnList);
                            }
                        }
                    }
                } catch (Exception e) {
                    // nothing
                }
            } else {
                /* hardcoded value like epdg.epc.att.net */
                if (!format.isEmpty()) {
                    result.add(format);
                }
            }
        }
        return result;
    }
}
