/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  
 *
 */
#include "cpcallsplugin.h"
#include "cpcallsplugingroup.h"
#include "cppluginlogging.h"
#include <cpsettingformentryitemdataimpl.h>
#include <QtDebug>

/*!
    Constructor of CpCallsPlugin.
 */
CpCallsPlugin::CpCallsPlugin() : QObject(0)
{
    // Install plugin specific msg handler
    INSTALL_TRACE_MSG_HANDLER;
    
    DPRINT;
}

/*!
    Destructor of CpCallsPlugin.
 */
CpCallsPlugin::~CpCallsPlugin()
{
    DPRINT;
    
    // Uninstall plugin specific msg handler
    UNINSTALL_TRACE_MSG_HANDLER;
}

/*!
    CpCallsPlugin::createSettingFormItemData.
 */
QList<CpSettingFormItemData*> CpCallsPlugin::createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const
{
    DPRINT;
    QList<CpSettingFormItemData*> ret;
    ret.append(new CpCallsPluginGroup(itemDataHelper));
    return ret;
}

Q_EXPORT_PLUGIN2(cpcallsplugin, CpCallsPlugin);

