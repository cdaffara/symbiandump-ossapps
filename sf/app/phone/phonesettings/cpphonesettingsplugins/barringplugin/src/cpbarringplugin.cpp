/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cpbarringplugin.h"
#include "cpbarringplugingroup.h"
#include "cppluginlogging.h"

/*!
    Constructor of CpBarringPlugin.
 */
CpBarringPlugin::CpBarringPlugin()
{
    // Install plugin specific msg handler
    INSTALL_TRACE_MSG_HANDLER; 

    DPRINT;
}

/*!
    Destructor of CpBarringPlugin.
 */
CpBarringPlugin::~CpBarringPlugin()
{
    DPRINT;
    
    // Uninstall plugin specific msg handler
    UNINSTALL_TRACE_MSG_HANDLER;
}

/*!
    CpBarringPlugin::createSettingFormItemData.
 */
QList<CpSettingFormItemData*> CpBarringPlugin::createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const
{
    DPRINT;
    QList<CpSettingFormItemData*> ret;
    ret.append(new CpBarringPluginGroup(itemDataHelper));
    return ret;
}

Q_EXPORT_PLUGIN2(CpBarringPlugin, CpBarringPlugin);
