# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# Add the board
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
list(APPEND DTS_ROOT ${CMAKE_CURRENT_LIST_DIR})

# Add out-off-tree drivers
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/drivers/hrs3300)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/drivers/bma421)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/drivers/cst816s)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(pinetime)

target_sources(app PRIVATE src/main.c)
