// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "../../rp.h"
#include "../../../report_parser.h"
#include "../../../report_descriptor_parser.h"

#include "matrix.h"

static uint8_t pre_keyreport[8];

__attribute__((weak)) void report_descriptor_parser_user(uint8_t        dev_num,
                                                         uint8_t const *buf,
                                                         uint16_t       len) {
    if (keyboard_config.parser_type == 1) {
        // no descriptor parser
    } else {
        parse_report_descriptor(dev_num, buf, len);
    }
}

__attribute__((weak)) bool report_parser_user(uint8_t const *buf, uint16_t len,
                                              matrix_row_t *current_matrix) {
    packet_header_t const *packet_header = (packet_header_t const *)buf;

    if (keyboard_config.parser_type == 1) {
        return report_parser_fixed(buf, packet_header->len, pre_keyreport,
                                   current_matrix);
    } else {
        return parse_report(packet_header->dev_num, &packet_header->data_start,
                            packet_header->len);
    }
}

__attribute__((weak)) void on_disconnect_device_user(uint8_t device) {
    memset(pre_keyreport, 0, sizeof(pre_keyreport));
}
