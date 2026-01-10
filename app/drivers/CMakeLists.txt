# copyright (c) 2026 Qingsong Gou <gouqs@hotmail.com>
# SPDX-License-Identifier: Apache-2.0

add_library(pbldrivers STATIC stubs/battery.c)

target_include_directories(pbldrivers PUBLIC .)
