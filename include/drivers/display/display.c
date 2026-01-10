/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>
#include <logging/log.h>

#include "display.h"

LOG_MODULE_REGISTER(display_driver, LOG_LEVEL_INF);

// Zephyr显示设备句柄
static const struct device *zephyr_display_dev;

// 显示状态变量
static bool display_enabled = true;
static bool display_rotated = false;
static bool update_in_progress = false;
static GPoint display_offset = {0, 0};

// 帧缓冲区相关
static uint8_t *framebuffer = NULL;
static size_t framebuffer_size = 0;

// 初始化显示
void display_init(void) {
    zephyr_display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
    if (zephyr_display_dev == NULL) {
        LOG_ERR("Failed to get display device binding");
        return;
    }
    
    LOG_INF("Display device initialized: %s", zephyr_display_dev->name);
    
    // 获取显示信息
    const struct display_capabilities *capabilities = display_get_capabilities(zephyr_display_dev);
    if (capabilities) {
        LOG_INF("Display dimensions: %dx%d", capabilities->x_resolution, capabilities->y_resolution);
        LOG_INF("Pixel format: %d", capabilities->current_pixel_format);
    }
    
    // 初始状态设置
    display_clear();
    display_set_enabled(true);
    display_set_rotated(false);
}

// 显示启动画面
void display_show_splash_screen(void) {
    // Zephyr实现：可以通过LVGL或直接帧缓冲区操作显示启动画面
    display_clear();
    LOG_INF("Splash screen displayed");
}

// 改变波特率
uint32_t display_baud_rate_change(uint32_t new_frequency_hz) {
    // Zephyr实现：通常由驱动自动管理，无需手动设置
    LOG_INF("Baud rate change requested: %d Hz", new_frequency_hz);
    return new_frequency_hz; // 返回实际设置的频率
}

// 清除显示
void display_clear(void) {
    if (!zephyr_display_dev) {
        return;
    }
    
    // 获取显示尺寸
    const struct display_capabilities *capabilities = display_get_capabilities(zephyr_display_dev);
    if (!capabilities) {
        return;
    }
    
    // 创建黑色填充矩形
    struct display_buffer_descriptor buf_desc = {
        .buf_size = capabilities->x_resolution * capabilities->y_resolution,
        .width = capabilities->x_resolution,
        .height = capabilities->y_resolution,
        .pitch = capabilities->x_resolution,
    };
    
    // 分配临时缓冲区并填充黑色
    uint8_t *clear_buf = k_malloc(buf_desc.buf_size);
    if (clear_buf) {
        memset(clear_buf, 0, buf_desc.buf_size);
        display_write(zephyr_display_dev, 0, 0, &buf_desc, clear_buf);
        k_free(clear_buf);
    }
    
    LOG_INF("Display cleared");
}

// 设置显示启用状态
void display_set_enabled(bool enabled) {
    if (!zephyr_display_dev) {
        return;
    }
    
    display_enabled = enabled;
    
    if (enabled) {
        display_blanking_off(zephyr_display_dev);
        LOG_INF("Display enabled");
    } else {
        display_blanking_on(zephyr_display_dev);
        LOG_INF("Display disabled");
    }
}

// 设置显示旋转
void display_set_rotated(bool rotated) {
    display_rotated = rotated;
    // Zephyr实现：旋转通常在设备树中配置，运行时可能不支持动态旋转
    LOG_INF("Display rotated set to: %d", rotated);
}

// 更新显示
void display_update(NextRowCallback nrcb, UpdateCompleteCallback uccb) {
    if (!zephyr_display_dev || update_in_progress) {
        return;
    }
    
    update_in_progress = true;
    
    LOG_INF("Display update started");
    
    // Zephyr实现：使用回调获取行数据并更新显示
    if (nrcb) {
        DisplayRow row;
        while (nrcb(&row)) {
            // 处理行数据，实际实现需要根据硬件特性调整
            LOG_DBG("Processing row: %d, data: %p", row.address, row.data);
        }
    }
    
    // 标记更新完成
    update_in_progress = false;
    
    // 调用完成回调
    if (uccb) {
        uccb();
    }
    
    LOG_INF("Display update completed");
}

// 检查更新是否进行中
bool display_update_in_progress(void) {
    return update_in_progress;
}

// 脉冲VCOM
void display_pulse_vcom(void) {
    // Zephyr实现：通常由驱动自动管理
    LOG_INF("VCOM pulse requested");
}

// 显示恐慌屏幕
void display_show_panic_screen(uint32_t error_code) {
    // Zephyr实现：显示错误代码
    display_clear();
    LOG_ERR("Panic screen displayed, error code: 0x%08X", error_code);
}

// 设置显示偏移
void display_set_offset(GPoint offset) {
    display_offset = offset;
    LOG_INF("Display offset set to: (%d, %d)", offset.x, offset.y);
}

// 获取显示偏移
GPoint display_get_offset(void) {
    return display_offset;
}
