#pragma once

#include <Arduino.h>
#include <Print.h>
#include "./Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define GC9D01N_TFTWIDTH 160
#define GC9D01N_TFTHEIGHT 160

#define GC9D01N_RST_DELAY 800    ///< delay ms wait for reset finish
#define GC9D01N_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define GC9D01N_SLPOUT_DELAY 200 ///< delay ms wait for sleep out finish

#define GC9D01N_NOP 0x00
#define GC9D01N_SWRESET 0x01
#define GC9D01N_RDDID 0x04
#define GC9D01N_RDDST 0x09

#define GC9D01N_SLPIN 0x10
#define GC9D01N_SLPOUT 0x11
#define GC9D01N_PTLON 0x12
#define GC9D01N_NORON 0x13

#define GC9D01N_INVOFF 0x20
#define GC9D01N_INVON 0x21
#define GC9D01N_DISPOFF 0x28
#define GC9D01N_DISPON 0x29

#define GC9D01N_CASET 0x2A
#define GC9D01N_RASET 0x2B
#define GC9D01N_RAMWR 0x2C
#define GC9D01N_RAMRD 0x2E

#define GC9D01N_PTLAR 0x30
#define GC9D01N_COLMOD 0x3A
#define GC9D01N_MADCTL 0x36

#define GC9D01N_MADCTL_MY 0x80
#define GC9D01N_MADCTL_MX 0x40
#define GC9D01N_MADCTL_MV 0x20
#define GC9D01N_MADCTL_ML 0x10
#define GC9D01N_MADCTL_BGR 0x08
#define GC9D01N_MADCTL_MH 0x04
#define GC9D01N_MADCTL_RGB 0x00

#define GC9D01N_RDID1 0xDA
#define GC9D01N_RDID2 0xDB
#define GC9D01N_RDID3 0xDC
#define GC9D01N_RDID4 0xDD

static const uint8_t gc9d01n_init_operations[] = {
    BEGIN_WRITE,

    WRITE_COMMAND_8, 0xFE,
    WRITE_COMMAND_8, 0xEF,

    WRITE_C8_D8, 0x80, 0xFF,

    WRITE_C8_D8, 0x81, 0xFF,

    WRITE_C8_D8, 0x82, 0xFF,

    WRITE_C8_D8, 0x83, 0xFF,

    WRITE_C8_D8, 0x84, 0xFF,

    WRITE_C8_D8, 0x85, 0xFF,

    WRITE_C8_D8, 0x86, 0xFF,

    WRITE_C8_D8, 0x87, 0xFF,

    WRITE_C8_D8, 0x88, 0xFF,

    WRITE_C8_D8, 0x89, 0xFF,

    WRITE_C8_D8, 0x8A, 0xFF,

    WRITE_C8_D8, 0x8B, 0xFF,

    WRITE_C8_D8, 0x8C, 0xFF,

    WRITE_C8_D8, 0x8D, 0xFF,

    WRITE_C8_D8, 0x8E, 0xFF,

    WRITE_C8_D8, 0x8F, 0xFF,

    WRITE_C8_D8, 0x3A, 0x05,

    WRITE_C8_D8, 0xEC, 0x01,

    WRITE_COMMAND_8, 0x74,
    WRITE_BYTES, 7,
    0x02, 0x0E, 0x00, 0x00,
    0x00, 0x00, 0x00,

    WRITE_C8_D8, 0x98, 0x3E,

    WRITE_C8_D8, 0x99, 0x3E,

    WRITE_COMMAND_8, 0xB5,
    WRITE_BYTES, 2,
    0x0D, 0x0D,

    WRITE_COMMAND_8, 0x60,
    WRITE_BYTES, 4,
    0x38, 0x0F, 0x79, 0x67,

    WRITE_COMMAND_8, 0x61,
    WRITE_BYTES, 4,
    0x38, 0x11, 0x79, 0x67,

    WRITE_COMMAND_8, 0x64,
    WRITE_BYTES, 6,
    0x38, 0x17, 0x71, 0x5F,
    0x79, 0x67,

    WRITE_COMMAND_8, 0x65,
    WRITE_BYTES, 6,
    0x38, 0x13, 0x71, 0x5B,
    0x79, 0x67,

    WRITE_COMMAND_8, 0x6A,
    WRITE_BYTES, 2,
    0x00, 0x00,

    WRITE_COMMAND_8, 0x6C,
    WRITE_BYTES, 7,
    0x22, 0x02, 0x22, 0x02,
    0x22, 0x22, 0x50,

    WRITE_COMMAND_8, 0x6E,
    WRITE_BYTES, 32,
    0x03, 0x03, 0x01, 0x01,
    0x00, 0x00, 0x0F, 0x0F,
    0x0D, 0x0D, 0x0B, 0x0B,
    0x09, 0x09, 0x00, 0x00,
    0x00, 0x00, 0x0A, 0x0A,
    0x0C, 0x0C, 0x0E, 0x0E,
    0x10, 0x10, 0x00, 0x00,
    0x02, 0x02, 0x04, 0x04,

    WRITE_C8_D8, 0xBF, 0x01,

    WRITE_C8_D8, 0xF9, 0x40,

    WRITE_COMMAND_8, 0x9B,
    WRITE_BYTES, 5,
    0x3B, 0x93, 0x33, 0x7F,
    0x00,

    WRITE_C8_D8, 0x7E, 0x30,

    WRITE_COMMAND_8, 0x70,
    WRITE_BYTES, 6,
    0x0D, 0x02, 0x08, 0x0D,
    0x02, 0x08,

    WRITE_COMMAND_8, 0x71,
    WRITE_BYTES, 3,
    0x0D, 0x02, 0x08,

    WRITE_COMMAND_8, 0x91,
    WRITE_BYTES, 2,
    0x0E, 0x09,

    WRITE_COMMAND_8, 0xC3,
    WRITE_BYTES, 5,
    0x19, 0xC4, 0x19, 0xC9,
    0x3C,

    WRITE_COMMAND_8, 0xF0, // 设置伽马值1
    WRITE_BYTES, 6,
    0x53, 0x15, 0x0A, 0x04,
    0x00, 0x3E,

    WRITE_COMMAND_8, 0xF1, // 设置伽马值2
    WRITE_BYTES, 6,
    0x56, 0xA8, 0x7F, 0x33,
    0x34, 0x5F,

    WRITE_COMMAND_8, 0xF2, // 设置伽马值3
    WRITE_BYTES, 6,
    0x53, 0x15, 0x0A, 0x04,
    0x00, 0x3A,

    WRITE_COMMAND_8, 0xF3, // 设置伽马值4
    WRITE_BYTES, 6,
    0x52, 0xA4, 0x7F, 0x33,
    0x34, 0xDF,

    WRITE_C8_D8, 0x36, 0x00,

    WRITE_COMMAND_8, GC9D01N_SLPOUT,
    END_WRITE,

    DELAY, GC9D01N_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, GC9D01N_DISPON, // Display on
    WRITE_COMMAND_8, GC9D01N_RAMWR,
    END_WRITE,

    DELAY, 20};

class Arduino_GC9D01N : public Arduino_TFT
{
public:
    Arduino_GC9D01N(
        Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
        bool ips = false, int16_t w = GC9D01N_TFTWIDTH, int16_t h = GC9D01N_TFTHEIGHT,
        uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0);

    bool begin(int32_t speed = GFX_NOT_DEFINED) override;
    void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;
    void setRotation(uint8_t r) override;
    void invertDisplay(bool) override;
    void displayOn() override;
    void displayOff() override;

protected:
    void tftInit() override;

private:
};
