/*
 * LbmxEvent.hpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <string>
#include <lbm/smtc_modem_api/smtc_modem_utilities.h>

////////////////////////////////////////////////////////////////////////////////
// LbmxEvent

class LbmxEvent : public smtc_modem_event_t
{
public:
    LbmxEvent();
    LbmxEvent(const LbmxEvent&) = delete;

    std::string getEventString() const;
    static std::string getGnssEventString(uint8_t status);
    static std::string getWifiEventString(uint8_t status);

    bool fetch();

};

////////////////////////////////////////////////////////////////////////////////
