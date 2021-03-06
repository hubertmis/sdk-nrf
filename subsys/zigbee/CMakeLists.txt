#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

zephyr_interface_library_named(zigbee)

target_include_directories(zigbee INTERFACE common)
target_include_directories(zigbee INTERFACE osif)
target_include_directories(zigbee INTERFACE zb_error)

zephyr_library()

zephyr_library_link_libraries(zigbee)

# Source files
zephyr_library_sources(osif/zb_nrf_platform.c)
zephyr_library_sources(osif/zb_nrf_logger.c)
zephyr_library_sources(osif/zb_nrf_nvram.c)
zephyr_library_sources(osif/zb_nrf_timer.c)
zephyr_library_sources(osif/zb_nrf_transceiver.c)
zephyr_library_sources(osif/zb_nrf_crypto.c)
zephyr_library_sources(osif/zb_nrf_pwr_mgmt.c)
zephyr_library_sources(osif/nrf_ecb_driver.c)
zephyr_library_sources(common/zigbee_helpers.c)
zephyr_library_sources(common/zigbee_logger_eprxzcl.c)

zephyr_library_link_libraries(zboss)
