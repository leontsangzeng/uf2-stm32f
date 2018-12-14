#include "bl.h"

#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/cortex.h>

#define BINDATA_SIZE (32 * 1024)

extern const uint8_t bindata[];

extern uint32_t vector_table[];

void flushFlash(void) {}

void delay(unsigned msec) {
    int k = msec * 15000;
    while (k--)
        asm("nop");
}

void flash_bootloader(void) {
    cm_disable_interrupts();

    flash_unlock();

    // erase bootloader
    flash_erase_sector(0, FLASH_CR_PROGRAM_X32);
    flash_erase_sector(1, FLASH_CR_PROGRAM_X32);

    uint32_t dst = 0x08000000;
    for (int i = 0; i < BINDATA_SIZE; i += 4) {
        flash_program_word(dst + i, *(uint32_t *)(bindata + i));
    }

    // self-destruct
    flash_program_word((uint32_t)vector_table, 0);
    flash_program_word((uint32_t)vector_table + 4, 0);

    screen_init();
    print(3, 3, 6, "Bootloader updated!");
    print4(3, 40, 4, "Yay:)");
    print(3, 100, 5, "Press reset");
    draw_screen();

    for (;;) {
        led_off(LED_BOOTLOADER);
        delay(300);
        led_on(LED_BOOTLOADER);
        delay(100);
    }

    // resetIntoBootloader();
}