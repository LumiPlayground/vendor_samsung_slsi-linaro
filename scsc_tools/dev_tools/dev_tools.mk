ifeq ($(CONFIG_SAMSUNG_SCSC_WIFIBT),true)

PRODUCT_PACKAGES_ENG += moredump \
			wlbt_onoff.sh \
			disable_auto_coredump \
			scsc_get_platform_info.sh \
			memload \
			hw_scsc_devmem2 \
			hw_scsc_mxlogger \

PRODUCT_PACKAGES_DEBUG += moredump \
			trigger_moredump \
			wlbt_test_bt_on_off.sh \
			omnicli \
			if_unifi.so \
			btcli \
			wlan_debug_level.sh \
			enable_wlbt_log \
			scsc_enable_flight_mode.sh \
			tgen \
			mxdecoder \
			slsi_wlan_mib \
			slsi_wlan_udi_log \
			slsi_wlan_udi_log_decode \
			slsi_wlan_src_sink \
			slsi_wlan_loopback_config \
			vectordriverbroker \
			vectordriverbroker.bin \
			devmem2 \
			wlbt_ldos.sh \
			wlbtd \
			mx_logger.sh \
			mx_logger_dump.sh \
			mx_log_collection.sh \
			cpu_stats.sh

endif

