# SPDX-License-Identifier: Apache-2.0

add_subdirectory(drivers)
add_subdirectory_ifdef(CONFIG_BT bluetooth)

zephyr_include_directories(include)