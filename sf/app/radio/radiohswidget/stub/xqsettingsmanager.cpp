/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  XQSettingsManager stub for FM Radio home screen widget
*               unit testing.
*
*/

// User includes
#include "xqsettingsmanager.h"
#include "radiologger.h"

extern QVariant settings_manager_mock_return_profile;
extern QVariant settings_manager_mock_return_2nd_profile;
extern bool settings_manager_mock_bool_return;

/*!
    \class XQSettingsManager
    \brief Stub implementation of XQSettingsManager for unit testing
*/

// ======== MEMBER FUNCTIONS ========

XQSettingsManager::XQSettingsManager(QObject* parent) : QObject(parent)
{
    LOG_METHOD_ENTER;
}

XQSettingsManager::~XQSettingsManager()
{
    LOG_METHOD_ENTER;
}

QVariant XQSettingsManager::readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    LOG_METHOD_ENTER;
    Q_UNUSED(key);
    Q_UNUSED(type);
    return settings_manager_mock_return_profile;
}

bool XQSettingsManager::startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    LOG_METHOD_ENTER;
    // Set the 2nd startup mode.
    settings_manager_mock_return_profile = settings_manager_mock_return_2nd_profile;
    emit valueChanged(key, type);
    return settings_manager_mock_bool_return;
}

bool XQSettingsManager::stopMonitoring(const XQSettingsKey& key)
{
    return true;
}

