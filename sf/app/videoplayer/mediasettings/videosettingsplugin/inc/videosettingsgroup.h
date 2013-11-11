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
#ifndef VIDEOSETTINGSGROUP_H
#define VIDEOSETTINGSGROUP_H

#include <cpsettingformitemdata.h>
#include <qabstractitemmodel.h>
#include <qtranslator.h>

class CpItemDataHelper;
class CMPSettingsModel;
class VideoSettingsAccessPointEntry;

class VideoSettingsGroup : public CpSettingFormItemData
{
    Q_OBJECT
public:
    
    /**
     * Constructor
     */
    VideoSettingsGroup(CpItemDataHelper &itemDataHelper);
    
    /**
     * Destructor
     */
    ~VideoSettingsGroup();
    
    /**
     * Getter for the default access point id.
     * 
     * @return Default access point id.
     */
    uint getAccessPointId();
    
    /**
     * Setter for the default access point id.
     * 
     * @param defaultAp Access point id.
     */
    void setAccessPointId(const uint& defaultAp);
    
private slots:
    
    /**
     * Slot for when mLowestUDPPortItem editing finishes.
     */
    void lowestUdpPortEditingFinished();
    
    /**
     * Slot for when mHighestUDPPortItem editing finishes.
     */
    void highestUdpPortEditingFinished();
    
    /**
     * Slot for when mUseProxyItem editing finishes.
     */
    void useProxyToggled(int state);
    
    /**
     * Slot for when mProxyServerItem editing finishes.
     */
    void proxyServerEditingFinished();
    
    /**
     * Slot for when mProxyPortItem editing finishes.
     */
    void proxyPortEditingFinished();
    
    /**
     * Called when item is shown. Connected to the form's
     * itemShown signal.
     */
    void itemShown(const QModelIndex& index);

private:
    
    /**
     * Initializes the mSettingsModel.
     */
    void InitSettingsModelL();
    
    /**
     * Loads all settings data from the mSettingsModel to items.
     */
    void loadSettings();
    
private:
    
    /**
     * Item data helper given by the CP framework.
     */
    CpItemDataHelper& mItemDataHelper;
    
    /**
     * Holds videos translation file. It's needed to localize texts in this plugin.
     */
    QTranslator mTranslator;
    
    /**
     * The settings model where settings are loaded and saved.
     */
    CMPSettingsModel* mSettingsModel;
    
    /**
     * The access point multiselect item.
     */
    VideoSettingsAccessPointEntry *mAccessPointItem;
    
    /**
     * The lowest UDP port text item.
     */
    CpSettingFormItemData *mLowestUDPPortItem;
    
    /**
     * The highest UDP port text item.
     */
    CpSettingFormItemData *mHighestUDPPortItem;
    
    /**
     * The use proxy checkbox item.
     */
    CpSettingFormItemData *mUseProxyItem;
    
    /**
     * The proxy server text item.
     */
    CpSettingFormItemData *mProxyServerItem;
    
    /**
     * The proxy port text item.
     */
    CpSettingFormItemData *mProxyPortItem;
};

#endif // VIDEOSETTINGSGROUP_H
