# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_BMA421		bma421)
zephyr_library_sources_ifdef(CONFIG_HRS3300		hrs3300)
zephyr_library_sources_ifdef(CONFIG_CST816S		cst816s)

