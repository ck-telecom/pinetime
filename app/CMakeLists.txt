# SPDX-License-Identifier: Apache-2.0

# Add out-off-tree drivers
# list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/drivers/hrs3300)
#list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/drivers/bma421)

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(pinetime VERSION 0.1.1)

target_sources(app PRIVATE src/main.c)
#target_sources(app PRIVATE src/service/button.c)
#target_sources(app PRIVATE src/service/button_cfg.c)
#target_sources(app PRIVATE src/service/vibrator.c)
#target_sources(app PRIVATE src/service/display.c)
#target_sources(app PRIVATE src/service/touch.c)
#target_sources(app PRIVATE src/service/bma4xx.c)
#target_sources(app PRIVATE src/service/charger.c)
#target_sources(app PRIVATE src/service/battery.c)
#target_sources(app PRIVATE src/service/event_service.c)
#target_sources(app PRIVATE src/service/display_power.c)
#target_sources(app PRIVATE src/service/backlight.c)
#target_sources(app PRIVATE src/event/event_service.c)
#target_sources(app PRIVATE src/event/event_service_mem.c)

target_sources(app PRIVATE src/bluetooth/bt.c)

target_sources(app PRIVATE src/apps/app.c)
target_sources(app PRIVATE src/apps/app/clock.c)
#target_sources(app PRIVATE src/apps/app/music.c)
target_sources(app PRIVATE src/apps/app/passkey.c)

zephyr_include_directories(src)
zephyr_include_directories(src/bluetooth)
zephyr_include_directories(src/bluetooth/service)
zephyr_include_directories(src/apps)

add_subdirectory(src)

execute_process(COMMAND git rev-parse --short HEAD
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE PROJECT_GIT_COMMIT_HASH
                RESULT_VARIABLE PROJECT_GIT_COMMIT_HASH_SUCCESS)

message("---------------------------------")
message("    * Version : " ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
message("    * GitRef(S) :" ${PROJECT_GIT_COMMIT_HASH})
message("---------------------------------")
