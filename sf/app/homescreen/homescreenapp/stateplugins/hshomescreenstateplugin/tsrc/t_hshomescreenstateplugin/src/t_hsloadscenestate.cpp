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
* Description:
*
*/

#include <QStateMachine>
#ifdef Q_OS_SYMBIAN
#include <XQSettingsManager>
#include <startupdomainpskeys.h>
#endif
#include <HbInstance>
#include <HbMainWindow>

#include "t_hshomescreenstateplugin.h"
#include "hsloadscenestate.h"
#include "hsscene.h"
#include "hsgui.h"

void HomeScreenStatePluginTest::testLoadSceneStateConstructors()
{
    HsLoadSceneState *lss = new HsLoadSceneState;
    delete lss;
}

void HomeScreenStatePluginTest::testLoadSceneStateOnEntryExit()
{
    HbInstance::instance();
    HbMainWindow mainWindow;
    mainWindow.show();

    HsLoadSceneState *lss = new HsLoadSceneState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(lss);
    sm->setInitialState(lss);
    sm->start();
    QCoreApplication::sendPostedEvents();
    // main window deleted -> HsGui must be deleted also
    delete HsGui::takeInstance();
    delete sm;
}

#ifdef Q_OS_SYMBIAN
void HomeScreenStatePluginTest::testLoadSceneStateHandleKeyChange()
{
    HbInstance::instance();
    HbMainWindow mainWindow;
    mainWindow.show();
    QCoreApplication::sendPostedEvents();
    
    HsLoadSceneState *lss = new HsLoadSceneState;
    XQSettingsKey testKey(XQSettingsKey::TargetPublishAndSubscribe, KPSUidStartup.iUid, KPSStartupUiPhase);
    QVariant variant(QVariant::Int);
    lss->handleKeyChange(testKey, variant);
    int value = EStartupUiPhaseAllDone;
    variant.setValue(value);
    lss->handleKeyChange(testKey, variant);

    // main window deleted -> HsGui must be deleted also
    delete HsGui::takeInstance();
    delete lss;
}
#endif
