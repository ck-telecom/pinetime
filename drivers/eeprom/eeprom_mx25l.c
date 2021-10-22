#define DT_DRV_COMPAT mxicy_cmos_mx25l

#include <drivers/eeprom.h>
#include <drivers/spi.h>
#include <logging/log.h>
LOG_MODULE_REGISTER(eeprom_mx25l);

struct eeprom_mx25l_config {
	struct spi_dt_spec spi;
};

static int eeprom_mx25l_read(const struct device *dev, off_t offset,
				void *buf,
				size_t len)
{
	return 0;
}

static int eeprom_mx25l_write(const struct device *dev, off_t offset,
				const void *buf, size_t len)
{
	return 0;
}

static size_t eeprom_mx25l_size(const struct device *dev)
{
	const struct eeprom_mx25l_config *config = dev->config;

	return config->size;
}

static int eeprom_mx25l_init(const struct device *dev)
{
	return 0;
}

static const struct eeprom_driver_api eeprom_mx25l_api = {
	.read = eeprom_mx25l_read,
	.write = eeprom_mx25l_write,
	.size = eeprom_mx25l_size,
};

#define EEPROM_AT25_BUS(n, t)						 \
	{ .spi = SPI_DT_SPEC_GET(INST_DT_AT2X(n, t),			 \
				 SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | \
				 SPI_WORD_SET(8), 0) }

static struct eeprom_mx25l_config mx25l_config = {
	.spi = SPI_DT_SPEC_GET(INST_DT)
};
//https://wenku.baidu.com/view/2aab4ffe48649b6648d7c1c708a1284ac850050c.html