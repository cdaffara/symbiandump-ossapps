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
 * Description:This is the first view for msgsettings plugin  
 *
 */

#ifndef MSGSETTINGSVIEW_H
#define MSGSETTINGSVIEW_H

#ifdef  SETTINGSVIEW_DLL
#define MSG_SETTING_VIEW_EXPORT Q_DECL_EXPORT
#else
#define MSG_SETTING_VIEW_EXPORT Q_DECL_IMPORT
#endif

#include "msgbaseview.h"

class HbMainWindow;
class HbAction;
class MsgSMSCenterView;
class MsgSettingsForm;

class MSG_SETTING_VIEW_EXPORT MsgSettingsView: public MsgBaseView
{
    Q_OBJECT
public:
    enum SettingsView
    {
    DefaultView = 0,
    SMSView,
    MMSView
    };
    
public:
    MsgSettingsView(
            SettingsView settingsView = MsgSettingsView::DefaultView,
            QGraphicsItem *parent = 0);
    
    ~MsgSettingsView();

public slots:
    void onNewSMSCCenterClicked(int index);
    void onSmsCenterEditViewClosed();
        
private:    
    //Sms Center View
    MsgSMSCenterView* mSMSCenterView;  
    
    //Settings Form
    MsgSettingsForm* mSettingsForm;
    
    //main window.
    HbMainWindow* mMainWindow;
    
    //current settings view.
    SettingsView mCurrentView;
};
#endif // MSGSETTINGSVIEW_H
