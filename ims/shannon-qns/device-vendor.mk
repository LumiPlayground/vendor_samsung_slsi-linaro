$(warning "Building QNS with source code..")

DEVICE_PACKAGE_OVERLAYS += \
    vendor/samsung_slsi/ims/shannon-qns/overlay

PRODUCT_PACKAGES += \
    ShannonQualifiedNetworksService
