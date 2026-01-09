#include <drivers/adc.h>
#include <logging/log.h>

#include <hal/nrf_saadc.h>

#define CHANNEL_ID 7
#define RESOLUTION 12
#define DIVIDER 2
#define COUNT_DOWN 5000
#define BATTERY_PEROID_TIME 5

LOG_MODULE_REGISTER(BATTERY, LOG_LEVEL_INF);

static const struct device* percentage_dev;

static const struct adc_channel_cfg m_1st_channel_cfg = {
    .gain             = ADC_GAIN_1_4,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40),
    .channel_id       = CHANNEL_ID,
    .differential     = 0,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
    .input_positive   = NRF_SAADC_INPUT_AIN7,
#endif
};

static struct k_timer timer;

static int battery_read_voltage(uint16_t *data, int data_len)
{
    int retval = 0;

    const struct adc_sequence sequence = {
        .channels    = BIT(CHANNEL_ID),
        .buffer      = data,
        .buffer_size = data_len,
        .resolution  = RESOLUTION,
    };
    retval = adc_read(percentage_dev, &sequence);

    return retval;
}

void battery_update_percentage(struct k_timer *timer)
{
    uint16_t data = 0;
    battery_read_voltage(&data, sizeof(data));
    LOG_INF("adc raw:0x%x", data);
    // send btn event
    // k_msg_put();
}

int batttery_init(const struct device *dev)
{
    int retval = 0;

    percentage_dev = device_get_binding("ADC_0");
    if (adc_channel_setup(percentage_dev, &m_1st_channel_cfg) < 0) {
        LOG_ERR("error setup adc channel");
    }
    k_timer_init(&timer, battery_update_percentage, NULL);
    k_timer_start(&timer, K_NO_WAIT, K_SECONDS(BATTERY_PEROID_TIME));

    return retval;
}
SYS_INIT(batttery_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
