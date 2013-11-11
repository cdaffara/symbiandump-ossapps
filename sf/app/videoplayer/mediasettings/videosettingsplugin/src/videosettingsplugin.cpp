/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#include "videosettingsplugin.h"
#include "videosettingsgroup.h"
#include <cpsettingformentryitemdataimpl.h>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoSettingsPlugin::VideoSettingsPlugin()
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoSettingsPlugin::~VideoSettingsPlugin()
{
}

// ---------------------------------------------------------------------------
// createSettingFormItemData
// ---------------------------------------------------------------------------
//
QList<CpSettingFormItemData*> VideoSettingsPlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    return QList<CpSettingFormItemData*>() << new VideoSettingsGroup(itemDataHelper);
}

Q_EXPORT_PLUGIN2(videosettingsplugin, VideoSettingsPlugin);
