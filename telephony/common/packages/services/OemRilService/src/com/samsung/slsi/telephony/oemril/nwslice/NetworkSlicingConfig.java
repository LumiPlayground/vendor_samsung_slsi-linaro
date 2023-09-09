/**
 * Copyright 2021 The Android Open Source Project
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

package com.samsung.slsi.telephony.oemril;

import android.annotation.NonNull;
import android.annotation.Nullable;
import android.os.Parcel;
import android.os.Parcelable;

import com.android.telephony.Rlog;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

//import android.telephony.data.TrafficDescriptor;
//import android.telephony.data.UrspRule;
// IpDesciprtor is adopted from package android.cmccslice
// import android.telephony.data.IpDescriptor;
import android.cmccslice.IpDescriptors;
import android.cmccslice.TrafficDescriptor;

import vendor.samsung_slsi.telephony.hardware.radio.V1_0.*;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.*;

import com.android.internal.telephony.RIL;
import com.android.internal.telephony.RILUtils;

/**
 * Represents a slicing configuration
 */
public final class NetworkSlicingConfig implements Parcelable {
    private final List<UrspRule> mUrspRules;
    private final List<NetworkSliceInfo> mSliceInfo;

    public NetworkSlicingConfig() {
        mUrspRules = new ArrayList<UrspRule>();
        mSliceInfo = new ArrayList<NetworkSliceInfo>();
    }

    /** @hide */
    public NetworkSlicingConfig(vendor.samsung_slsi.telephony.hardware.radio.V1_2.SlicingConfig sc) {
        this(sc.urspRules, sc.sliceInfo);
    }

    /** @hide */
    public NetworkSlicingConfig(List<vendor.samsung_slsi.telephony.hardware.radio.V1_2.UrspRule> urspRules,
            List<vendor.samsung_slsi.telephony.hardware.radio.V1_2.SliceInfo> sliceInfo) {
        mUrspRules = new ArrayList<UrspRule>();
        for (vendor.samsung_slsi.telephony.hardware.radio.V1_2.UrspRule ur : urspRules) {
            ArrayList<TrafficDescriptor> tds = new ArrayList<TrafficDescriptor>();
            /*
            for (vendor.samsung_slsi.telephony.hardware.radio.V1_2.TrafficDescriptor td : ur.trafficDescriptors) {
                mUrspRules.add(new UrspRule(ur.precedence, convertTrafficDescriptor(td)));
            }
            */
        }
        mSliceInfo = new ArrayList<NetworkSliceInfo>();
        for (vendor.samsung_slsi.telephony.hardware.radio.V1_2.SliceInfo si : sliceInfo) {
            mSliceInfo.add(sliceInfoBuilder(si));
        }
    }

    private NetworkSliceInfo sliceInfoBuilder(vendor.samsung_slsi.telephony.hardware.radio.V1_2.SliceInfo si) {
        NetworkSliceInfo.Builder builder = new NetworkSliceInfo.Builder()
                .setSliceServiceType(si.sst)
                .setMappedHplmnSliceServiceType(si.mappedHplmnSst);
        if (si.sliceDifferentiator != NetworkSliceInfo.SLICE_DIFFERENTIATOR_NO_SLICE) {
            builder
                .setSliceDifferentiator(si.sliceDifferentiator)
                .setMappedHplmnSliceDifferentiator(si.mappedHplmnSD);
        }
        return builder.build();
    }

    /** @hide */
    public NetworkSlicingConfig(Parcel p) {
        mUrspRules = p.createTypedArrayList(UrspRule.CREATOR);
        mSliceInfo = p.createTypedArrayList(NetworkSliceInfo.CREATOR);
    }

    /**
     * This list contains the current URSP rules. Empty list represents that no rules are
     * configured.
     * @return the current URSP rules for this slicing configuration.
     */
    public @NonNull List<UrspRule> getUrspRules() {
        return mUrspRules;
    }

    /**
     * @return the list of all slices for this slicing configuration.
     */
    public @NonNull List<NetworkSliceInfo> getSliceInfo() {
        return mSliceInfo;
    }

    @Override
    public void writeToParcel(@NonNull Parcel dest, int flags) {
        dest.writeTypedList(mUrspRules, flags);
        dest.writeTypedList(mSliceInfo, flags);
    }

    public static final @NonNull Parcelable.Creator<NetworkSlicingConfig> CREATOR =
            new Parcelable.Creator<NetworkSlicingConfig>() {
                @Override
                public NetworkSlicingConfig createFromParcel(Parcel source) {
                    return new NetworkSlicingConfig(source);
                }

                @Override
                public NetworkSlicingConfig[] newArray(int size) {
                    return new NetworkSlicingConfig[size];
                }
            };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public boolean equals(@Nullable Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        NetworkSlicingConfig that = (NetworkSlicingConfig) o;
        return mUrspRules.size() == that.mUrspRules.size()
                && mUrspRules.containsAll(that.mUrspRules)
                && mSliceInfo.size() == that.mSliceInfo.size()
                && mSliceInfo.containsAll(that.mSliceInfo);
    }

    @Override
    public int hashCode() {
        return Objects.hash(mUrspRules, mSliceInfo);
    }

    @Override
    public String toString() {
        return "{.urspRules = " + mUrspRules + ", .sliceInfo = " + mSliceInfo + "}";
    }

    // Returing informaiton can have multiple entry for each field, need to split into array
    public static TrafficDescriptor convertTrafficDescriptor(vendor.samsung_slsi.telephony.hardware.radio.V1_2.TrafficDescriptor td) {
        IpDescriptors ipDescriptors;
        if (td.ipDescriptors.getDiscriminator() == OptionalIpDescriptors.hidl_discriminator.noinit) {
            ipDescriptors = null;
        } else {
            vendor.samsung_slsi.telephony.hardware.radio.V1_2.IpDescriptors ipHalInfo = td.ipDescriptors.value();
            ipDescriptors = new IpDescriptors(ipHalInfo.ipv4, ipHalInfo.maskV4,
                              ipHalInfo.ipv6, ipHalInfo.prefixLength,
                              ipHalInfo.port,
                              ipHalInfo.portStartRange,
                              ipHalInfo.portEndRange,
                              ipHalInfo.protocolId);
           // Rlog.d("NSC", "is3Tuple=" + ipHalInfo.is3Tuple);
        }
        String dnn;
        if (td.dnn.getDiscriminator() != OptionalDnn.hidl_discriminator.noinit) {
            List<String> dnns = Arrays.asList(td.dnn.value().split(" "));
            if (dnns.size() > 0) dnn = dnns.get(0);
            else dnn= null;
        } else {
            dnn = null;
        }
        // This will return full data OSID + OSAPPID_LEN + OSAPPID
        String osAppId;
        if (td.osAppId.getDiscriminator() != OptionalOsAppId.hidl_discriminator.noinit) {
            ArrayList<byte[]> arrayOsAppId = VendorRIL.splitOsAppId(RILUtils.arrayListToPrimitiveArray(td.osAppId.value().osAppId));
            //byte[] firstOsAppid = RIL.extractOsAppId(td.osAppId.value().osAppId);
            if (arrayOsAppId.size() > 0)
                osAppId = new String(arrayOsAppId.get(0));
            else
                osAppId = null;
        } else {
            osAppId = null;
        }
        // Rlog.d("NSC", "length=" + osAppId.length() + ", OSID=" + new String(RIL.extractOsId(osAppId)) + ", OsAppId=" + RIL.extractOsAppId(osAppId));

        /*
        // This will just return OsAppId
        byte[] osId = new byte[16];
        String osAppId;
        if (td.osAppId.getDiscriminator() != OptionalOsAppId.hidl_discriminator.noinit &&
            td.osAppId.value().osAppId.size() > 16) {
            int length = td.osAppId.value().osAppId.get(16);
            byte[] osappid = new byte[length];
            int i;
            for (i = 0; i < 16; i++) osId[i] = td.osAppId.value().osAppId.get(i); // not used
            for (i = 0; i < td.osAppId.value().osAppId.get(16); i++) {
                osappid[i] = td.osAppId.value().osAppId.get(i + 17);
            }
            osappid[i] = 0;
            osAppId = new String(osappid);
        } else {
            osAppId = null;
        }
        */

        String domainDescriptors;
        if (td.domainDescriptors.getDiscriminator() != OptionalDomainDescriptors.hidl_discriminator.noinit) {
            List<String> fqdns = Arrays.asList(td.domainDescriptors.value().split(" "));
            domainDescriptors = fqdns.get(0);
        } else {
            domainDescriptors = null;
        }
        String connectionCapabilities;
        if (td.connectionCapabilities.getDiscriminator() != OptionalConnectionCapabilities.hidl_discriminator.noinit) {
            List<String> ccs = Arrays.asList(td.connectionCapabilities.value().split(" "));
            connectionCapabilities = ccs.get(0);
        } else {
            connectionCapabilities = null;
        }
        // Rlog.d("NSC", "tdMask=" + td.tdMask);

        // Matchall false : not much meaningful for response
        //return new TrafficDescriptor(false, osAppId, ipDescriptors, domainDescriptors, dnn, connectionCapabilities);
        return new TrafficDescriptor(false, osAppId, ipDescriptors, domainDescriptors, dnn, connectionCapabilities);
    }

}
