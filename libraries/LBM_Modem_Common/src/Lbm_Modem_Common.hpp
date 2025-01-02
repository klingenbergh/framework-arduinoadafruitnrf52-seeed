/*
 * Lbm_Modem_Common.hpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

#pragma once




////////////////////////////////////////////////////////////////////////////////
// Includes



#include <cstdio>

#include "common/apps_modem_common.h"
#include "common/smtc_hal_dbg_trace.h"
#include "common/lorawan_key_config.h"
#include "common/apps_utilities.h"
#include "common/smtc_hal_mcu.h"
#include "common/smtc_hal_options.h"
#include "common/apps_common_util/apps_common_util.h"


#include <mw/geolocation_middleware/common/mw_common.h>
#include <mw/geolocation_middleware/gnss/src/gnss_middleware.h>

#include <mw/geolocation_middleware/wifi/src/wifi_middleware.h>
#include <mw/geolocation_middleware/wifi/src/wifi_helpers.h>

#include <lbm/smtc_modem_core/smtc_ralf/src/ralf.h>
#include <lbm/smtc_modem_hal/smtc_modem_hal.h>
#include <lbm/smtc_modem_core/radio_drivers/lr11xx_driver/src/lr11xx_gnss_types.h>
#include <lbm/smtc_modem_core/radio_drivers/lr11xx_driver/src/lr11xx_system.h>





