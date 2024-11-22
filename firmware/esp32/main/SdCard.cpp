#include "SdCard.hpp"

#include <esp_log.h>
#include <esp_vfs_fat.h>

SdCard::SdCard()
{
    const sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    const spi_bus_config_t spi_bus_config {
        // configure pins per HSPI https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
        .mosi_io_num = GPIO_NUM_13,
        .miso_io_num = GPIO_NUM_12,
        .sclk_io_num = GPIO_NUM_14,
        .quadwp_io_num = -1, /* not used */
        .quadhd_io_num = -1, /* not used */
        .data4_io_num = -1,  /* not used */
        .data5_io_num = -1,  /* not used */
        .data6_io_num = -1,  /* not used */
        .data7_io_num = -1,  /* not used */
        .max_transfer_sz = 0, /* use default */
        .flags = 0, /* not used */
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_0,
        .intr_flags = 0 /* not used */
    };
    ESP_ERROR_CHECK(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot),
                                       &spi_bus_config, SPI_DMA_CH_AUTO));

    const sdspi_device_config_t slot_config = {
        .host_id = SDSPI_DEFAULT_HOST,
        .gpio_cs = SDSPI_SLOT_NO_CS,
        .gpio_cd = SDSPI_SLOT_NO_CD,
        .gpio_wp = SDSPI_SLOT_NO_WP,
        .gpio_int = SDSPI_SLOT_NO_INT,
        .gpio_wp_polarity = false /* not used */
    };
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 10, // FIXME 
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE, /* reformat parameter */
        .disk_status_check_enable = false,
        .use_one_fat= true  /* reformat parameter */
    };
    sdmmc_card_t* card;
    auto result = esp_vfs_fat_sdspi_mount(path, &host, &slot_config, &mount_config, &card);
    ESP_LOGD(TAG, "mount sdcard result [%x %s]", result, esp_err_to_name(result));

}