/*
 * Copyright (C) 2015 The Android Open Source Project
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

package com.android.internal.telephony;

import android.compat.annotation.UnsupportedAppUsage;
import android.content.ContentValues;
import android.content.pm.PackageManager;
import android.os.AsyncResult;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.TextUtils;

import com.android.internal.telephony.uicc.AdnCapacity;
import com.android.internal.telephony.uicc.AdnRecord;
import com.android.internal.telephony.uicc.AdnRecordCache;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.uicc.IccConstants;
import com.android.internal.telephony.uicc.IccFileHandler;
import com.android.internal.telephony.uicc.IccRecords;
import com.android.internal.telephony.uicc.IccUtils;
import com.android.telephony.Rlog;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * {@hide}
 */
public class ExynosIccPhoneBookInterfaceManager extends IccPhoneBookInterfaceManager {

    public static final String LOG_TAG = "ExynosIccPhoneBookIM";
    private static final boolean DBG = true;
    private static final boolean VDBG = false; /* STOPSHIP if true */
    private static final boolean LOAD_SDN_RECORDS_WHEN_LOADING_ADN_RECORDS = true;
    private static int mPhoneId = -1;

    public ExynosIccPhoneBookInterfaceManager(Phone phone) {
        super(phone);
    }

    @Override
    protected void logd(String msg) {
        Rlog.d(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }

    @Override
    protected void loge(String msg) {
        Rlog.e(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }

    @Override
    public boolean
    updateExtentedAdnRecordsInEfByIndex(int efid, ContentValues values, int index, String pin2) {
        if (mPhone.getContext().checkCallingOrSelfPermission(
                android.Manifest.permission.WRITE_CONTACTS)
                != PackageManager.PERMISSION_GRANTED) {
            throw new SecurityException(
                    "Requires android.permission.WRITE_CONTACTS permission");
        }

        efid = updateEfForIccType(efid);
        if (DBG) {
            logd("updateExtentedAdnRecordsInEfByIndex: efid=" + efid + ", values = " +
                values + " index=" + index + ", pin2=" + pin2);
        }

        checkThread();
        Request updateRequest = new Request();
        synchronized (updateRequest) {
            Message response = mBaseHandler.obtainMessage(EVENT_UPDATE_DONE, updateRequest);
            if (usesPbCache(efid)) {
                AdnRecord newAdn =
                        generateAdnRecordWithNewTagByContentValues(IccConstants.EF_ADN,
                        index, values);
                mSimPbRecordCache.updateSimPbAdnByRecordId(index, newAdn, response);
                waitForResult(updateRequest);
                return (boolean) updateRequest.mResult;
            } else {
                AdnRecord newAdn = generateAdnRecordWithNewTagByContentValues(efid, index, values);
                if (mAdnCache != null) {
                    mAdnCache.updateUsimAdnByIndex(efid, newAdn, index, pin2, response);
                    waitForResult(updateRequest);
                    return (boolean) updateRequest.mResult;
                } else {
                    loge("Failure while trying to update by index due to uninitialised adncache");
                    return false;
                }
            }
        }
    }

    @Override
    public List<AdnRecord> getPbRecordsInEf(int efid) {
        if (mPhone != null) {
            mPhoneId = mPhone.getPhoneId();
            if (mAdnCache != null) {
                mAdnCache.setPhoneIdForSimPhoneBook(mPhoneId);
            }
        }

        // copy loadRequest.request to RequestResult
        List<AdnRecord> RequestResult = new ArrayList<AdnRecord>();
        List<AdnRecord> adnList = super.getAdnRecordsInEf(efid);
        if (adnList != null) {
            logd("getPbRecordsInEf - adnList size:" + adnList.size());
            for (int i = 0; i < adnList.size(); i++) {
                RequestResult.add(new AdnRecord(adnList.get(i)));
            }
        }

        if (LOAD_SDN_RECORDS_WHEN_LOADING_ADN_RECORDS) {
            // Try to load SDN records after loading ADN is done.
            List<AdnRecord> sdnList = super.getAdnRecordsInEf(IccConstants.EF_SDN);
            if (sdnList != null) {
                logd("getPbRecordsInEf - sdnList size:" + sdnList.size());
                RequestResult.addAll(sdnList);
            }
        }

        logd("getPbRecordsInEf - return pbr records size:" + RequestResult.size());
        return RequestResult;
    }

    @Override
    public int getAdnSize() {
        int adnCount = 0;
        if (mSimPbRecordCache.isEnabled()) {
            AdnCapacity capa = getAdnRecordsCapacity();
            adnCount = capa.getMaxAdnCount();
        } else if (mAdnCache != null) {
            if (mPhone.getCurrentUiccAppType() == AppType.APPTYPE_USIM
                    || mPhone.getCurrentUiccAppType() == AppType.APPTYPE_CSIM
                    || mPhone.getCurrentUiccAppType() == AppType.APPTYPE_RUIM) {
                adnCount = mAdnCache.getUsimAdnSize();
            } else {
                adnCount = mAdnCache.getAdnSize();
            }
        } else {
            loge("mAdnCache is NULL when getAdnSize.");
        }
        logd("getAdnSize:" + adnCount);
        return adnCount;
    }

    @Override
    public int getEmailSize() {
        int emailCount = 0;
        if (mSimPbRecordCache.isEnabled()) {
            AdnCapacity capa = getAdnRecordsCapacity();
            emailCount = capa.getMaxEmailCount();
        } else if (mAdnCache != null) {
            emailCount = mAdnCache.getEmailSize();
        } else {
            loge("mAdnCache is NULL when getEmailSize.");
        }
        logd("getEmailSize:" + emailCount);
        return emailCount;
    }

    @Override
    public int getAnrSize() {
        int anrCount = 0;
        if (mSimPbRecordCache.isEnabled()) {
            AdnCapacity capa = getAdnRecordsCapacity();
            anrCount = capa.getMaxAnrCount();
        } else if (mAdnCache != null) {
            anrCount = mAdnCache.getAnrSize();
        } else {
            loge("mAdnCache is NULL when getAnrSize.");
        }
        logd("getAnrSize:" + anrCount);
        return anrCount;
    }

    @Override
    public int getSpareEmailCount() {
        int spareEmailCount = 0;
        if (mSimPbRecordCache.isEnabled()) {
            AdnCapacity capa = getAdnRecordsCapacity();
            spareEmailCount = capa.getMaxEmailCount() - capa.getUsedEmailCount();
        } else if (mAdnCache != null) {
            spareEmailCount = mAdnCache.getSpareEmailCount();
        } else {
            loge("mAdnCache is NULL when getSpareEmailCount.");
        }
        logd("getSpareEmailCount:" + spareEmailCount);
        return spareEmailCount;
    }

    @Override
    public int getSpareAnrCount() {
        int spareAnrCount = 0;
        if (mSimPbRecordCache.isEnabled()) {
            AdnCapacity capa = getAdnRecordsCapacity();
            spareAnrCount = capa.getMaxAnrCount() - capa.getUsedAnrCount();
        } else if (mAdnCache != null) {
            spareAnrCount = mAdnCache.getSpareAnrCount();
        } else {
            loge("mAdnCache is NULL when getSpareAnrCount.");
        }
        logd("getSpareAnrCount:" + spareAnrCount);
        return spareAnrCount;
    }

    @Override
    public int getSupportedAnrSetCount() {
        int maxAnrSetCount = 0;
        if (mSimPbRecordCache.isEnabled()) {
            AdnCapacity capa = getAdnRecordsCapacity();
            maxAnrSetCount = 1;
        } else if (mAdnCache != null) {
            maxAnrSetCount = mAdnCache.getSupportedAnrSetCount();
        } else {
            loge("mAdnCache is NULL when getSupportedAnrSetCount.");
        }
        logd("getSupportedAnrSetCount:" + maxAnrSetCount);
        return maxAnrSetCount;
    }
}

