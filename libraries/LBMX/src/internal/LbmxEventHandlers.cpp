/*
 * LbmxEventHandlers.cpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "LbmxEventHandlers.hpp"
#include <lbm/smtc_modem_api/smtc_modem_middleware_advanced_api.h>
#include <mw/geolocation_middleware/gnss/src/gnss_middleware.h>
#include <mw/geolocation_middleware/wifi/src/wifi_middleware.h>

////////////////////////////////////////////////////////////////////////////////
// LbmxEventHandlers

void LbmxEventHandlers::reset(const LbmxEvent& event) { }
void LbmxEventHandlers::alarm(const LbmxEvent& event) { }
void LbmxEventHandlers::joined(const LbmxEvent& event) { }
void LbmxEventHandlers::txDone(const LbmxEvent& event) { }
void LbmxEventHandlers::downData(const LbmxEvent& event) { }
void LbmxEventHandlers::uploadDone(const LbmxEvent& event) { }
void LbmxEventHandlers::setConf(const LbmxEvent& event) { }
void LbmxEventHandlers::mute(const LbmxEvent& event) { }
void LbmxEventHandlers::streamDone(const LbmxEvent& event) { }
void LbmxEventHandlers::joinFail(const LbmxEvent& event) { }
void LbmxEventHandlers::time(const LbmxEvent& event) { }
void LbmxEventHandlers::timeoutAdrChanged(const LbmxEvent& event) { }
void LbmxEventHandlers::newLinkAdr(const LbmxEvent& event) { }
void LbmxEventHandlers::linkCheck(const LbmxEvent& event) { }
void LbmxEventHandlers::almanacUpdate(const LbmxEvent& event) { }
void LbmxEventHandlers::userRadioAccess(const LbmxEvent& event) { }
void LbmxEventHandlers::classBPingSlotInfo(const LbmxEvent& event) { }
void LbmxEventHandlers::classBStatus(const LbmxEvent& event) { }
void LbmxEventHandlers::d2dClassBTxDone(const LbmxEvent& event) { }

void LbmxEventHandlers::gnssScanCancelled(const LbmxEvent& event) { }
void LbmxEventHandlers::gnssScanDone(const LbmxEvent& event) { }
void LbmxEventHandlers::gnssTerminated(const LbmxEvent& event) { }
void LbmxEventHandlers::gnssErrorNoTime(const LbmxEvent& event) { }
void LbmxEventHandlers::gnssErrorAlmanacUpdate(const LbmxEvent& event) { }
void LbmxEventHandlers::gnssErrorNoAidingPosition(const LbmxEvent& event) { }
void LbmxEventHandlers::gnssErrorUnknown(const LbmxEvent& event) { }
void LbmxEventHandlers::gnssScanStopped(const LbmxEvent& event) { }

void LbmxEventHandlers::wifiScanCancelled(const LbmxEvent& event) { }
void LbmxEventHandlers::wifiScanDone(const LbmxEvent& event) { }
void LbmxEventHandlers::wifiTerminated(const LbmxEvent& event) { }
void LbmxEventHandlers::wifiErrorUnknown(const LbmxEvent& event) { }
void LbmxEventHandlers::wifiScanStopped(const LbmxEvent& event) { }

void LbmxEventHandlers::invoke(const LbmxEvent& event)
{
    switch (event.event_type)
    {
    case SMTC_MODEM_EVENT_RESET:
        reset(event);
        break;
    case SMTC_MODEM_EVENT_ALARM:
        alarm(event);
        break;
    case SMTC_MODEM_EVENT_JOINED:
        joined(event);
        break;
    case SMTC_MODEM_EVENT_TXDONE:
        txDone(event);
        break;
    case SMTC_MODEM_EVENT_DOWNDATA:
        downData(event);
        break;
    case SMTC_MODEM_EVENT_UPLOADDONE:
        uploadDone(event);
        break;
    case SMTC_MODEM_EVENT_SETCONF:
        setConf(event);
        break;
    case SMTC_MODEM_EVENT_MUTE:
        mute(event);
        break;
    case SMTC_MODEM_EVENT_STREAMDONE:
        streamDone(event);
        break;
    case SMTC_MODEM_EVENT_JOINFAIL:
        joinFail(event);
        break;
    case SMTC_MODEM_EVENT_TIME:
        time(event);
        break;
    case SMTC_MODEM_EVENT_TIMEOUT_ADR_CHANGED:
        timeoutAdrChanged(event);
        break;
    case SMTC_MODEM_EVENT_NEW_LINK_ADR:
        newLinkAdr(event);
        break;
    case SMTC_MODEM_EVENT_LINK_CHECK:
        linkCheck(event);
        break;
    case SMTC_MODEM_EVENT_ALMANAC_UPDATE:
        almanacUpdate(event);
        break;
    case SMTC_MODEM_EVENT_USER_RADIO_ACCESS:
        userRadioAccess(event);
        break;
    case SMTC_MODEM_EVENT_CLASS_B_PING_SLOT_INFO:
        classBPingSlotInfo(event);
        break;
    case SMTC_MODEM_EVENT_CLASS_B_STATUS:
        classBStatus(event);
        break;
    case SMTC_MODEM_EVENT_D2D_CLASS_B_TX_DONE:
        d2dClassBTxDone(event);
        break;
    case SMTC_MODEM_EVENT_MIDDLEWARE_1:
        {
            const uint8_t events = event.event_data.middleware_event_status.status;

            if (gnss_mw_has_event(events, GNSS_MW_EVENT_SCAN_CANCELLED          )) gnssScanCancelled(event);
            if (gnss_mw_has_event(events, GNSS_MW_EVENT_SCAN_DONE               )) gnssScanDone(event);
            if (gnss_mw_has_event(events, GNSS_MW_EVENT_TERMINATED              )) gnssTerminated(event);
            if (gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_NO_TIME           )) gnssErrorNoTime(event);
            if (gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_ALMANAC_UPDATE    )) gnssErrorAlmanacUpdate(event);
            if (gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_NO_AIDING_POSITION)) gnssErrorNoAidingPosition(event);
            if (gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_UNKNOWN           )) gnssErrorUnknown(event);

            if (gnss_mw_has_event(events, GNSS_MW_EVENT_SCAN_CANCELLED)           ||
                gnss_mw_has_event(events, GNSS_MW_EVENT_TERMINATED)               ||
                gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_NO_TIME)            ||
                gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_ALMANAC_UPDATE)     ||
                gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_NO_AIDING_POSITION) ||
                gnss_mw_has_event(events, GNSS_MW_EVENT_ERROR_UNKNOWN)              )
            {
                gnssScanStopped(event);
            }

            gnss_mw_clear_pending_events();
        }
        break;
    case SMTC_MODEM_EVENT_MIDDLEWARE_2:
        {
            const uint8_t events = event.event_data.middleware_event_status.status;

            if (wifi_mw_has_event(events, WIFI_MW_EVENT_SCAN_CANCELLED)) wifiScanCancelled(event);
            if (wifi_mw_has_event(events, WIFI_MW_EVENT_SCAN_DONE     )) wifiScanDone(event);
            if (wifi_mw_has_event(events, WIFI_MW_EVENT_TERMINATED    )) wifiTerminated(event);
            if (wifi_mw_has_event(events, WIFI_MW_EVENT_ERROR_UNKNOWN )) wifiErrorUnknown(event);

            if (wifi_mw_has_event(events, WIFI_MW_EVENT_SCAN_CANCELLED) ||
                wifi_mw_has_event(events, WIFI_MW_EVENT_TERMINATED    ) ||
                wifi_mw_has_event(events, WIFI_MW_EVENT_ERROR_UNKNOWN )   )
            {
                wifiScanStopped(event);
            }

            wifi_mw_clear_pending_events();
        }
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
