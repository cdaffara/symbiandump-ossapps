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
* Description:   CpSettingFormEntryItemData stub
* 
*/

// INCLUDE FILES
#include "cpsettingformentryitemdata.h"

// ---------------------------------------------------------------------------
// CpSettingFormEntryItemData
// ---------------------------------------------------------------------------
//
CpSettingFormEntryItemData::CpSettingFormEntryItemData(
    EntryItemType type,
    CpItemDataHelper &itemDataHelper,
    const QString &text,
    const QString &description,
    const QString &iconName,
    const HbDataFormModelItem *parent) :
    mText(text),
    mDescription(description)
{
    Q_UNUSED(type);
    Q_UNUSED(itemDataHelper);
    Q_UNUSED(iconName);
    Q_UNUSED(parent);
}

// ---------------------------------------------------------------------------
// CpSettingFormEntryItemData
// ---------------------------------------------------------------------------
//
CpSettingFormEntryItemData::~CpSettingFormEntryItemData()
{
}

// ---------------------------------------------------------------------------
// text
// ---------------------------------------------------------------------------
//
QString CpSettingFormEntryItemData::text() const
{
    return mText;
}

// ---------------------------------------------------------------------------
// setText
// ---------------------------------------------------------------------------
//
void CpSettingFormEntryItemData::setText(const QString &text)
{
    mText = text;
}

// ---------------------------------------------------------------------------
// description
// ---------------------------------------------------------------------------
//
QString CpSettingFormEntryItemData::description() const
{
    return mDescription;
}

// ---------------------------------------------------------------------------
// setDescription
// ---------------------------------------------------------------------------
//
void CpSettingFormEntryItemData::setDescription(const QString &description)
{
    mDescription = description;
}

// ---------------------------------------------------------------------------
// callCreateSettingView
// ---------------------------------------------------------------------------
//
CpBaseSettingView* CpSettingFormEntryItemData::callCreateSettingView() const
{
    return createSettingView();
}
