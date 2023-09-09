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


public final class IpDescriptors implements Parcelable {
    private String ipv4 = "";
    private String maskV4 = "";
    private String ipv6 = "";
    private int prefixLength;
    private int destPort;
    private int destPortStartRange;
    private int destPortEndRange;
    private int protocol;

    public void setIpv4(@NonNull String ipv4) {
        this.ipv4 = ipv4;
    }

    public void setMaskV4(@NonNull String maskV4) {
        this.maskV4 = maskV4;
    }

    public void setIpv6(@NonNull String ipv6) {
        this.ipv6 = ipv6;
    }

    public void setPrefixLength(int prefixLength) {
        this.prefixLength = prefixLength;
    }

    public void setDestPort(int destPort) {
        this.destPort = destPort;
    }

    public void setDestPortStartRange(int destPortStartRange) {
        this.destPortStartRange = destPortStartRange;
    }

    public void setDestPortEndRange(int destPortEndRange) {
        this.destPortEndRange = destPortEndRange;
    }

    public void setProtocol(int protocol) {
        this.protocol = protocol;
    }


    public @Nullable String getIpv4() {
        return ipv4;
    }

    public @Nullable String getMaskV4() {
        return maskV4;
    }

    public @Nullable String getIpv6() {
        return ipv6;
    }

    public int getPrefixLength() {
        return prefixLength;
    }

    public int getDestPort() {
        return destPort;
    }

    public int getDestPortStartRange() {
        return destPortStartRange;
    }

    public int getDestPortEndRange() {
        return destPortEndRange;
    }

    public int getProtocol() {
        return protocol;
    }

    public IpDescriptors() {
    }

    protected IpDescriptors(@NonNull Parcel in) {
        ipv4 = in.readString();
        maskV4 = in.readString();
        ipv6 = in.readString();
        prefixLength = in.readInt();
        destPort = in.readInt();
        destPortStartRange = in.readInt();
        destPortEndRange = in.readInt();
        protocol = in.readInt();
    }

    public IpDescriptors(@Nullable String ipv4,
                         @Nullable String maskV4,
                         @Nullable String ipv6,
                         int prefixLength,
                         int destPort,
                         int destPortStartRange,
                         int destPortEndRange,
                         int protocol) {
        this.ipv4 = ipv4;
        this.maskV4 = maskV4;
        this.ipv6 = ipv6;
        this.prefixLength = prefixLength;
        this.destPort = destPort;
        this.destPortStartRange = destPortStartRange;
        this.destPortEndRange = destPortEndRange;
        this.protocol = protocol;
    }

    @Override
    public void writeToParcel(@NonNull Parcel dest, int flags) {
        dest.writeString(ipv4);
        dest.writeString(maskV4);
        dest.writeString(ipv6);
        dest.writeInt(prefixLength);
        dest.writeInt(destPort);
        dest.writeInt(destPortStartRange);
        dest.writeInt(destPortEndRange);
        dest.writeInt(protocol);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Nullable
    public static final Creator<IpDescriptors> CREATOR = new Creator<IpDescriptors>() {
        @Override
        public IpDescriptors createFromParcel(@NonNull Parcel in) {
            return new IpDescriptors(in);
        }

        @Override
        public IpDescriptors[] newArray(int size) {
            return new IpDescriptors[size];
        }
    };

    @Override
    public String toString() {
        return "IpDescriptors{" +
                "ipv4='" + ipv4 + '\'' +
                ", maskV4='" + maskV4 + '\'' +
                ", ipv6='" + ipv6 + '\'' +
                ", prefixLength=" + prefixLength +
                ", destPort=" + destPort +
                ", destPortStartRange=" + destPortStartRange +
                ", destPortEndRange=" + destPortEndRange +
                ", protocol=" + protocol +
                '}';
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj == null) {
            boolean re = TextUtils.isEmpty(ipv4)
                    && TextUtils.isEmpty(maskV4)
                    && TextUtils.isEmpty(ipv6)
                    && prefixLength == 0
                    && destPort == 0
                    && destPortStartRange == 0
                    && destPortEndRange == 0
                    && protocol == 0;
            return re;
        }
        if (this == obj) {
            return true;
        }
        if (obj instanceof IpDescriptors) {
            IpDescriptors ipDescriptors = (IpDescriptors) obj;
            boolean result = equals(ipDescriptors.getIpv4(), this.ipv4)
                    && equals(ipDescriptors.getMaskV4(), this.maskV4)
                    && equals(ipDescriptors.getIpv6(), this.ipv6)
                    && ipDescriptors.getPrefixLength() == this.prefixLength
                    && ipDescriptors.getDestPort() == this.destPort
                    && ipDescriptors.getProtocol() == this.protocol
                    && ipDescriptors.getDestPortStartRange() == this.destPortStartRange
                    && ipDescriptors.getDestPortEndRange() == this.destPortEndRange;
            return result;
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
        return Objects.hash(ipv4, maskV4, ipv6, prefixLength, destPort, destPortStartRange, destPortEndRange, protocol);
    }
}
