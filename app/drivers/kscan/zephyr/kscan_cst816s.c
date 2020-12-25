/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT hynitron,cst816s

#include <drivers/kscan.h>
#include <drivers/i2c.h>
#include <drivers/gpio.h>

#include <logging/log.h>

#include "cst816s.h"

#define RESET_PIN       DT_INST_GPIO_PIN(0, reset_gpios)
#define INTERRUPT_PIN   DT_INST_GPIO_PIN(0, int1_gpios)


struct cst816s_data {
    const struct device *dev;
    const struct device *i2c;
    kscan_callback_t callback;
    const struct device *reset_gpio;
#ifdef CONFIG_KSCAN_CST816S_INTERRUPT
    const struct device *int_gpio;
    struct gpio_callback int_gpio_cb;
    struct k_work work;
#else
    struct k_timer timer;
#endif
};


LOG_MODULE_REGISTER(cst816s, CONFIG_KSCAN_LOG_LEVEL);


static int cst816s_process(const struct device *dev)
{
    int r;
    uint8_t buf[9] = 0;
    uint8_t msb;
    uint8_t lsb;

    uint16_t x, y;
    bool pressed;
    struct cst816s_data *drv_data = dev->data;

    r = i2c_burst_read(drv_data->i2c, /*DT_INST_REG_ADDR(0)*/CST816S_I2C_ADDRESS,
                   CST816S_REG_DATA, buf, 9);
    if (r < 0) {
        return r;
    }

    switch (buf[CST816S_REG_GESTURE_ID]) {
        case CST816S_GESTURE_NONE:
            drv_data->gesture = NONE;
            break;

        case CST816S_GESTURE_SLIDE_UP:
            drv_data->gesture = SLIDE_UP;
            break;

        case CST816S_GESTURE_SLIDE_DOWN:
            drv_data->gesture = SLIDE_DOWN;
            break;

        case CST816S_GESTURE_SLIDE_LEFT:
            drv_data->gesture = SLIDE_LEFT;
            break;

        case CST816S_GESTURE_SLIDE_RIGHT:
            drv_data->gesture = SLIDE_RIGHT;
            break;

        case CST816S_GESTURE_CLICK:
            drv_data->gesture = CLICK;
            break;

        case CST816S_GESTURE_DOUBLE_CLICK:
            drv_data->gesture = DOUBLE_CLICK;
            break;

        case CST816S_GESTURE_LONG_PRESS:
            drv_data->gesture = LONG_PRESS;
            break;
    }

//    drv_data->number_touch_point = buf[CST816S_REG_FINGER_NUM];

    msb = buf[CST816S_REG_XPOS_H] & 0x0f;
    lsb = buf[CST816S_REG_XPOS_L];
//    drv_data->touch_point_1.x = (msb<<8)|lsb;
    x = (msb << 8) | lsb;

    msb = buf[CST816S_REG_YPOS_H] & 0x0f;
    lsb = buf[CST816S_REG_YPOS_L];
//    drv_data->touch_point_1.y = (msb<<8)|lsb;
    y = (msb << 8) | lsb;

//    drv_data->action = (enum cst816s_action)(buf[CST816S_REG_XPOS_H] >> 6); 




    pressed = true;

    if (data->callback)
        data->callback(dev, x, y, pressed);

    return 0;
}


static void cst816s_work_handler(struct k_work *work)
{
	struct cst816s_data *data = CONTAINER_OF(work, struct cst816s_data, work);

	cst816s_process(data->dev);
}

#ifdef CONFIG_KSCAN_CST816S_INTERRUPT
static void cst816s_isr_handler(const struct device *dev,
			       struct gpio_callback *cb, uint32_t pins)
{
	struct cst816s_data *data = CONTAINER_OF(cb, struct cst816s_data, int_gpio_cb);

	k_work_submit(&data->work);
}
#else
static void cst816s_timer_handler(struct k_timer *timer)
{
	struct cst816s_data *data = CONTAINER_OF(timer, struct cst816s_data, timer);

	k_work_submit(&data->work);
}
#endif

static int cst816s_configure(const struct device *dev,
			    kscan_callback_t callback)
{
	struct cst816s_data *data = dev->data;

	if (!callback) {
		LOG_ERR("Invalid callback (NULL)");
		return -EINVAL;
	}

	data->callback = callback;

	return 0;
}

static int cst816s_enable_callback(const struct device *dev)
{
	struct cst816s_data *data = dev->data;

#ifdef CONFIG_KSCAN_CST816S_INTERRUPT
	gpio_add_callback(data->int_gpio, &data->int_gpio_cb);
#else
	k_timer_start(&data->timer, K_MSEC(CONFIG_KSCAN_FT5336_PERIOD),
		      K_MSEC(CONFIG_KSCAN_FT5336_PERIOD));
#endif

	return 0;
}

static int cst816s_disable_callback(const struct device *dev)
{
	struct cst816s_data *data = dev->data;

#ifdef CONFIG_KSCAN_CST816S_INTERRUPT
	gpio_remove_callback(data->int_gpio, &data->int_gpio_cb);
#else
	k_timer_stop(&data->timer);
#endif

	return 0;
}

static int cst816s_init(const struct device *dev)
{
	struct cst816s_data *data = dev->data;

	data->i2c = device_get_binding(DT_INST_BUS_LABEL(0));
	if (!data->i2c) {
		LOG_ERR("Could not find I2C controller:%s", DT_INST_BUS_LABEL(0));
		return -ENODEV;
	}

	data->dev = dev;

	k_work_init(&data->work, cst816s_work_handler);

	int r;

	data->reset_gpio = device_get_binding(DT_INST_GPIO_LABEL(0, reset_gpios));
	if (!data->reset_gpio) {
		LOG_ERR("Could not find reset GPIO controller");
		return -ENODEV;
	}
	r = gpio_pin_configure(data->reset_gpio, RESET_PIN,
			       DT_INST_GPIO_FLAGS(0, reset_gpios) | GPIO_OUTPUT);
	if (r < 0) {
		LOG_ERR("Could not configure reset GPIO pin");
		return r;
	}

//        cst816s_chip_init(dev);

#ifdef CONFIG_KSCAN_CST816S_INTERRUPT
	data->int_gpio = device_get_binding(DT_INST_GPIO_LABEL(0, reset_gpios));
	if (!data->int_gpio) {
		LOG_ERR("Could not find interrupt GPIO controller");
		return -ENODEV;
	}

	r = gpio_pin_configure(data->int_gpio, INTERRUPT_PIN,
                               DT_INST_GPIO_FLAGS(0, int_gpios) | GPIO_INPUT);
	if (r < 0) {
		LOG_ERR("Could not configure interrupt GPIO pin");
		return r;
	}
/*
	r = gpio_pin_interrupt_configure(data->int_gpio, config->int_gpio.pin,
					 GPIO_INT_EDGE_TO_ACTIVE);
	if (r < 0) {
		LOG_ERR("Could not configure interrupt GPIO interrupt.");
		return r;
	}
*/
	gpio_init_callback(&data->int_gpio_cb, cst816s_isr_handler,
			   BIT(INTERRUPT_PIN));
#else
	k_timer_init(&data->timer, cst816s_timer_handler, NULL);
#endif

	return 0;
}

static const struct kscan_driver_api cst816s_driver_api = {
	.config = cst816s_configure,
	.enable_callback = cst816s_enable_callback,
	.disable_callback = cst816s_disable_callback,
};

static struct cst816s_data cst816s_data;

DEVICE_AND_API_INIT(cst816s, DT_INST_LABEL(0), cst816s_init, &cst816s_data,
        NULL, POST_KERNEL, CONFIG_KSCAN_INIT_PRIORITY,
        &cst816s_driver_api);
