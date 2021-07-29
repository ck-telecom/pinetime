# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# Add the board
# list(APPEND BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
# list(APPEND DTS_ROOT ${CMAKE_CURRENT_LIST_DIR})

# Add out-off-tree drivers
# list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/drivers/hrs3300)
# list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/drivers/bma421)
# KSCAN can not be out-of-tree built
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/drivers/cst816s)


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(pinetime)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/service/button.c)
target_sources(app PRIVATE src/service/button_cfg.c)
target_sources(app PRIVATE src/service/vibrator.c)
target_sources(app PRIVATE src/service/display.c)
# target_sources(app PRIVATE src/event.c)
