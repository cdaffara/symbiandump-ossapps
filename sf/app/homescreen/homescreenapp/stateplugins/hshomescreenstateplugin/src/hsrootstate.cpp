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

#include <HbMainWindow>
#include "hsbackuprestoreobserver.h"
#include "hsrootstate.h"

/*!
    \class HsRootState
    \ingroup group_hshomescreenstateplugin
    \brief View part of the home screen idle state.

    Loads the home screen scene.
*/

/*!
    Constructs a new load scene state with the given \a parent.
*/
HsRootState::HsRootState(QState *parent)
    : QState(parent)
{	
    connect(this, SIGNAL(entered()), SLOT(action_startListenBURStatus()));
    connect(this, SIGNAL(exited()), SLOT(action_stopListenBURStatus()));
}

/*!
    Destroys this idle state.
*/
HsRootState::~HsRootState()
{
}

/*!
    Starts listen backup/restore status.
*/
void HsRootState::action_startListenBURStatus()
{
    HsBackupRestoreObserver *backupRestoreObserver = 
        HsBackupRestoreObserver::instance();
    
    connect(backupRestoreObserver, 
            SIGNAL(event_backupRestoreStarted()),
            SIGNAL(event_backupRestore()));
}

/*!
    Stops listen backup/restore status.
*/
void HsRootState::action_stopListenBURStatus()
{
    HsBackupRestoreObserver *backupRestoreObserver = 
        HsBackupRestoreObserver::instance();
                
    backupRestoreObserver->disconnect(this);    
}
