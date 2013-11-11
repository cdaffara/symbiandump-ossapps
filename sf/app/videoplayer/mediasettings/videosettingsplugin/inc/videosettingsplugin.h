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
#ifndef	VIDEOSETTINGSPLUGIN_H
#define	VIDEOSETTINGSPLUGIN_H

#include <qobject.h>
#include <cpplugininterface.h>

class VideoSettingsPlugin : public QObject, public CpPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
    
public:
    
    /**
     * Constructor
     */
    VideoSettingsPlugin();
    
    /**
     * Destructor
     */
    virtual ~VideoSettingsPlugin();
    
    /**
     * Creates the CpSettingFormItemData for video settings to be shown in control panel.
     */
    virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
};

#endif // VIDEOSETTINGSPLUGIN_H
