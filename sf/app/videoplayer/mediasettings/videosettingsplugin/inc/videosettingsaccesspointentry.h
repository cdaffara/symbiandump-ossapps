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
#ifndef VIDEOSETTINGSACCESSPOINTENTRY_H
#define VIDEOSETTINGSACCESSPOINTENTRY_H

#include <cpsettingformentryitemdata.h>
#include <cmapplsettingsui.h>

class VideoSettingsGroup;
class CmConnectionMethodShim;
class CmManagerShim;

class VideoSettingsAccessPointEntry : public CpSettingFormEntryItemData
{
    Q_OBJECT
public:
    
    /**
     * Constructor
     */
    VideoSettingsAccessPointEntry(CpItemDataHelper &itemDataHelper, 
        const QString& text = QString(),
        VideoSettingsGroup *parent = 0);
    
    /**
     * Destructor
     */
    ~VideoSettingsAccessPointEntry();
    
    /**
     * sets the iap id.
     * 
     * @param apId The iap id.
     */
    void setIapId(const uint& apId);
    
private:
    
    /**
     * From CpSettingFormEntryItemData. 
     * 
     * Creates a view for this, if needed. In our case always 
     * returns 0 as we don't want a new view.
     */
    virtual CpBaseSettingView *createSettingView() const;
    
    /**
     * Convinience method for fetching a connection method
     * with a given id.
     * 
     * @param id Connection method id.
     * @return The connection method object.
     */
    CmConnectionMethodShim* getConnectionMethod(const uint& id);
    
    /**
     * Fetches the connection name from the given connection method
     * and sets it as the description for this.
     */
    void setAccessPointName(CmConnectionMethodShim* connMethod);
    
private slots:
    
    /**
     * Slot that is called when the button is pressed.
     * Opens the access point selection dialog.
     */
    void openSelectionDialogSlot();
    
    /**
     * Slot that is called when the access point selection
     * dialog closes.
     * 
     * @param error Possible error code. One from enum
     *      CmApplSettingsUi::ApplSettingsError.
     */
    void accessPointDialogFinished(uint error);
    
private:
    
    /**
     * The VideoSettingsGroup.
     */
    VideoSettingsGroup* mParent;
    
    /**
     * The access point selection dialog.
     */
    CmApplSettingsUi* mApplSettings;
    
    /**
     * The current access point selection.
     */
    CmApplSettingsUi::SettingSelection mSelection;
    
    /**
     * The manager that handles all destinations and connection methods.
     * Used for getting the connection methods.
     */
    CmManagerShim* mCmManager;

};

#endif // VIDEOSETTINGSACCESSPOINTENTRY_H
