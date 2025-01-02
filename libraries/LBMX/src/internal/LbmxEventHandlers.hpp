/*
 * LbmxEventHandlers.hpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "LbmxEvent.hpp"

////////////////////////////////////////////////////////////////////////////////
// LbmxEventHandlers

class LbmxEventHandlers
{
protected:
    virtual void reset(const LbmxEvent& event);                 // event.event_data.reset
    virtual void alarm(const LbmxEvent& event);
    virtual void joined(const LbmxEvent& event);
    virtual void txDone(const LbmxEvent& event);                // event.event_data.txdone
    virtual void downData(const LbmxEvent& event);              // event.event_data.downdata
    virtual void uploadDone(const LbmxEvent& event);            // event.event_data.uploaddone
    virtual void setConf(const LbmxEvent& event);               // event.event_data.setconf
    virtual void mute(const LbmxEvent& event);                  // event.event_data.mute
    virtual void streamDone(const LbmxEvent& event);
    virtual void joinFail(const LbmxEvent& event);
    virtual void time(const LbmxEvent& event);                  // event.event_data.time
    virtual void timeoutAdrChanged(const LbmxEvent& event);
    virtual void newLinkAdr(const LbmxEvent& event);
    virtual void linkCheck(const LbmxEvent& event);             // event.event_data.link_check
    virtual void almanacUpdate(const LbmxEvent& event);         // event.event_data.almanac_update
    virtual void userRadioAccess(const LbmxEvent& event);       // event.event_data.user_radio_access
    virtual void classBPingSlotInfo(const LbmxEvent& event);    // event.event_data.class_b_ping_slot_info
    virtual void classBStatus(const LbmxEvent& event);          // event.event_data.class_b_status
    virtual void d2dClassBTxDone(const LbmxEvent& event);       // event.event_data.d2d_class_b_tx_done

    virtual void gnssScanCancelled(const LbmxEvent& event);         // event.event_data.middleware_event_status
    virtual void gnssScanDone(const LbmxEvent& event);              // event.event_data.middleware_event_status
    virtual void gnssTerminated(const LbmxEvent& event);            // event.event_data.middleware_event_status
    virtual void gnssErrorNoTime(const LbmxEvent& event);           // event.event_data.middleware_event_status
    virtual void gnssErrorAlmanacUpdate(const LbmxEvent& event);    // event.event_data.middleware_event_status
    virtual void gnssErrorNoAidingPosition(const LbmxEvent& event); // event.event_data.middleware_event_status
    virtual void gnssErrorUnknown(const LbmxEvent& event);          // event.event_data.middleware_event_status
    virtual void gnssScanStopped(const LbmxEvent& event);           // event.event_data.middleware_event_status

    virtual void wifiScanCancelled(const LbmxEvent& event);         // event.event_data.middleware_event_status
    virtual void wifiScanDone(const LbmxEvent& event);              // event.event_data.middleware_event_status
    virtual void wifiTerminated(const LbmxEvent& event);            // event.event_data.middleware_event_status
    virtual void wifiErrorUnknown(const LbmxEvent& event);          // event.event_data.middleware_event_status
    virtual void wifiScanStopped(const LbmxEvent& event);           // event.event_data.middleware_event_status

public:
    void invoke(const LbmxEvent& event);

};

////////////////////////////////////////////////////////////////////////////////
