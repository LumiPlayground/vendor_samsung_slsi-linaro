package com.samsung.slsi.telephony.oemril;

import android.annotation.NonNull;
import android.annotation.Nullable;
import android.os.Parcel;
import android.os.Parcelable;

import java.util.Arrays;
import java.util.Objects;
import android.cmccslice.IpDescriptors;

/** {@hide} */
public final class TrafficDescriptor_orgext implements Parcelable {
    private final String mDnn;
    private final byte[] mOsAppId;

    private  IpDescriptors ipDescriptors;
    private  String domainDescriptors;
    private  String connectionCapabilities;

    /*
    private TrafficDescriptor(@NonNull Parcel in) {
        mDnn = in.readString();
        mOsAppId = in.createByteArray();
    }
    */

    /**
     * Create a traffic descriptor, as defined in 3GPP TS 24.526 Section 5.2
     * @param dnn optional DNN, which must be used for traffic matching, if present
     * @param osAppId OsId + osAppId of the traffic descriptor
     *
     * @hide
     */
    public TrafficDescriptor_orgext(String dnn, byte[] osAppId) {
        mDnn = dnn;
        mOsAppId = osAppId;
    }

    /**
     * DNN stands for Data Network Name and represents an APN as defined in 3GPP TS 23.003.
     * @return the DNN of this traffic descriptor if one is included by the network, null
     * otherwise.
     */
    public @Nullable String getDataNetworkName() {
        return mDnn;
    }

    /**
     * OsAppId is the app id as defined in 3GPP TS 24.526 Section 5.2, and it identifies a traffic
     * category. It includes the OS Id component of the field as defined in the specs.
     * @return the OS App ID of this traffic descriptor if one is included by the network, null
     * otherwise.
     */
    public @Nullable byte[] getOsAppId() {
        return mOsAppId;
    }

    @Override
    public int describeContents() {
        return 0;
    }


    /**
     * Extension
     */

    public @Nullable String getDnn() {
        return mDnn;
    }

    public @Nullable IpDescriptors getIpDescriptors() {
        return ipDescriptors;
    }

    public @Nullable String getDomainDescriptors() {
        return domainDescriptors;
    }

    public @Nullable String getConnectionCapabilities() {
        return connectionCapabilities;
    }

    public TrafficDescriptor_orgext(String dnn, byte[] osAppId, 
                             IpDescriptors ipDescriptors,
                             String domainDescriptors,
                             String connectionCapabilities) {
        mDnn = dnn;
        mOsAppId = osAppId;

        this.ipDescriptors = ipDescriptors;
        this.domainDescriptors = domainDescriptors;
        this.connectionCapabilities = connectionCapabilities;
    }

    protected TrafficDescriptor_orgext(@Nullable Parcel in) {
        mDnn = in.readString();
        mOsAppId = in.createByteArray();

        ipDescriptors = in.readParcelable(Thread.currentThread().getContextClassLoader());
        domainDescriptors = in.readString();
        connectionCapabilities = in.readString();
    }

    @Override
    public String toString() {
        return "TrafficDescriptor_orgext{mDnn=" + mDnn + ", mOsAppId=" + mOsAppId +
                ", ipDescriptors=" + ipDescriptors +
                ", domainDescriptors='" + domainDescriptors + '\'' +
                ", connectionCapabilities='" + connectionCapabilities + '\'' +
                '}';
    }

    @Override
    public void writeToParcel(@NonNull Parcel dest, int flags) {
        dest.writeString(mDnn);
        dest.writeByteArray(mOsAppId);

        dest.writeParcelable(ipDescriptors, 0);
        dest.writeString(domainDescriptors);
        dest.writeString(connectionCapabilities);
    }

    public static final @NonNull Parcelable.Creator<TrafficDescriptor_orgext> CREATOR =
        new Parcelable.Creator<TrafficDescriptor_orgext>() {
            @Override
            public @NonNull TrafficDescriptor_orgext createFromParcel(@NonNull Parcel source) {
                return new TrafficDescriptor_orgext(source);
            }

            @Override
            public @NonNull TrafficDescriptor_orgext[] newArray(int size) {
                return new TrafficDescriptor_orgext[size];
            }
        };


    @Override
    public boolean equals(@Nullable Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        TrafficDescriptor_orgext that = (TrafficDescriptor_orgext) o;
        return Objects.equals(mDnn, that.mDnn) && Arrays.equals(mOsAppId, that.mOsAppId) &&
            Objects.equals(ipDescriptors, that.ipDescriptors) &&
            Objects.equals(domainDescriptors, that.domainDescriptors) &&
            Objects.equals(connectionCapabilities, that.connectionCapabilities);
    }

    @Override
    public int hashCode() {
        return Objects.hash(mDnn, mOsAppId, ipDescriptors, domainDescriptors, connectionCapabilities);
    }

    /**
     * Provides a convenient way to set the fields of a {@link TrafficDescriptor} when creating a
     * new instance.
     *
     * <p>The example below shows how you might create a new {@code TrafficDescriptor}:
     *
     * <pre><code>
     *
     * TrafficDescriptor response = new TrafficDescriptor.Builder()
     *     .setDnn("")
     *     .build();
     * </code></pre>
     *
     */
    public static final class Builder {
        private String mDnn = null;
        private byte[] mOsAppId = null;

        private  IpDescriptors ipDescriptors;
        private  String domainDescriptors;
        private  String connectionCapabilities;

        /**
         * Default constructor for Builder.
         */
        public Builder() {
        }

        /**
         * Set the Data Network Name(DNN).
         *
         * @return The same instance of the builder.
         */
        @NonNull
        public Builder setDataNetworkName(@NonNull String dnn) {
            this.mDnn = dnn;
            return this;
        }

        /**
         * Set the OS App ID (including OS Id as defind in the specs).
         *
         * @return The same instance of the builder.
         */
        @NonNull
        public Builder setOsAppId(@NonNull byte[] osAppId) {
            this.mOsAppId = osAppId;
            return this;
        }

        @NonNull
        public Builder setDnn(@NonNull String dnn) {
            this.mDnn = dnn;
            return this;
        }

        @NonNull
        public Builder setIpDescriptors(@NonNull IpDescriptors ipDescriptors) {
            this.ipDescriptors = ipDescriptors;
            return this;
        }

        @NonNull
        public Builder setDomainDescriptors(@NonNull String domainDescriptors) {
            this.domainDescriptors = domainDescriptors;
            return this;
        }

        @NonNull
        public Builder setConnectionCapabilities(@NonNull String connectionCapabilities) {
            this.connectionCapabilities = connectionCapabilities;
            return this;
        }


        /**
         * Build the {@link TrafficDescriptor}.
         *
         * @throws IllegalArgumentException if DNN and OS App ID are null.
         *
         * @return the {@link TrafficDescriptor} object.
         */
        @NonNull
        public TrafficDescriptor_orgext build() {
            if (this.mDnn == null && this.mOsAppId == null &&
                this.ipDescriptors == null && this.domainDescriptors == null &&
                this.connectionCapabilities == null) {
                throw new IllegalArgumentException("DNN and OS App ID and all other field are null");
            }
            return new TrafficDescriptor_orgext(this.mDnn, this.mOsAppId, 
                                         this.ipDescriptors,
                                         this.domainDescriptors,
                                         this.connectionCapabilities);
        }
    }

}
