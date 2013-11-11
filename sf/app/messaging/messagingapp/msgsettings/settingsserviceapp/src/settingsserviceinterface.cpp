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

#include "settingsserviceinterface.h"
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbapplication.h>

#include "msgsettingsview.h"
#include "debugtraces.h"

//----------------------------------------------------------------------------
// SettingsViewInterface::SettingsViewInterface
// @see header
//----------------------------------------------------------------------------
SettingsViewInterface::SettingsViewInterface(HbMainWindow *mainWindow,QObject* parent)
:XQServiceProvider(QLatin1String("messagesettings.com.nokia.symbian.IMessageSettings"),parent),
mMainWindow(mainWindow)
    {
    publishAll();    

    //creating back action.
    mBackAction = new HbAction(Hb::BackNaviAction, this);
    connect(mBackAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    }

//----------------------------------------------------------------------------
// SettingsViewInterface::~SettingsViewInterface
// @see header
//----------------------------------------------------------------------------
SettingsViewInterface::~SettingsViewInterface()
    {
    }

//----------------------------------------------------------------------------
// SettingsViewInterface::launchSettings
// @see header
//----------------------------------------------------------------------------
void SettingsViewInterface::launchSettings(int view)
    {   
    QDEBUG_WRITE("Entered launchSettings")
        MsgSettingsView* settingsView = new MsgSettingsView((MsgSettingsView::SettingsView)view);
        settingsView->setNavigationAction(mBackAction);
        mMainWindow->addView(settingsView);
        
        if(view != MsgSettingsView::SMSView)
        {
            mMainWindow->setCurrentView(settingsView);
        }        
    }

//EOF

