/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "applib/graphics/framebuffer.h"

#include "applib/graphics/gtypes.h"
#include "system/logging.h"
#include "system/passert.h"
#include "util/bitset.h"
#include "drivers/display/display.h"

#include <stdint.h>
#include <string.h>

volatile const int FrameBuffer_MaxX = DISP_COLS;
volatile const int FrameBuffer_MaxY = DISP_ROWS;
volatile const int FrameBuffer_BytesPerRow = FRAMEBUFFER_BYTES_PER_ROW;

uint32_t *framebuffer_get_line(FrameBuffer *f, uint8_t y) {
  PBL_ASSERTN(y < f->size.h);

  return f->buffer + (y * ((f->size.w / 32) + 1));
}

inline size_t framebuffer_get_size_bytes(FrameBuffer *f) {
  // TODO: Make FRAMEBUFFER_SIZE_BYTES a macro which takes the cols and rows if we ever want to
  // support different size framebuffers for watches which have native 1-bit framebuffers where the
  // size is not just COLS * ROWS.
  return FRAMEBUFFER_SIZE_BYTES;
}

void framebuffer_clear(FrameBuffer *f) {
  memset(f->buffer, 0xff, framebuffer_get_size_bytes(f));
  framebuffer_dirty_all(f);
  f->is_dirty = true;

  // Update Zephyr display
  display_clear();
}

void framebuffer_mark_dirty_rect(FrameBuffer *f, GRect rect) {
  if (!f->is_dirty) {
    f->dirty_rect = rect;
  } else {
    f->dirty_rect = grect_union(&f->dirty_rect, &rect);
  }

  const GRect clip_rect = (GRect) { GPointZero, f->size };
  grect_clip(&f->dirty_rect, &clip_rect);

  f->is_dirty = true;
}

// Static variables for framebuffer flush
static FrameBuffer *current_flush_framebuffer = NULL;
static uint8_t current_flush_start_row = 0;
static uint8_t current_flush_end_row = 0;
static uint8_t current_flush_current_row = 0;

// Next row callback for display_update
static bool framebuffer_next_row_callback(DisplayRow* row) {
  if (current_flush_current_row >= current_flush_end_row) {
    return false;
  }

  row->address = current_flush_current_row;
  // Get the line from framebuffer and convert to bytes (each uint32_t contains 4 bytes)
  row->data = (uint8_t*)framebuffer_get_line(current_flush_framebuffer, current_flush_current_row);

  current_flush_current_row++;
  return true;
}

// Update complete callback for display_update
static void framebuffer_update_complete_callback(void) {
  if (current_flush_framebuffer) {
    framebuffer_reset_dirty(current_flush_framebuffer);
    LOG_DBG("Framebuffer flush completed");
    current_flush_framebuffer = NULL;
  }
}

// New function to update display from framebuffer
void framebuffer_flush(FrameBuffer *f) {
  if (!f->is_dirty) {
    return;
  }

  // Convert 1-bit framebuffer to display format and update display
  if (display_update_in_progress()) {
    return;
  }

  // Get the dirty region and convert to display coordinates
  GRect dirty_rect = f->dirty_rect;
  if (grect_is_empty(&dirty_rect)) {
    framebuffer_reset_dirty(f);
    return;
  }

  // Convert dirty region to absolute coordinates
  current_flush_start_row = dirty_rect.origin.y;
  current_flush_end_row = dirty_rect.origin.y + dirty_rect.size.h;

  LOG_DBG("Flushing framebuffer, dirty rect: x=%d, y=%d, w=%d, h=%d",
          dirty_rect.origin.x, dirty_rect.origin.y,
          dirty_rect.size.w, dirty_rect.size.h);

  // Set static variables for callbacks
  current_flush_framebuffer = f;
  current_flush_current_row = current_flush_start_row;

  // Update display with the dirty region
  display_update(framebuffer_next_row_callback, framebuffer_update_complete_callback);
}
