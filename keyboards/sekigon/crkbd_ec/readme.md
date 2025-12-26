# Corne EC

Fork project of corne keyboard using NiZ EC switch

-   Keyboard Maintainer: [sekigon-gonnoc](https://github.com/yourusername)
-   Hardware Supported: Corne EC PCB, Xiao RP2040
-   Hardware Availability: [BOOTH](https://nogikes.booth.pm/items/2371017)

Build example (after setting up your build environment):

    qmk compile -kb sekigon/crkbd_ec -km default

Flashing example:

    qmk flash -kb sekigon/crkbd_ec -km default

Handedness defaults to EE_HANDS with SPLIT_USB_DETECT; if you need to force a side you can pass `MASTER=left` or `MASTER=right` to the build/flash command.

Electrostatic threshold is configurable via VIA raw command `0xec` (send `0xffff` for both values to reset).

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
