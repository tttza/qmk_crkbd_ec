
#pragma once

#include "usb_host_os_identifier.h"

void host_os_eeconfig_update(OS_TYPE os);
void host_os_eeconfig_init(void);

// weak symbol
void on_host_os_eeconfig_update(void);
