/*
 * LbmxDeviceManagement.hpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <vector>
#include <lbm/smtc_modem_api/smtc_modem_utilities.h>

////////////////////////////////////////////////////////////////////////////////
// LbmxDeviceManagement

class LbmxDeviceManagement
{
private:
    LbmxDeviceManagement() = delete;
    LbmxDeviceManagement(const LbmxDeviceManagement&) = delete;

public:
    static smtc_modem_return_code_t setInfoInterval(smtc_modem_dm_info_interval_format_t format, uint8_t interval);
    static smtc_modem_return_code_t setInfoFields(const std::vector<uint8_t>& fields);

    static smtc_modem_return_code_t requestUplink(const std::vector<uint8_t>& fields);

};

////////////////////////////////////////////////////////////////////////////////
