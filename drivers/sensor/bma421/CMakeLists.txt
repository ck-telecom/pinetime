# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_include_directories(.)
zephyr_include_directories(bma421)

zephyr_library_sources_ifdef(CONFIG_BMA421 bma421/bma4.c)
zephyr_library_sources_ifdef(CONFIG_BMA421 bma421/bma421.c)

zephyr_library_sources_ifdef(CONFIG_BMA421 bma421.c)
zephyr_library_sources_ifdef(CONFIG_BMA421_TRIGGER bma421_trigger.c)
