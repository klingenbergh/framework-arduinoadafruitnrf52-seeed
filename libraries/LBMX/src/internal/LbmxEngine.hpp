/*
 * LbmxEngine.hpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <lbm/smtc_modem_api/smtc_modem_utilities.h>

////////////////////////////////////////////////////////////////////////////////
// LbmxEngine

class LbmxEngine
{
private:
    static constexpr uint8_t stackId = 0;

private:
    LbmxEngine() = delete;
    LbmxEngine(const LbmxEngine&) = delete;

public:
    static void begin(const ralf_t* radio, void(*eventCallback)());
    static uint32_t doWork();

    static void printVersions(ralf_t* ralf);

    static smtc_modem_return_code_t setRegion(smtc_modem_region_t region);
    static smtc_modem_return_code_t setOTAA(const uint8_t devEui[SMTC_MODEM_EUI_LENGTH], const uint8_t joinEui[SMTC_MODEM_EUI_LENGTH], const uint8_t nwkKey[SMTC_MODEM_KEY_LENGTH]);

    static smtc_modem_return_code_t joinNetwork();

    static smtc_modem_return_code_t requestUplink(uint8_t fport, bool confirmed, const void* payload, uint8_t payloadSize);
    static smtc_modem_return_code_t requestEmptyUplink(bool confirmed);
    static smtc_modem_return_code_t requestEmptyUplink(uint8_t fport, bool confirmed);

    static smtc_modem_return_code_t startAlarm(uint32_t activateTimeSeconds);
    static smtc_modem_return_code_t stopAlarm();

};

////////////////////////////////////////////////////////////////////////////////
