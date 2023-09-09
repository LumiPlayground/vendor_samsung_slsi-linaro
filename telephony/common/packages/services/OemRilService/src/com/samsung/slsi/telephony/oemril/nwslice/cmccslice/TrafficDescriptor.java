/*
 * Copyright (c) 2021 China Mobile Group Device Co.,Ltd. All Rights Reserved.
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

package android.cmccslice;

import android.annotation.NonNull;
import android.annotation.Nullable;
import android.os.Parcel;
import android.os.Parcelable;
import android.text.TextUtils;

import java.util.Objects;

public final class TrafficDescriptor implements Parcelable {

    private String osAppId = "";
    private IpDescriptors ipDescriptors;
    private String domainDescriptors = "";
    private String dnn = "";
    private String connectionCapabilities = "";
    private boolean matchAll;

    public void setOsAppId(@NonNull String osAppId) {
        this.osAppId = osAppId;
    }

    public void setIpDescriptors(@NonNull IpDescriptors ipDescriptors) {
        this.ipDescriptors = ipDescriptors;
    }

    public void setDomainDescriptors(@NonNull String domainDescriptors) {
        this.domainDescriptors = domainDescriptors;
    }

    public void setDnn(@NonNull String dnn) {
        this.dnn = dnn;
    }

    public void setConnectionCapabilities(@NonNull String connectionCapabilities) {
        this.connectionCapabilities = connectionCapabilities;
    }

    public @Nullable String getOsAppId() {
        return osAppId;
    }

    public @Nullable IpDescriptors getIpDescriptors() {
        return ipDescriptors;
    }

    public @Nullable String getDomainDescriptors() {
        return domainDescriptors;
    }

    public @Nullable String getDnn() {
        return dnn;
    }

    public @Nullable String getConnectionCapabilities() {
        return connectionCapabilities;
    }

    public boolean isMatchAll() {
        return matchAll;
    }

    public void setMatchAll(boolean matchAll) {
        this.matchAll = matchAll;
    }

    public TrafficDescriptor() {
    }

    public TrafficDescriptor(boolean matchAll,
                             @Nullable String osAppId,
                             @Nullable IpDescriptors ipDescriptors,
                             @Nullable String domainDescriptors,
                             @Nullable String dnn,
                             @Nullable String connectionCapabilities) {
        this.matchAll = matchAll;
        this.osAppId = osAppId;
        this.ipDescriptors = ipDescriptors;
        this.domainDescriptors = domainDescriptors;
        this.dnn = dnn;
        this.connectionCapabilities = connectionCapabilities;
    }

    @Nullable
    public static final Creator<TrafficDescriptor> CREATOR = new Creator<TrafficDescriptor>() {
        @Override
        public TrafficDescriptor createFromParcel(Parcel in) {
            return new TrafficDescriptor(in);
        }

        @Override
        public TrafficDescriptor[] newArray(int size) {
            return new TrafficDescriptor[size];
        }
    };

    protected TrafficDescriptor(@NonNull Parcel in) {
        matchAll = in.readInt() != 0;
        osAppId = in.readString();
        ipDescriptors = in.readParcelable(Thread.currentThread().getContextClassLoader());
        domainDescriptors = in.readString();
        dnn = in.readString();
        connectionCapabilities = in.readString();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(@NonNull Parcel parcel, int i) {
        parcel.writeInt(matchAll ? 1 : 0);;
        parcel.writeString(osAppId);
        parcel.writeParcelable(ipDescriptors, 0);
        parcel.writeString(domainDescriptors);
        parcel.writeString(dnn);
        parcel.writeString(connectionCapabilities);
    }

    @Override
    public String toString() {
        return "TrafficDescriptor{" +
                "matchAll= '" + matchAll + '\'' +
                ", osAppId='" + osAppId + '\'' +
                ", ipDescriptors=" + ipDescriptors +
                ", domainDescriptors='" + domainDescriptors + '\'' +
                ", dnn='" + dnn + '\'' +
                ", connectionCapabilities='" + connectionCapabilities + '\'' +
                '}';
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj == null) {
            return false;
        }
        if (this == obj) {
            return true;
        }
        if (obj instanceof TrafficDescriptor) {
            TrafficDescriptor trafficDescriptor = (TrafficDescriptor) obj;
            boolean result = trafficDescriptor.matchAll == this.matchAll
                    && equals(trafficDescriptor.getOsAppId(), this.osAppId)
                    && equals(trafficDescriptor.getConnectionCapabilities(), this.connectionCapabilities)
                    && equals(trafficDescriptor.getDnn(), this.dnn)
                    && equals(trafficDescriptor.getDomainDescriptors(), this.domainDescriptors);
            if (trafficDescriptor.getIpDescriptors() != null) {
                return result && trafficDescriptor.getIpDescriptors().equals(this.ipDescriptors);
            } else {
                if (this.ipDescriptors == null) {
                    return result;
                } else {
                    boolean res = TextUtils.isEmpty(ipDescriptors.getIpv4())
                            && TextUtils.isEmpty(ipDescriptors.getMaskV4())
                            && TextUtils.isEmpty(ipDescriptors.getIpv6())
                            && ipDescriptors.getPrefixLength() == 0
                            && ipDescriptors.getDestPort() == 0
                            && ipDescriptors.getDestPortStartRange() == 0
                            && ipDescriptors.getDestPortEndRange() == 0
                            && ipDescriptors.getProtocol() == 0;
                    return result && res;
                }
            }
        }

        return false;
    }

    private boolean equals(CharSequence a, CharSequence b) {
        if (a == null && b == null) return true;
        if (a == b) return true;
        int length;
        if (a != null && b != null && (length = a.length()) == b.length()) {
            if (a instanceof String && b instanceof String) {
                return a.equals(b);
            } else {
                for (int i = 0; i < length; i++) {
                    if (a.charAt(i) != b.charAt(i)) return false;
                }
                return true;
            }
        }
        return false;
    }

    @Override
    public int hashCode() {
        return Objects.hash(matchAll, osAppId, ipDescriptors, domainDescriptors, dnn, connectionCapabilities);
    }


}
