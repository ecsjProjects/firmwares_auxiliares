#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#define DLE 0x10
#define STX 0x02
#define ETX 0x03

// Maximum length of text labels, not including the null terminator
#define MAX_TEXT_LEN 31

typedef enum {
    // The command was successfully received.
    OP_CODE_ACK,

    // The command was not successfully received (wrong CRC, wrong length,
    // invalid command, invalid payload, etc.)
    OP_CODE_NACK,

    // Check if the device is alive. The payload is empty and the response
    // is an ACK.
    OP_CODE_IS_ALIVE,

    // Define when the screen should be redraw. The payload is a single byte
    // with the configuration:
    // - 0: don't redraw after each command.
    // - 1: always redraw after each command.
    // - 2: redraw now.
    OP_CODE_SET_REDRAW,

    // Set a pre-defined screen configuration.
    // - byte 0: the index of the pre-defined screen (see `set_screen_t`).
    OP_CODE_SET_SCREEN,

    // Set the text of one element. The payload is:
    // - byte 0: text data point (see `dp_text_t`).
    // - bytes 1-N: the text to be displayed
    OP_CODE_SET_TEXT,

    // Change one icon. The payload is:
    // - byte 0: the icon data point (see `dp_icon_t`).
    // - byte 1: is the icon state (0 hidden, 1 on, 2 off, 3 over)
    // - byte 2 (optional): the icon image (0-8)
    OP_CODE_SET_ICON,

    // Set same range of text to blink. The payload is:
    // - byte 0: text data point (see `dp_text_t`).
    // - bytes 1: index of the first character to blink
    // - bytes 2: index of the end of the blinking range (exclusive)
    OP_CODE_SET_TEXT_BLINK,

    _OP_CODE_MAX,
} op_code_t;

typedef enum {
    VALIDITY_VALID = 0x00,
    // The CRC16 checksum of the received data is invalid.
    VALIDITY_INVALID_CRC = 0x01,
    // The length of the received data is invalid (too short or too long)
    VALIDITY_INVALID_LENGTH = 0x02,
    // The command is not valid (for any other reason)
    VALIDITY_INVALID = 0x03,
} validity_t;

typedef enum {
    SET_REDRAW_DONT = 0x00,
    SET_REDRAW_ALWAYS = 0x01,
    SET_REDRAW_NOW = 0x02,
    SET_REDRAW__MAX,
} set_redraw_t;

typedef enum {
    DP_SCREEN_EMPTY = 0,
    DP_SCREEN_INITIAL = 1,
    DP_SCREEN_PRINCIPAL,
    DP_SCREEN_SHOW_TIMER,
    DP_SCREEN_SETTING_DATE,
    DP_SCREEN_CHANNELS_SET,
    DP_SCREEN_SETTING_MIC_IN_SET_1,
    DP_SCREEN_SETTING_MIC_IN_SET_2,
    DP_SCREEN_SETTING_LINE_IN_SET_1,
    DP_SCREEN_SETTING_LINE_IN_SET_2,
    DP_SCREEN_SETTING_USB_IN_SET_1,
    DP_SCREEN_SETTING_USB_IN_SET_2,
    DP_SCREEN_SETTING_BLUETOOTH_IN_SET_1,
    DP_SCREEN_SETTING_BLUETOOTH_IN_SET_2,
    DP_SCREEN_SETTING_TELCO_IN_SET_1,
    DP_SCREEN_SETTING_TELCO_IN_SET_2,
    DP_SCREEN__MAX,
} dp_screen_t;

typedef enum {
    ICON_STATE_HIDDEN = 0x00,
    ICON_STATE_ON,
    ICON_STATE_OFF,
    ICON_STATE_OVER,
    ICON_STATE__MAX,
} icon_state_t;

typedef enum {
    DP_IMAGE_HOME = 1,
    DP_IMAGE_SETTING,
    DP_IMAGE_CLOCK,
    DP_IMAGE_BLUETOOTH,
    DP_IMAGE_PHONE,
    DP_IMAGE_USB,
    DP_IMAGE_LINE,
    DP_IMAGE_MIC,
    DP_IMAGE_BIG_LOGO,
    DP_IMAGE_SMALL_LOGO,
    DP_IMAGE__MAX,
} dp_image_t;

typedef enum {
    DP_TEXT_TOP = 0,
    DP_TEXT_CENTER,
    DP_TEXT_BOTTOM,

    DP_TEXT_LINE_0,
    DP_TEXT_LINE_1,
    DP_TEXT_LINE_2,
    DP_TEXT_LINE_3,
    DP_TEXT_LINE__LAST = DP_TEXT_LINE_3,

    DP_TEXT_ICON_0,
    DP_TEXT_ICON_1,
    DP_TEXT_ICON_2,
    DP_TEXT_ICON_3,
    DP_TEXT_ICON_4,
    DP_TEXT_ICON_5,
    DP_TEXT_ICON_6,
    DP_TEXT_ICON_7,
    DP_TEXT_ICON_8,
    DP_TEXT_ICON_9,
    DP_TEXT_ICON_10,
    DP_TEXT_ICON_11,
    DP_TEXT_ICON_12,
    DP_TEXT_ICON_13,
    DP_TEXT_ICON_14,
    DP_TEXT_ICON_15,
    DP_TEXT_ICON_16,
    DP_TEXT_ICON_17,
    DP_TEXT_ICON__LAST = DP_TEXT_ICON_17,

    DP_TEXT__MAX,
} dp_text_t;

typedef enum {
    DP_ICON_BOTTOM_0 = 0,
    DP_ICON_BOTTOM_1,
    DP_ICON_BOTTOM_2,
    DP_ICON_BOTTOM__LAST = DP_ICON_BOTTOM_2,

    DP_ICON_CENTER_0,
    DP_ICON_CENTER_1,
    DP_ICON_CENTER_2,
    DP_ICON_CENTER_3,
    DP_ICON_CENTER_4,
    DP_ICON_CENTER_5,
    DP_ICON_CENTER_6,
    DP_ICON_CENTER_7,
    DP_ICON_CENTER_8,
    DP_ICON_CENTER_9,
    DP_ICON_CENTER_10,
    DP_ICON_CENTER_11,
    DP_ICON_CENTER_12,
    DP_ICON_CENTER_13,
    DP_ICON_CENTER_14,
    DP_ICON_CENTER_15,
    DP_ICON_CENTER_16,
    DP_ICON_CENTER_17,
    DP_ICON_CENTER__LAST = DP_ICON_CENTER_17,

    DP_ICON_BIG_LOGO,
    DP_ICON_SMALL_LOGO,

    DP_ICON__MAX,
} dp_icon_t;

// static_assert(DP_ICON_CENTER__LAST - DP_ICON_CENTER_0 == DP_TEXT_ICON__LAST - DP_TEXT_ICON_0,
//               "The number of icons and texts must be the same");

typedef struct {
    // The length of the payload, not including the `op_code` and other fields.
    uint16_t payload_len;
    // The sequence number of the command. A response will contain the same
    // sequence number as the command. Sequences numbers from 0 to 127 are
    // reserved for the main board, and from 128 to 255 are reserved for the
    // peripheral.
    uint8_t seq_num;
    // The operation code of the command. This is a single byte that indicates
    // the type of command being sent. The operation code is used to identify
    // the command and determine how to process it.
    op_code_t op_code;
    // The payload of the command. The content is interpreted based on the
    // operation code.
    uint8_t *payload;
    // The validity state of the command.
    validity_t validity;
} command_t;

typedef void (*on_decoded_frame_callback_t)(void *user_data, uint8_t *data, size_t len,
                                            size_t start, size_t end);

// TODO: maybe extract the global state of the decoder into a struct?

// Calculates the CRC16 checksum of the given data.
uint16_t calculate_crc16(uint16_t start, const uint8_t *data, size_t length);

// Encode a command to be sent over UART in the following format:
//
// [DLE] [STX] [Stuffed Data] [DLE] [ETX]
// [Data] -> [Payload Length: u16] [Seq Num: u8] [Op Code: u8] [Payload: ...] [CRC16: u16le]
//
// The encoded data is stored in `out_buffer`, and the length of the encoded
// data is stored in `out_buffer_len`. The length of the passed buffer must be
// passed in `out_buffer_len`. The function returns 0 on success, and non-zero
// on failure.
int encode_frame(uint8_t seq_num, op_code_t op_code, uint8_t *payload, uint16_t payload_len,
                 uint8_t *out_buffer, uint16_t *out_buffer_len);

// Same as `encode_frame`, but encodes the data in the same buffer as the
// payload. The buffer under `payload` must be large enough to hold the encoded
// data, and is passed in `out_buffer_len`.
//
// The encoded data is stored in `payload`, and the length of the encoded data
// is stored in `out_buffer_len`. The function returns 0 on success, and
// non-zero on failure.
int encode_frame_in_place(uint8_t seq_num, op_code_t op_code, uint8_t *payload,
                          uint16_t payload_len, uint16_t *out_buffer_len);

// Decode a data frame received from the main board, through UART. This
// function is stateful, and will decode the frame from segments of the data
// received, throughout multiple calls.
//
// `data` is a circular buffer with size `len`, and `start` (inclusive) and
// `end` (exclusive) are the indices of the start and end of the data to be
// processed.
//
// The frame format is:
// [DLE] [STX] [Stuffed Data] [DLE] [ETX]
// [Data] -> [Payload Length: u16] [Op Code: u8] [Payload: ...] [CRC16]
//
// On a successful decode, the function will save the command in an global
// buffer. This buffer can be accessed with `get_command`.
void decode_frame(uint8_t *data, size_t len, size_t start, size_t end,
                  on_decoded_frame_callback_t callback, void *user_data);

// Reads data contained between `start` (inclusive) and `end` (exclusive)
// indexes of the `input` circular buffer of size `len`, remove its escaping
// `DLE` bytes ("unstuff") and write the unstuffed data to `output`. The length
// of the unstuff data is store in `*length`. Returns true if the data was
// successfully unstuffed, and false otherwise.
bool unstuff_data(const uint8_t *input, size_t len, size_t start, size_t end, uint8_t *output,
                  size_t *length);

// Modifies the data contained between `start` (inclusive) and `end`
// (exclusive) indexes of the `input` circular buffer of size `len`, removing
// its escaping `DLE` bytes ("unstuff"). The length of the unstuff data is
// store in `*length` (the end of the stuffed data will be at `(start +
// *length) % len`). Returns true if the data was successfully unstuffed, and
// false otherwise.
bool unstuff_data_in_place(uint8_t *data, size_t len, size_t start, size_t end, size_t *length);

// Convert a bytes to command. The command is stored in the `command_t` struct.
// This struct holds a pointer to the payload, which is stored in `data`.
//
// The function returns true if the command was successfully decoded, and false
// otherwise.
//
// This also checks the validity of the command, and sets the `validity` field
// of the command.
bool data_to_command(uint8_t *data, size_t len, command_t *command);
