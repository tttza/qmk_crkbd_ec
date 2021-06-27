
#include "iusb.h"
#include "hid_device.h"
#include "usb_descriptors.h"

/* declarations */
uint8_t keyboard_leds(void) {
    // TODO
    return 0;
}

void    send_keyboard(report_keyboard_t *report)
{
    tud_hid_n_report(ITF_NUM_HID_KEYBOARD, 0, report, sizeof(*report));
}

void    send_mouse(report_mouse_t *report)
{
    tud_hid_n_report(ITF_NUM_HID_MOUSE, 0, report, sizeof(*report));
}

void    send_system(uint16_t data)
{
    tud_hid_n_report(ITF_NUM_HID_EXTRA, REPORT_ID_SYSTEM, &data, 2);
}
void    send_consumer(uint16_t data)
{
    tud_hid_n_report(ITF_NUM_HID_EXTRA, REPORT_ID_CONSUMER, &data, 2);
}

