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

#include "videosettingsaccesspointentry.h"
#include "videosettingsgroup.h"
#include <cpitemdatahelper.h>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoSettingsAccessPointEntry::VideoSettingsAccessPointEntry(
    CpItemDataHelper &itemDataHelper,
    const QString& text,
    VideoSettingsGroup *parent) :
    CpSettingFormItemData(HbDataFormModelItem::CustomItemBase, text),
    mText(text),
    mParent(parent)
{
    mParent->appendChild(this);
    Q_UNUSED(itemDataHelper);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoSettingsAccessPointEntry::~VideoSettingsAccessPointEntry()
{
}

// ---------------------------------------------------------------------------
// accessPointNameFromIapId
// ---------------------------------------------------------------------------
//
void VideoSettingsAccessPointEntry::setIapId(const uint& apId)
{
    mApId = apId;
}
