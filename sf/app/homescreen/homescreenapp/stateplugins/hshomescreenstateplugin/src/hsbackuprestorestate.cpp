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
#include <QScopedPointer>
#include <HbMainWindow>
#include <HbView>
#include <HbAction>
#include "hsdatabase.h"
#include "hsscene.h"
#include "hsbackuprestoreobserver.h"
#include "hsbackuprestorestate.h"
#include "hsscene.h"
#include "hsgui.h"


/*!
    \class HsBackupRestoreState
    \ingroup group_hshomescreenstateplugin
    \brief Home screen Bacup/Restore state.

    Releases all Home screen data, which will be backed up or restored.
*/

/*!
    Constructs a new backup/restore state with the given \a parent.
*/
HsBackupRestoreState::HsBackupRestoreState(QState *parent)
    : QState(parent)      
{	
    connect(this, SIGNAL(entered()), SLOT(action_startListenBURStatus()));
    connect(this, SIGNAL(exited()), SLOT(action_stopListenBURStatus()));
}

/*!
    Destroys this backup/restore state.
*/
HsBackupRestoreState::~HsBackupRestoreState()
{
}

/*!
    Starts listen backup/restore status and releases Home screen data.
*/
void HsBackupRestoreState::action_startListenBURStatus()
{
    HsBackupRestoreObserver *backupRestoreObserver = 
        HsBackupRestoreObserver::instance();
    
    connect(backupRestoreObserver, 
            SIGNAL(event_backupRestoreCompleted()),
            SIGNAL(event_loadScene()));
    
    // Close database
    mDatabase.reset(HsDatabase::takeInstance());
    mDatabase->close();        
    
    deleteIdleView();        

    // Delete scene
    HsScene::setInstance(0);
    
    backupRestoreObserver->readyForBUR();
}

/*!
    Stops listen backup/restore status.
*/
void HsBackupRestoreState::action_stopListenBURStatus()
{
    HsBackupRestoreObserver::instance()->disconnect(this);    
    
    // Open database
    HsDatabase::setInstance(mDatabase.data());
    mDatabase.take();
    HsDatabase::instance()->open();
}

/*!
   Deletes idle view
*/
void HsBackupRestoreState::deleteIdleView()
{
    // Delete idle view
    HsGui::instance()->cleanupIdleUi();
}
