# SPDX-License-Identifier: Apache-2.0

# Add out-off-tree drivers
# list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/drivers/hrs3300)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/drivers/bma421)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/drivers/cst816s)

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(pinetime)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/service/button.c)
target_sources(app PRIVATE src/service/button_cfg.c)
target_sources(app PRIVATE src/service/vibrator.c)
target_sources(app PRIVATE src/service/display.c)
target_sources(app PRIVATE src/service/touch.c)
target_sources(app PRIVATE src/backlight.c)
target_sources(app PRIVATE src/battery.c)
