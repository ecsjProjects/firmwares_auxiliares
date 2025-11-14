#include <assert.h>
// #include <stdatomic.h>
// #include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "uart_protocol.h"

// #include "sdkconfig.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/semphr.h"
// #include "esp_timer.h"
// #include "esp_lcd_panel_ops.h"
// #include "esp_lcd_panel_rgb.h"
// // #include "driver/gpio.h"
// #include "esp_err.h"
// #include "esp_log.h"
// // #include "lvgl.h"
// #include "esp_debug_helpers.h"

// // Assuming uart_protocol.h defines op_code_t and other necessary types
// #include "uart_protocol.h"

// CRC-16-CCITT polynomial (X^16 + X^12 + X^5 + 1)
#define CRC16_POLY 0x1021
#define MAX_FRAME_SIZE 256
#define DLE 0x10
#define STX 0x02
#define ETX 0x03

// State machine states for RX
typedef enum
{
    RX_STATE_IDLE,
    RX_STATE_AFTER_DLE,
    RX_STATE_IN_FRAME,
    RX_STATE_ESCAPE
} rx_state_t;

static rx_state_t rx_state = RX_STATE_IDLE;
static uint8_t rx_buffer[MAX_FRAME_SIZE];
static size_t rx_index = 0;

// typedef enum {
//     // The command was successfully received.
//     OP_CODE_ACK,

//     // The command was not successfully received (wrong CRC, wrong length,
//     // invalid command, invalid payload, etc.)
//     OP_CODE_NACK,

//     // Check if the device is alive. The payload is empty and the response
//     // is an ACK.
//     OP_CODE_IS_ALIVE,

//     // Define when the screen should be redraw. The payload is a single byte
//     // with the configuration:
//     // - 0: don't redraw after each command.
//     // - 1: always redraw after each command.
//     // - 2: redraw now.
//     OP_CODE_SET_REDRAW,

//     // Set a pre-defined screen configuration.
//     // - byte 0: the index of the pre-defined screen (see `set_screen_t`).
//     OP_CODE_SET_SCREEN,

//     // Set the text of one element. The payload is:
//     // - byte 0: text data point (see `dp_text_t`).
//     // - bytes 1-N: the text to be displayed
//     OP_CODE_SET_TEXT,

//     // Change one icon. The payload is:
//     // - byte 0: the icon data point (see `dp_icon_t`).
//     // - byte 1: is the icon state (0 hidden, 1 on, 2 off, 3 over)
//     // - byte 2 (optional): the icon image (0-8)
//     OP_CODE_SET_ICON,

//     // Set same range of text to blink. The payload is:
//     // - byte 0: text data point (see `dp_text_t`).
//     // - bytes 1: index of the first character to blink
//     // - bytes 2: index of the end of the blinking range (exclusive)
//     OP_CODE_SET_TEXT_BLINK,

//     _OP_CODE_MAX,
// } op_code_t;

// typedef struct {
//     // The length of the payload, not including the `op_code` and other fields.
//     uint16_t payload_len;
//     // The sequence number of the command. A response will contain the same
//     // sequence number as the command. Sequences numbers from 0 to 127 are
//     // reserved for the main board, and from 128 to 255 are reserved for the
//     // peripheral.
//     uint8_t seq_num;
//     // The operation code of the command. This is a single byte that indicates
//     // the type of command being sent. The operation code is used to identify
//     // the command and determine how to process it.
//     op_code_t op_code;
//     // The payload of the command. The content is interpreted based on the
//     // operation code.
//     uint8_t *payload;
//     // The validity state of the command.
//     validity_t validity;
// } command_t;

// User-defined function to handle a valid received packet.
// This should be implemented based on the specific application.
// For example, it could dispatch based on op_code and seq_num.
void handle_received_packet(uint8_t seq_num, op_code_t op_code, const uint8_t *payload, uint16_t payload_len);
bool data_to_command(uint8_t *data, size_t len, command_t *command);
void receive_commands_task(void *arg);

//--------------------------------------------------------------------------------------------------------------------
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
}

void loop()
{
    // put your main code here, to run repeatedly:
    
    // uart_rx_handler(Serial.read());
    receive_commands_task(NULL);

            
}
//--------------------------------------------------------------------------------------------------------------------

// This function is already provided in the code snippet
uint16_t calculate_crc16(uint16_t start, const uint8_t *data, size_t length)
{
    uint16_t crc = start;
    for (size_t i = 0; i < length; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++)
        {
            crc = (crc & 0x8000) ? ((crc << 1) ^ CRC16_POLY) : (crc << 1);
        }
    }
    return crc;
}

// // Process the destuffed packet
// static void process_packet(const uint8_t *data, size_t len)
// {
//     if (len < 6)
//     { // Minimum: LEN16(2) + SEQ(1) + OP(1) + CRC16(2)
//         // LOG_ERROR("Packet too short: %zu bytes", len);
//         return;
//     }

//     uint16_t payload_len = ((uint16_t)data[0] << 8) | data[1];
//     if (len != (size_t)6 + payload_len)
//     {
//         // LOG_ERROR("Invalid packet length: expected %zu, got %zu", 6 + payload_len, len);
//         return;
//     }

//     uint8_t seq_num = data[2];
//     op_code_t op_code = (op_code_t)data[3];
//     const uint8_t *payload = data + 4;
//     uint16_t received_crc = ((uint16_t)data[4 + payload_len] << 8) | data[5 + payload_len];

//     // Calculate CRC over LEN16 + SEQ + OP + PAYLOAD
//     uint16_t calc_crc = 0xFFFF;
//     calc_crc = calculate_crc16(calc_crc, data, 4 + payload_len);

//     if (calc_crc != received_crc)
//     {
//         // LOG_ERROR("CRC mismatch: calculated 0x%04X, received 0x%04X", calc_crc, received_crc);
//         return;
//     }

//     // Packet is valid, handle it
//     // LOG_DEBUG("Valid packet received: SEQ=%u, OP=0x%02X, PAYLOAD_LEN=%u", seq_num, op_code, payload_len);
//     handle_received_packet(seq_num, op_code, payload, payload_len);
// }

// // UART RX handler: call this for each incoming byte
// void uart_rx_handler(uint8_t byte)
// {
//     switch (rx_state)
//     {
//     case RX_STATE_IDLE:
//         if (byte == DLE)
//         {
//             rx_state = RX_STATE_AFTER_DLE;
//         }
//         // Else ignore garbage bytes
//         break;

//     case RX_STATE_AFTER_DLE:
//         if (byte == STX)
//         {
//             rx_state = RX_STATE_IN_FRAME;
//             rx_index = 0;
//         }
//         else
//         {
//             // Unexpected, reset to idle
//             // If byte == DLE, it could chain, but for simplicity, reset
//             rx_state = RX_STATE_IDLE;
//             if (byte == DLE)
//             {
//                 rx_state = RX_STATE_AFTER_DLE; // Allow chaining if next is STX
//             }
//         }
//         break;

//     case RX_STATE_IN_FRAME:
//         if (byte == DLE)
//         {
//             rx_state = RX_STATE_ESCAPE;
//         }
//         else
//         {
//             if (rx_index < MAX_FRAME_SIZE)
//             {
//                 rx_buffer[rx_index++] = byte;
//             }
//             else
//             {
//                 // Buffer overflow, reset
//                 // LOG_ERROR("RX buffer overflow");
//                 rx_state = RX_STATE_IDLE;
//             }
//         }
//         break;

//     case RX_STATE_ESCAPE:
//         if (byte == DLE)
//         {
//             // Escaped DLE
//             if (rx_index < MAX_FRAME_SIZE)
//             {
//                 rx_buffer[rx_index++] = DLE;
//             }
//             else
//             {
//                 // Buffer overflow, reset
//                 // LOG_ERROR("RX buffer overflow in escape");
//                 rx_state = RX_STATE_IDLE;
//                 return;
//             }
//             rx_state = RX_STATE_IN_FRAME;
//         }
//         else if (byte == ETX)
//         {
//             // End of frame
//             process_packet(rx_buffer, rx_index);
//             rx_state = RX_STATE_IDLE;
//         }
//         else
//         {
//             // Invalid escape sequence, reset
//             // LOG_ERROR("Invalid escape byte: 0x%02X", byte);
//             rx_state = RX_STATE_IDLE;
//         }
//         break;
//     }
// }

// // Example stub for handle_received_packet - implement as needed
// void handle_received_packet(uint8_t seq_num, op_code_t op_code, const uint8_t *payload, uint16_t payload_len)
// {
//     // TODO: Implement packet handling logic here
//     // For example:
//     // switch (op_code) {
//     //     case OP_CODE_SET_SCREEN:
//     //         // Process payload[0] as screen_index, etc.

//     //         break;
//     //     // ...
//     // }
//     // If needed, send response using send_command or similar
//     Serial.println("CRC inválido!");
// }

#define UART_NUM UART_NUM_1
#define UART_NUM UART_NUM_1
#define UART_BUF_SIZE 256
#define CMD_BUF_SIZE 256
#define CIRCULAR_BUF_SIZE 1024


// QueueHandle_t cmd_queue = NULL;

void receive_commands_task(void *arg) {
    // Configure UART parameters

    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, PIN_UART_TX, PIN_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // ESP_LOGI(TAG, "UART driver installed");

    static uint8_t buffer[256];
    size_t index = 0;
    bool escape = false;
    bool start_found = false;

    // uart_write_bytes(UART_NUM, "UART RX started\n", 16);

    while (1) {
        uart_read_bytes(UART_NUM, &buffer[index], 1, portMAX_DELAY);
       buffer[index] = Serial.read();
        if (!escape && buffer[index] == DLE) {
            escape = true;
            continue;
        }

        if (escape) {
            escape = false;

            if (buffer[index] == STX) {
                if (start_found) {
                    // Previous packet was not finished
                    ESP_LOGW(TAG, "Invalid packet");
                }
                // packet started, stuffed content start at index=0
                index = 0;
                start_found = true;
                continue;
            }

            // packet end
            if (buffer[index] == ETX) {
                if (start_found) {
                    // Complete packet received
                    unstuff_data_in_place(buffer, sizeof(buffer), 0, index, &index);
                    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, index, ESP_LOG_INFO);
                    command_t cmd;
                    data_to_command(buffer, index, &cmd);

                    // Move payload to a dynamically allocated memory
                    if (cmd.payload_len > 0) {
                        uint8_t *payload = (uint8_t *)malloc(cmd.payload_len);
                        memcpy(payload, cmd.payload, cmd.payload_len);
                        cmd.payload = payload;
                    } else {
                        cmd.payload = NULL;
                    }

                    // Send the command to the queue
                    xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
                } else {
                    // Invalid packet
                    ESP_LOGW(TAG, "Invalid packet");
                    index = 0;
                }
                index = 0;
                start_found = false;
                continue;
            }
        }

        if (start_found)
            index++;
    }
}



bool data_to_command(uint8_t *data, size_t len, command_t *command) {
    assert(command != NULL);

    int i = 0;
    command->payload_len = data[i] << 8 | data[i + 1];
    i += 2;
    command->seq_num = data[i++];
    command->op_code = data[i++];
    command->payload = &data[i];

    if (len != 4 + (size_t)command->payload_len + 2) {
        // LOG_WARN("invalid length: %d != %d\n", (int)len, (int)(4 + (size_t)command->payload_len + 2));
        command->validity = VALIDITY_INVALID_LENGTH;
        return true;
    }

    i += command->payload_len;
    uint16_t data_crc16 = data[i] << 8 | data[i + 1];

    uint16_t crc16 = 0xFFFF;
    crc16 = calculate_crc16(crc16, data, len - 2);
    command->validity = (crc16 == data_crc16) ? VALIDITY_VALID : VALIDITY_INVALID_CRC;
    if (command->validity == VALIDITY_INVALID_CRC) {
        // LOG_WARN("invalid CRC: exp %04x, fnd %04x\n", crc16, data_crc16);
    }

    return true;
}
