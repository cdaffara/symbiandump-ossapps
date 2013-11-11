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
#include <cpsettingformentryitemdataimpl.h>
#include "cpdivertplugin.h"
#include "cpdivertplugingroup.h"
#include "cppluginlogging.h"

/*!
    Constructor of CpDivertPlugin.
 */
CpDivertPlugin::CpDivertPlugin()
{
    // Install plugin specific msg handler
    INSTALL_TRACE_MSG_HANDLER;
    DPRINT;
}

/*!
    Destructor of CpDivertPlugin.
 */
CpDivertPlugin::~CpDivertPlugin()
{
    DPRINT;

    // Uninstall plugin specific msg handler
    UNINSTALL_TRACE_MSG_HANDLER;
}

/*!
    CpDivertPlugin::createSettingFormItemData.
 */
QList<CpSettingFormItemData*> CpDivertPlugin::createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const
{
    DPRINT;
    QList<CpSettingFormItemData*> ret;
    ret.append(new CpDivertPluginGroup(itemDataHelper));
    return ret;
}

Q_EXPORT_PLUGIN2(cpdivertplugin, CpDivertPlugin);
