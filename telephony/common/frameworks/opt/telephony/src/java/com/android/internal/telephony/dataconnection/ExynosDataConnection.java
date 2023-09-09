/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony.vendor.dataconnection;

import static android.telephony.data.DataCallResponse.PDU_SESSION_ID_NOT_SET;

import android.telephony.AccessNetworkConstants;
import android.telephony.data.DataService;
import android.text.TextUtils;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.dataconnection.ApnContext;
import com.android.internal.telephony.dataconnection.DataConnection;
import com.android.internal.telephony.dataconnection.DataServiceManager;
import com.android.internal.telephony.dataconnection.DcController;
import com.android.internal.telephony.dataconnection.DcTesterFailBringUpAll;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.internal.util.IndentingPrintWriter;
import com.android.internal.util.Protocol;
import com.android.telephony.Rlog;

import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * {@hide}
 *
 * ExynosDataConnection DataConnection
 *
 * This a class for representing a single data connection, with instances of this
 * class representing a connection via the cellular network. There may be multiple
 * data connections and all of them are managed by the <code>DataConnectionTracker</code>.
 *
 * NOTE: All DataConnection objects must be running on the same looper, which is the default
 * as the coordinator has members which are used without synchronization.
 */
public class ExynosDataConnection extends DataConnection {
    private static final boolean DBG = true;
    private static final boolean VDBG = true;

    private static AtomicInteger mInstanceNumber = new AtomicInteger(0);

    private DataServiceManager mDataServiceManager;

    // ***** Event codes for driving the state machine, package visible for Dcc
    static final int BASE = Protocol.BASE_DATA_CONNECTION;
    static final int EVENT_DEACTIVATE_DONE = BASE + 3;

    protected ExynosDataConnection(Phone phone, String tagSuffix, int id,
                           DcTracker dct, DataServiceManager dataServiceManager,
                           DcTesterFailBringUpAll failBringUpAll, DcController dcc,
                           boolean doAllocatePduSessionId) {
        super(phone, "Ex-" + tagSuffix, id, dct, dataServiceManager, failBringUpAll, dcc);
        mDataServiceManager = dataServiceManager;
    }

    /**
     * Create the connection object
     *
     * @param phone the Phone
     * @param id the connection id
     * @return DataConnection that was created.
     */
    public static DataConnection makeDataConnection(Phone phone, int id, DcTracker dct,
                                                    DataServiceManager dataServiceManager,
                                                    DcTesterFailBringUpAll failBringUpAll,
                                                    DcController dcc,
                                                    boolean doAllocatePduSessionId) {
        String transportType = (dataServiceManager.getTransportType()
                == AccessNetworkConstants.TRANSPORT_TYPE_WWAN)
                ? "C"   // Cellular
                : "I";  // IWLAN
        ExynosDataConnection dc = new ExynosDataConnection(phone, transportType + "-"
                + mInstanceNumber.incrementAndGet(), id, dct, dataServiceManager, failBringUpAll,
                dcc, doAllocatePduSessionId);
        dc.start();
        if (DBG) dc.log("Made " + dc.getName());
        return dc;
    }

    /**
     * TearDown the data connection when the deactivation is complete a Message with
     * msg.what == EVENT_DEACTIVATE_DONE
     *
     * @param o is the object returned in the AsyncResult.obj.
     */
    @Override
    protected void tearDownData(Object o) {
        int discReason = DataService.REQUEST_REASON_NORMAL;
        ApnContext apnContext = null;
        if ((o != null) && (o instanceof DisconnectParams)) {
            DisconnectParams dp = (DisconnectParams) o;
            apnContext = dp.mApnContext;
            if (TextUtils.equals(dp.mReason, Phone.REASON_RADIO_TURNED_OFF)) {
                discReason = DataService.REQUEST_REASON_SHUTDOWN;
            } else if (dp.mReleaseType == DcTracker.RELEASE_TYPE_HANDOVER) {
                discReason = DataService.REQUEST_REASON_HANDOVER;
            } else if (TextUtils.equals(dp.mReason, Phone.REASON_PDP_RESET)) {
                // The addtional reason of the data request for adjusting
                // baseband's request REQUEST_REASON_PDP_RESET = 4;
                discReason = 4;
            }
        }

        String str = "tearDownData. mCid=" + mCid + ", reason=" + discReason;
        if (DBG) log(str);
        if (apnContext != null) ApnContext.requestLog(apnContext, str);


        //Needed to be final to work in a closure
        final int fDiscReason = discReason;
        releasePduSessionId(() -> {
            // This is run after release pdu session id is complete
            this.setPduSessionId(PDU_SESSION_ID_NOT_SET);
            mDataServiceManager.deactivateDataCall(mCid, fDiscReason,
                    obtainMessage(EVENT_DEACTIVATE_DONE, mTag, 0, o));
            getDataCallSessionStats().setDeactivateDataCallReason(fDiscReason);
        });
    }

    /**
     * Log with debug
     *
     * @param s is string log
     */
    @Override
    protected void log(String s) {
        Rlog.d(getName(), s);
    }

    @Override
    public void dump(FileDescriptor fd, PrintWriter printWriter, String[] args) {
        IndentingPrintWriter pw = new IndentingPrintWriter(printWriter, " ");
        super.dump(fd, printWriter, args);
        pw.flush();
        pw.print("ExynosDataConnection ");
        pw.increaseIndent();
        pw.println("mInstanceNumber=" + mInstanceNumber);
        pw.decreaseIndent();
        pw.println();
        pw.flush();
    }
}

