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
 * Description: Interface for launching settings view as 
 * Qthighway service
 *
 */

#ifndef __SETTINGS_VIEW_INTERFACE_H__
#define __SETTINGS_VIEW_INTERFACE_H__

#include <QObject>
#include <xqserviceprovider.h>

class HbMainWindow;
class HbAction;

class SettingsViewInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /**
     * Constructor
     */
    SettingsViewInterface(HbMainWindow *mainWindow,QObject* parent=0);
    
    /**
     * Destructor
     */
    ~SettingsViewInterface();
    
public slots:
    /**
     * Function which launches the settings view
     * @param view, holds MsgSettingsView::SettingsView enum
     */
    void launchSettings(int view);

private:    
    /**
     * Main Window instance
     * Not owned
     */
    HbMainWindow *mMainWindow;
    
    HbAction* mBackAction;

    };

#endif /* __SETTINGS_VIEW_INTERFACE_H__ */
