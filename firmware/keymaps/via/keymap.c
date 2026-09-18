#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include <string.h>


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_MUTE
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) }
};
#endif

// --- Raw HID <-> OLED framebuffer bridge ---
// The companion app (written later) owns ALL rendering (text, icons, whatever).
// It rasterizes its own 128x32 monochrome image and sends the raw pixel bytes
// here; firmware just paints whatever bytes it's given. No font/icon logic
// lives in firmware at all.
//
// Frame size: 128 * 32 / 8 = 512 bytes total.
// HID packet payload: 31 usable bytes (byte 0 is a packet-index header).
// So a full frame = ceil(512 / 31) = 17 packets.
//
// Packet format:
//   byte 0      : packet index (0-16). Index 0xFF instead means "buffer ready,
//                 flip it to screen now" (an explicit "frame done" signal, so
//                 the companion app can send frames in any order/timing and
//                 firmware doesn't guess when a frame is complete).
//   bytes 1-31  : raw framebuffer bytes for this chunk (31 bytes/packet)

#define FB_SIZE 512
#define CHUNK_SIZE 31
#define TOTAL_CHUNKS 17  // ceil(512/31)
#define FRAME_DONE_MARKER 0xFF

static uint8_t framebuffer[FB_SIZE];
static bool frame_ready = false;

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (length < 1) return;

    uint8_t cmd = data[0];

    if (cmd == FRAME_DONE_MARKER) {
        frame_ready = true;
        memset(data, 0, length);
        data[0] = 0x01; // ack: frame flipped
        return;
    }

    uint8_t index = cmd;
    // Support offset indexing (0x80 + index) to avoid VIA command ID collisions
    if (index >= 0x80 && index < 0x80 + TOTAL_CHUNKS) {
        index -= 0x80;
    }

    if (index >= TOTAL_CHUNKS) {
        data[0] = 0xFF; // unhandled
        return;
    }

    uint16_t offset = (uint16_t)index * CHUNK_SIZE;
    uint8_t copy_len = length - 1;
    if (offset + copy_len > FB_SIZE) {
        copy_len = FB_SIZE - offset; // clamp last chunk (512 isn't evenly divisible by 31)
    }
    memcpy(&framebuffer[offset], &data[1], copy_len);

    data[0] = 0x00; // ack: chunk received
}

#if defined(OLED_ENABLE)
bool oled_task_user(void) {
    if (frame_ready) {
        oled_write_raw((const char *)framebuffer, FB_SIZE);
        frame_ready = false;
    }
    return false;
}
#endif
