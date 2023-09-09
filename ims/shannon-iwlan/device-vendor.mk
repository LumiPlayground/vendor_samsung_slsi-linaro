$(warning "Building IWLAN Services with source code..")

DEVICE_PACKAGE_OVERLAYS += \
    vendor/samsung_slsi/ims/shannon-iwlan/overlay

PRODUCT_COPY_FILES += \
    vendor/samsung_slsi/ims/shannon-iwlan/oem-iptables-init.sh:system/bin/oem-iptables-init.sh

PRODUCT_PACKAGES += \
    ShannonNetworkService \
    ShannonDataService
