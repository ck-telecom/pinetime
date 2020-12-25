# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_KSCAN_CST816S kscan_cst816s.c)
