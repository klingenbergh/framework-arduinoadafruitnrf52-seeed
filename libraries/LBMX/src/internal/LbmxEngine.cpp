/*
 * LbmxEngine.cpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "LbmxEngine.hpp"
#include <cstdio>
#include <cstdlib>
#include <mw/geolocation_middleware/gnss/src/gnss_middleware.h>
#include <mw/geolocation_middleware/wifi/src/wifi_middleware.h>
#include <lbm/smtc_modem_core/smtc_ralf/src/ralf.h>
#ifdef LR11XX
#include <lbm/smtc_modem_core/radio_drivers/lr11xx_driver/src/lr11xx_driver_version.h>
#include <lbm/smtc_modem_core/radio_drivers/lr11xx_driver/src/lr11xx_system.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// LbmxEngine

void LbmxEngine::begin(const ralf_t* radio, void(*eventCallback)())
{
    smtc_modem_init(radio, eventCallback);
}

uint32_t LbmxEngine::doWork()
{
    return smtc_modem_run_engine();
}

void LbmxEngine::printVersions(ralf_t* ralf)
{
#ifdef LR11XX
    lr11xx_system_version_t systemVersion;
    if (smtc_modem_suspend_before_user_radio_access() != SMTC_MODEM_RC_OK) abort();
    lr11xx_system_get_version(ralf->ral.context, &systemVersion);
    if (smtc_modem_resume_after_user_radio_access() != SMTC_MODEM_RC_OK) abort();
#endif

    smtc_modem_version_t modemVersion;
    if (smtc_modem_get_modem_version(&modemVersion) != SMTC_MODEM_RC_OK) abort();

    smtc_modem_lorawan_version_t lorawanVersion;
    if (smtc_modem_get_lorawan_version(&lorawanVersion) != SMTC_MODEM_RC_OK) abort();

    smtc_modem_lorawan_version_t rpVersion;
    if (smtc_modem_get_regional_params_version(&rpVersion) != SMTC_MODEM_RC_OK) abort();

    mw_version_t gnssVersion;
    if (gnss_mw_get_version(&gnssVersion) != MW_RC_OK) abort();

    mw_version_t wifiVersion;
    if (wifi_mw_get_version(&wifiVersion) != MW_RC_OK) abort();

    printf("LBM versions:\n");
#ifdef LR11XX
    printf(" LR11xx system    = hw:0x%02x, type:0x%02x, fw:0x%04x\n", systemVersion.hw, systemVersion.type, systemVersion.fw);
    printf(" LR11xx driver    = %s\n", lr11xx_driver_version_get_version_string());
#endif
    printf(" Modem            = v%d.%d.%d\n", modemVersion.major, modemVersion.minor, modemVersion.patch);
    printf("  LoRaWAN         = v%d.%d.%d.%d\n", lorawanVersion.major, lorawanVersion.minor, lorawanVersion.patch, lorawanVersion.revision);
    printf("  Regional params = v%d.%d.%d.%d\n", rpVersion.major, rpVersion.minor, rpVersion.patch, rpVersion.revision);
    printf(" GNSS             = v%d.%d.%d\n", gnssVersion.major, gnssVersion.minor, gnssVersion.patch);
    printf(" Wi-Fi            = v%d.%d.%d\n", wifiVersion.major, wifiVersion.minor, wifiVersion.patch);
}

smtc_modem_return_code_t LbmxEngine::setRegion(smtc_modem_region_t region)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_set_region(stackId, region)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxEngine::setOTAA(const uint8_t devEui[SMTC_MODEM_EUI_LENGTH], const uint8_t joinEui[SMTC_MODEM_EUI_LENGTH], const uint8_t nwkKey[SMTC_MODEM_KEY_LENGTH])
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_set_deveui(stackId, devEui)) != SMTC_MODEM_RC_OK) return rc;
    if ((rc = smtc_modem_set_joineui(stackId, joinEui)) != SMTC_MODEM_RC_OK) return rc;
    if ((rc = smtc_modem_set_nwkkey(stackId, nwkKey)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxEngine::joinNetwork()
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_join_network(stackId)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxEngine::requestUplink(uint8_t fport, bool confirmed, const void* payload, uint8_t payloadSize)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_request_uplink(stackId, fport, confirmed, reinterpret_cast<const uint8_t*>(payload), payloadSize)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxEngine::requestEmptyUplink(bool confirmed)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_request_empty_uplink(stackId, false, 0, confirmed)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxEngine::requestEmptyUplink(uint8_t fport, bool confirmed)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_request_empty_uplink(stackId, true, fport, confirmed)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxEngine::startAlarm(uint32_t activateTimeSeconds)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_alarm_start_timer(activateTimeSeconds)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxEngine::stopAlarm()
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_alarm_clear_timer()) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

////////////////////////////////////////////////////////////////////////////////
