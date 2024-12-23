/*
 * LbmxDeviceManagement.cpp
 * Copyright (C) 2023 Seeed K.K.
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "LbmxDeviceManagement.hpp"

////////////////////////////////////////////////////////////////////////////////
// LbmxDeviceManagement

smtc_modem_return_code_t LbmxDeviceManagement::setInfoInterval(smtc_modem_dm_info_interval_format_t format, uint8_t interval)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_dm_set_info_interval(format, interval)) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxDeviceManagement::setInfoFields(const std::vector<uint8_t>& fields)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_dm_set_info_fields(fields.data(), fields.size())) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

smtc_modem_return_code_t LbmxDeviceManagement::requestUplink(const std::vector<uint8_t>& fields)
{
    smtc_modem_return_code_t rc;

    if ((rc = smtc_modem_dm_request_single_uplink(fields.data(), fields.size())) != SMTC_MODEM_RC_OK) return rc;

    return SMTC_MODEM_RC_OK;
}

////////////////////////////////////////////////////////////////////////////////
