#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/uart.h>
#include <nfc_t2t_lib.h>
#include <nfc/ndef/msg.h>
#include <nfc/ndef/text_rec.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/sys/timeutil.h>

#define MAX_REC_COUNT        1
#define NDEF_MSG_BUF_SIZE    512    // Increased buffer size for larger payloads
#define NFC_FIELD_LED        DK_LED1

typedef enum {
    STATE_LOCKED,
    STATE_UNLOCKED
} system_state_t;

static system_state_t current_state = STATE_LOCKED; // Initial state
const struct device *uart_dev;
static uint8_t ndef_msg_buf[NDEF_MSG_BUF_SIZE]; // Buffer for NFC messages

/* Fixed geolocation coordinates */
const char *fixed_location = "42.089654, -75.970346";

/* Function to get current date and time (using Zephyr time API) */
void get_current_datetime(char *datetime_buf, size_t buf_size) {
    uint32_t now_ms = k_uptime_get();  // Get time in milliseconds
    struct tm timeinfo;
    time_t now = (time_t)(now_ms / 1000);  // Convert ms to seconds

    gmtime_r(&now, &timeinfo);  // Get the UTC time
    snprintf(datetime_buf, buf_size, "%02d/%02d/%04d %02d:%02d:%02d",
             timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year + 1900,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

/* Function to toggle state */
void toggle_state() {
    if (current_state == STATE_LOCKED) {
        current_state = STATE_UNLOCKED;
    } else {
        current_state = STATE_LOCKED;
    }
}

/* Function to send state, location, and timestamp information via UART */
void send_state_to_lora(system_state_t state, const char *datetime) {
    char msg[256];

    if (state == STATE_LOCKED) {
        snprintf(msg, sizeof(msg), "DATE: %s, STATE: LOCKED, LOCATION: %s\n", datetime, fixed_location);
    } else {
        snprintf(msg, sizeof(msg), "DATE: %s, STATE: UNLOCKED, LOCATION: %s\n", datetime, fixed_location);
    }

    for (size_t i = 0; msg[i] != '\0'; i++) {
        uart_poll_out(uart_dev, msg[i]);
    }

    printk("Sent state, location, and time: %s", msg);
}

/* Function to encode NFC message based on current state, location, and timestamp */
static int encode_nfc_message(uint8_t *buffer, uint32_t *len, const char *datetime) {
    int err;
    char nfc_payload[256];

    /* Generate the NFC payload dynamically */
    if (current_state == STATE_LOCKED) {
        snprintf(nfc_payload, sizeof(nfc_payload), "Locked at: %s, Location: %s", datetime, fixed_location);
    } else {
        snprintf(nfc_payload, sizeof(nfc_payload), "Unlocked at: %s, Location: %s", datetime, fixed_location);
    }

    /* Debug: Print the NFC payload to verify format */
    printk("NFC payload: %s\n", nfc_payload);

    /* Clear the buffer */
    memset(buffer, 0, NDEF_MSG_BUF_SIZE);

    /* Create the NFC NDEF text record */
    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_text_rec, UTF_8, (uint8_t *)"en", 2,
                                  (uint8_t *)nfc_payload, strlen(nfc_payload));
    NFC_NDEF_MSG_DEF(nfc_text_msg, MAX_REC_COUNT);

    err = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg), &NFC_NDEF_TEXT_RECORD_DESC(nfc_text_rec));
    if (err < 0) {
        printk("Cannot add NFC record! Error: %d\n", err);
        return err;
    }

    /* Encode the NFC message */
    err = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg), buffer, len);
    if (err < 0) {
        printk("NFC encoding failed! Error: %d\n", err);
        return err;
    }

    /* Debug: Print the encoded NFC message */
    printk("NFC message encoded successfully. Length: %d\n", *len);

    return err;
}

/* NFC callback function */
static void nfc_callback(void *context, nfc_t2t_event_t event, const uint8_t *data, size_t data_length) {
    ARG_UNUSED(context);
    ARG_UNUSED(data);
    ARG_UNUSED(data_length);

    uint32_t len = sizeof(ndef_msg_buf);
    char datetime[32];

    switch (event) {
        case NFC_T2T_EVENT_FIELD_ON:
            dk_set_led_on(NFC_FIELD_LED); // Turn on LED when NFC field is detected
            toggle_state(); // Toggle the state on NFC scan

            get_current_datetime(datetime, sizeof(datetime)); // Get current date and time

            send_state_to_lora(current_state, datetime); // Send updated state, location, and time via UART

        case NFC_T2T_EVENT_FIELD_OFF:
            dk_set_led_off(NFC_FIELD_LED); // Turn off LED when NFC field is removed
            break;

        default:
            break;
    }
}

/* UART initialization */
void uart_init(void) {
    uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
    if (!device_is_ready(uart_dev)) {
        printk("UART device not found!\n");
    } else {
        printk("UART initialized successfully.\n");
    }
}

/* Main function */
int main(void) {
    uint32_t len = sizeof(ndef_msg_buf);
    int err; // Declare error variable

    printk("Starting Smart Bicycle Theft Prevention System with Date, Time, and Location Sharing\n");

    /* Initialize LEDs */
    if (dk_leds_init() < 0) {
        printk("Failed to initialize LEDs!\n");
        return -1;
    }

    /* Initialize UART */
    uart_init();

    /* Set up NFC */
    err = nfc_t2t_setup(nfc_callback, NULL);
    if (err < 0) {
        printk("Cannot setup NFC T2T library! Error: %d\n", err);
        return -1;
    }

    /* Encode initial NFC message */
    char initial_datetime[32];
    get_current_datetime(initial_datetime, sizeof(initial_datetime));
    err = encode_nfc_message(ndef_msg_buf, &len, initial_datetime);
    if (err < 0) {
        printk("Cannot encode initial NFC message! Error: %d\n", err);
        return -1;
    }

    /* Set NFC payload */
    err = nfc_t2t_payload_set(ndef_msg_buf, len);
    if (err < 0) {
        printk("Cannot set NFC payload! Error: %d\n", err);
        return -1;
    }

    /* Start NFC field sensing */
    err = nfc_t2t_emulation_start();
    if (err < 0) {
        printk("Cannot start NFC field sensing! Error: %d\n", err);
        return -1;
    }

    printk("NFC setup complete. Waiting for NFC scan...\n");

    while (1) {
        k_sleep(K_SECONDS(1)); // Keep the system running
    }

    return 0;
}
