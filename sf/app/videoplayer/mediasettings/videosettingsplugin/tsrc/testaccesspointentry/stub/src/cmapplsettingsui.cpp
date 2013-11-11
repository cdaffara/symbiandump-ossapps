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
* Description:   CmApplSettingsUi stub
* 
*/

// INCLUDE FILES
#include "cmapplsettingsui.h"

// ---------------------------------------------------------------------------
// CmApplSettingsUi
// ---------------------------------------------------------------------------
//
CmApplSettingsUi::CmApplSettingsUi(QObject* parent) 
  : QObject(parent),
    mOpenCallAmount(0)
{
}

// ---------------------------------------------------------------------------
// CmApplSettingsUi
// ---------------------------------------------------------------------------
//
CmApplSettingsUi::~CmApplSettingsUi()
{
}

// ---------------------------------------------------------------------------
// setOptions
// ---------------------------------------------------------------------------
//
void CmApplSettingsUi::setOptions(
    const QFlags<SelectionDialogItems> &listItems,
    const QSet<BearerTypeFilter> &bearerFilter)
{
    mListItems = listItems;
    mBearerTypeFilter = bearerFilter;
}

// ---------------------------------------------------------------------------
// setSelection
// ---------------------------------------------------------------------------
//
void CmApplSettingsUi::setSelection(const SettingSelection &selection)
{
    mSelection = selection;
}

// ---------------------------------------------------------------------------
// selection
// ---------------------------------------------------------------------------
//
CmApplSettingsUi::SettingSelection CmApplSettingsUi::selection() const
{
    return mSelection;
}

// ---------------------------------------------------------------------------
// open
// ---------------------------------------------------------------------------
//
void CmApplSettingsUi::open()
{
    mOpenCallAmount++;
}

// ---------------------------------------------------------------------------
// emitFinished
// ---------------------------------------------------------------------------
//
void CmApplSettingsUi::emitFinished(uint status)
{
    emit finished(status);
}
