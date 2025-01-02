/*
 * LbmxEvent.cpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "LbmxEvent.hpp"
#include <cstdlib>
#include <lbm/smtc_modem_api/smtc_modem_middleware_advanced_api.h>
#include <mw/geolocation_middleware/gnss/src/gnss_middleware.h>
#include <mw/geolocation_middleware/wifi/src/wifi_middleware.h>

////////////////////////////////////////////////////////////////////////////////
// LbmxEvent

LbmxEvent::LbmxEvent()
{
    event_type = SMTC_MODEM_EVENT_NONE;
}

std::string LbmxEvent::getEventString() const
{
    switch (event_type)
    {
    case SMTC_MODEM_EVENT_RESET:
        return "RESET";
    case SMTC_MODEM_EVENT_ALARM:
        return "ALARM";
    case SMTC_MODEM_EVENT_JOINED:
        return "JOINED";
    case SMTC_MODEM_EVENT_TXDONE:
        return "TXDONE";
    case SMTC_MODEM_EVENT_DOWNDATA:
        return "DOWNDATA";
    case SMTC_MODEM_EVENT_UPLOADDONE:
        return "UPLOADDONE";
    case SMTC_MODEM_EVENT_SETCONF:
        return "SETCONF";
    case SMTC_MODEM_EVENT_MUTE:
        return "MUTE";
    case SMTC_MODEM_EVENT_STREAMDONE:
        return "STREAMDONE";
    case SMTC_MODEM_EVENT_JOINFAIL:
        return "JOINFAIL";
    case SMTC_MODEM_EVENT_TIME:
        return "TIME";
    case SMTC_MODEM_EVENT_TIMEOUT_ADR_CHANGED:
        return "TIMEOUT_ADR_CHANGED";
    case SMTC_MODEM_EVENT_NEW_LINK_ADR:
        return "NEW_LINK_ADR";
    case SMTC_MODEM_EVENT_LINK_CHECK:
        return "LINK_CHECK";
    case SMTC_MODEM_EVENT_ALMANAC_UPDATE:
        return "ALMANAC_UPDATE";
    case SMTC_MODEM_EVENT_USER_RADIO_ACCESS:
        return "USER_RADIO_ACCESS";
    case SMTC_MODEM_EVENT_CLASS_B_PING_SLOT_INFO:
        return "CLASS_B_PING_SLOT_INFO";
    case SMTC_MODEM_EVENT_CLASS_B_STATUS:
        return "CLASS_B_STATUS";
    case SMTC_MODEM_EVENT_D2D_CLASS_B_TX_DONE:
        return "D2D_CLASS_B_TX_DONE";
    case SMTC_MODEM_EVENT_MIDDLEWARE_1:
        return "MIDDLEWARE_1";
    case SMTC_MODEM_EVENT_MIDDLEWARE_2:
        return "MIDDLEWARE_2";
    case SMTC_MODEM_EVENT_MIDDLEWARE_3:
        return "MIDDLEWARE_3";
    case SMTC_MODEM_EVENT_NONE:
        return "NONE";
    default:
        return "???";
    }
}

std::string LbmxEvent::getGnssEventString(uint8_t status)
{
    switch (status)
    {
    case GNSS_MW_EVENT_SCAN_CANCELLED:
        return "SCAN_CANCELLED";
    case GNSS_MW_EVENT_SCAN_DONE:
        return "SCAN_DONE";
    case GNSS_MW_EVENT_TERMINATED:
        return "TERMINATED";
    case GNSS_MW_EVENT_ERROR_NO_TIME:
        return "ERROR_NO_TIME";
    case GNSS_MW_EVENT_ERROR_ALMANAC_UPDATE:
        return "ERROR_ALMANAC_UPDATE";
    case GNSS_MW_EVENT_ERROR_NO_AIDING_POSITION:
        return "ERROR_NO_AIDING_POSITION";
    case GNSS_MW_EVENT_ERROR_UNKNOWN:
        return "ERROR_UNKNOWN";
    default:
        return "???";
    }
}

std::string LbmxEvent::getWifiEventString(uint8_t status)
{
    switch (status)
    {
    case WIFI_MW_EVENT_SCAN_CANCELLED:
        return "SCAN_CANCELLED";
    case WIFI_MW_EVENT_SCAN_DONE:
        return "SCAN_DONE";
    case WIFI_MW_EVENT_TERMINATED:
        return "TERMINATED";
    case WIFI_MW_EVENT_ERROR_UNKNOWN:
        return "ERROR_UNKNOWN";
    default:
        return "???";
    }
}

bool LbmxEvent::fetch()
{
    uint8_t modemEventPendingCount;
    if (smtc_modem_get_event(this, &modemEventPendingCount) != SMTC_MODEM_RC_OK) abort();

    return event_type != SMTC_MODEM_EVENT_NONE;
}

////////////////////////////////////////////////////////////////////////////////
