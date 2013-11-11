/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <HbInstance>
#include <HbMainWindow>

#include "t_hshomescreenstateplugin.h"
#include "hsbackuprestorestate.h"
#include "hsscene.h"
#include "hsdatabase.h"
#include "hsgui.h"

void HomeScreenStatePluginTest::testLoadBackupRestoreStateConstructors()
{
    HsBackupRestoreState *brs = new HsBackupRestoreState;
    delete brs;
}

void HomeScreenStatePluginTest::testLoadBackupRestoreStateOnEntryExit()
{
    HbInstance::instance();
    HbMainWindow mainWindow;
    mainWindow.show();   
    QCoreApplication::sendPostedEvents();
   
    QStateMachine *sm = new QStateMachine;
    HsBackupRestoreState *brs = new HsBackupRestoreState;

    sm->addState(brs);
    sm->setInitialState(brs);    
    

    QFinalState *fs = new QFinalState;
    sm->addState(fs);

    brs->addTransition(this, SIGNAL(finishStateMachine()), fs);

    sm->start();   
    QCoreApplication::sendPostedEvents();  
    emit finishStateMachine();

    sm->stop();
    // main window deleted -> HsGui must be deleted also
    delete HsGui::takeInstance();
    delete sm;
}
