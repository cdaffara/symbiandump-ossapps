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

#include "mpxvideo_debug.h"
#include "videosettingsaccesspointentry.h"
#include "videosettingsgroup.h"
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>
#include <cpitemdatahelper.h>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoSettingsAccessPointEntry::VideoSettingsAccessPointEntry(
    CpItemDataHelper &itemDataHelper,
    const QString& text,
    VideoSettingsGroup *parent) :
    CpSettingFormEntryItemData(CpSettingFormEntryItemData::ButtonEntryItem, 
        itemDataHelper, text, QString(), QString(), parent),
    mParent(parent)
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::VideoSettingsAccessPointEntry()"));
    
    itemDataHelper.addConnection(this,SIGNAL(clicked()),this,SLOT(openSelectionDialogSlot()));
    mApplSettings = new CmApplSettingsUi(this);
    connect(mApplSettings, SIGNAL(finished(uint)), this, SLOT(accessPointDialogFinished(uint)));
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 0;
    mCmManager = new CmManagerShim();
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoSettingsAccessPointEntry::~VideoSettingsAccessPointEntry()
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::~VideoSettingsAccessPointEntry()"));
    
    delete mCmManager;
}

// ---------------------------------------------------------------------------
// accessPointNameFromIapId
// ---------------------------------------------------------------------------
//
void VideoSettingsAccessPointEntry::setIapId(const uint& apId)
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::accessPointNameFromIapId()"), 
        _L("apId = %i"), apId);
    
    QList<uint> cmArray;
    mCmManager->connectionMethod(cmArray, false);
    
    bool found = false;
    foreach(uint id, cmArray)
    {
        CmConnectionMethodShim* connMethod = getConnectionMethod(id);
        if(connMethod && connMethod->getIntAttribute(CMManagerShim::CmIapId) == apId)
        {
            mSelection.id = connMethod->getIntAttribute(CMManagerShim::CmId);
            setAccessPointName(connMethod);
            found = true;
            break;
        }
    }
    
    if(!found)
    {
        this->setDescription(hbTrId("txt_videos_dblist_none"));
    }
}

// ---------------------------------------------------------------------------
// createSettingView
// ---------------------------------------------------------------------------
//
CpBaseSettingView* VideoSettingsAccessPointEntry::createSettingView() const
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::createSettingView()"));
    
    // does not create a new view.
    return 0;
}

// ---------------------------------------------------------------------------
// getConnectionMethod
// ---------------------------------------------------------------------------
//
CmConnectionMethodShim* VideoSettingsAccessPointEntry::getConnectionMethod(const uint& id)
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::getConnectionMethod()"));
    
    CmConnectionMethodShim* connMethod = 0;
    try {
        connMethod = mCmManager->connectionMethod(id);
    } catch (const std::exception& exc) {
        MPX_DEBUG(_L("Caught exception while fetching connection method. Exception: %s"), exc.what());
        return 0;
    }
    
    return connMethod;
}

// ---------------------------------------------------------------------------
// setName
// ---------------------------------------------------------------------------
//
void VideoSettingsAccessPointEntry::setAccessPointName(CmConnectionMethodShim* connMethod)
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::setAccessPointName()"));
    
    QString name = connMethod->getStringAttribute(CMManagerShim::CmName);
    this->setDescription(name);
}

// ---------------------------------------------------------------------------
// openSelectionDialogSlot
// ---------------------------------------------------------------------------
//
void VideoSettingsAccessPointEntry::openSelectionDialogSlot()
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::openSelectionDialogSlot()"));
    
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    
    mApplSettings->setOptions(listItems, filter);
    mApplSettings->setSelection(mSelection);
    
    mApplSettings->open();
}

// ---------------------------------------------------------------------------
// accessPointDialogFinished
// ---------------------------------------------------------------------------
//
void VideoSettingsAccessPointEntry::accessPointDialogFinished(uint error)
{
    MPX_ENTER_EXIT(_L("VideoSettingsAccessPointEntry::accessPointDialogFinished()"), 
        _L("error = %i"), error);
    
    if(error == CmApplSettingsUi::ApplSettingsErrorNone)
    {
        CmApplSettingsUi::SettingSelection selection =
            mApplSettings->selection();
        MPX_DEBUG(_L("New access point connection method id = %i"), selection.id);
        CmConnectionMethodShim* connMethod = getConnectionMethod(selection.id);
        if(connMethod)
        {
            mSelection.id = selection.id;
            mParent->setAccessPointId(connMethod->getIntAttribute(CMManagerShim::CmIapId));
            setAccessPointName(connMethod);
        }
        else
        {
            MPX_DEBUG(_L("FATAL: Could not find connection method with id %i"), selection.id);
        }
    }
}
