
#include "iusb.h"
#include "device/usbd.h"
#include "hid_device.h"
#include "usb_descriptors.h"

/* declarations */
uint8_t keyboard_leds(void) {
    // TODO
    return 0;
}

void send_keyboard(report_keyboard_t *report) {
    static report_keyboard_t last_report = {0};
    if (memcmp(&last_report, report, sizeof(last_report)) == 0) {
        return;
    } else {
        last_report = *report;
    }

    while (!tud_hid_n_ready(ITF_NUM_HID_KEYBOARD)) {
        tud_task();
        if (!tud_ready()) {
            return;
        }
    }
    tud_hid_n_report(ITF_NUM_HID_KEYBOARD, 0, report, sizeof(*report));
}

void send_mouse(report_mouse_t *report) {
    static report_mouse_t last_report = {0};
    if (memcmp(&last_report, report, sizeof(last_report)) == 0) {
        if (last_report.h == 0 && last_report.v == 0 && last_report.x == 0 &&
            last_report.y == 0) {
            return;
        }
    } else {
        last_report = *report;
    }


    if (!tud_ready()) {
        return;
    }
    while (!tud_hid_n_ready(ITF_NUM_HID_MOUSE)) {
        tud_task();
        if (!tud_ready()) {
            return;
        }
    }
    tud_hid_n_report(ITF_NUM_HID_MOUSE, 0, report, sizeof(*report));
}

void send_system(uint16_t data) {
    while (!tud_hid_n_ready(ITF_NUM_HID_EXTRA)) {
        tud_task();
        if (!tud_ready()) {
            return;
        }
    }
    tud_hid_n_report(ITF_NUM_HID_EXTRA, REPORT_ID_SYSTEM, &data, 2);
}
void send_consumer(uint16_t data) {
    while (!tud_hid_n_ready(ITF_NUM_HID_EXTRA)) {
        tud_task();
        if (!tud_ready()) {
            return;
        }
    }
    tud_hid_n_report(ITF_NUM_HID_EXTRA, REPORT_ID_CONSUMER, &data, 2);
}
