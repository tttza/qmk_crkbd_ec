# Corne EC

Fork project of corne keyboard using NiZ EC switch

-   Keyboard Maintainer: [sekigon-gonnoc](https://github.com/yourusername)
-   Hardware Supported: Corne EC PCB, Xiao RP2040
-   Hardware Availability: [BOOTH](https://nogikes.booth.pm/items/2371017)

Make example for this keyboard (after setting up your build environment):

    make sekigon/crkbd_ec:default:uf2

Flashing example for this keyboard:

    make sekigon/crkbd_ec:default:flash
    
If you want to set eeconfig handedness, add `HANDEDNESS=left` or `HANDEDNESS=right` to build command.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
