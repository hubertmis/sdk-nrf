/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/cache.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/mspi.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(idle_exmif);

#ifdef CONFIG_MSPI_DW
#define MSPI_AVAILABLE 1 // TODO: this is ugly way of testing if MSPI is available
#endif

#if MSPI_AVAILABLE
#define MSPI_BUS                  DT_BUS(DT_ALIAS(dev0))
#define MSPI_TARGET               DT_ALIAS(dev0)

static const uint8_t data_block_cmd = 0x32;
static const uint32_t data_block_address = 0x002c00;
static uint8_t data_block_data[4 * 1024];

const struct mspi_xfer_packet data_block_packet[] = {
	{
		.dir                = MSPI_TX,
		.cmd                = data_block_cmd,
		.address            = data_block_address,
		.num_bytes          = sizeof(data_block_data),
		.data_buf           = data_block_data,
	},
};

const struct mspi_xfer data_block_xfer = {
	.xfer_mode                  = MSPI_DMA,
	.cmd_length                 = 1,
	.addr_length                = 3,
	.priority                   = 1,
	.packets                    = data_block_packet,
	.num_packet                 = 1,
	.timeout                    = 2500,
};
#endif /* MSPI_AVAILABLE */

int main(void)
{
	unsigned int cnt = 0;

#if MSPI_AVAILABLE
	const struct device *controller = DEVICE_DT_GET(MSPI_BUS);
	struct mspi_dev_id dev_id = MSPI_DEVICE_ID_DT(MSPI_TARGET);
	int ret;
	/* Initialize write buffer */
	for (int i = 0; i < ARRAY_SIZE(data_block_data); i++) {
		data_block_data[i] = (uint8_t)i;
	}
	// TODO: cache should be handled by the driver. Is this code needed here?
	ret = sys_cache_data_flush_range(data_block_data, sizeof(data_block_data));
	if (ret) {
		LOG_ERR("Failed to flush cache\n");
		return 1;
	}

	const struct mspi_dev_cfg data_block_cfg = {
		.freq = 40000000,
		.io_mode = MSPI_IO_MODE_QUAD_1_1_4,
	};
#endif /* MSPI_AVAILABLE */

#if defined CONFIG_FIRST_SLEEP_OFFSET
	k_msleep(1000);
#endif

	LOG_INF("Multicore idle test on %s", CONFIG_BOARD_TARGET);
#ifdef CONFIG_SOC_NRF54H20_CPUAPP
	while (1) {
		LOG_INF("Multicore idle_exmif test iteration %u", cnt++);

#if MSPI_AVAILABLE
		pm_device_runtime_get(controller);
		ret = mspi_dev_config(controller, &dev_id,
							MSPI_DEVICE_CONFIG_FREQUENCY |
							MSPI_DEVICE_CONFIG_IO_MODE,
							&data_block_cfg);
		if (ret) {
			LOG_ERR("Failed to configure 1 1 4 mode\n");
		}

		ret = mspi_transceive(controller, &dev_id, &data_block_xfer);
		if (ret) {
			LOG_ERR("Failed to send data\n");
		}

		pm_device_runtime_put(controller);
#endif /* MSPI_AVAILABLE */

		k_msleep(2000);
	}
#endif

	return 0;
}
