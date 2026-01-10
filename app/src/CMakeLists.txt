# SPDX-FileCopyrightText: 2024 Google LLC
# SPDX-License-Identifier: Apache-2.0

# Add app_adapter sources to the build
target_sources(app PRIVATE
  app_adapter.c
)

# Add include directories
target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../pebbleos/src/fw
)

# Add PebbleOS applib sources to the build
# We'll add them one by one as we port them
target_sources(app PRIVATE
  # Add applib sources here as we port them
  # ../../pebbleos/src/fw/applib/app.c
)
