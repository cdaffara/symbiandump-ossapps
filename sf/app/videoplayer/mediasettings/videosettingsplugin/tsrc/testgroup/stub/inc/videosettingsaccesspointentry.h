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
 * Description: VideoSettingsAccessPointEntry stub
 *
 */
#ifndef VIDEOSETTINGSACCESSPOINTENTRY_H
#define VIDEOSETTINGSACCESSPOINTENTRY_H

#include <cpsettingformitemdata.h>

class VideoSettingsGroup;

class VideoSettingsAccessPointEntry : public CpSettingFormItemData
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
    
public:
    
    QString mText;
    
    VideoSettingsGroup* mParent;
    
    uint mApId;
    
};

#endif // VIDEOSETTINGSACCESSPOINTENTRY_H
