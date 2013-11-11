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

#ifndef HSBACKUPRESTORESTATE_H
#define HSBACKUPRESTORESTATE_H

#include <QState>
#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HsDatabase;

class HsBackupRestoreState : public QState
{
    Q_OBJECT

public:
    HsBackupRestoreState(QState *parent = 0);
    ~HsBackupRestoreState();

signals:
	void event_loadScene(); 

private slots:    
    void action_startListenBURStatus();    
    void action_stopListenBURStatus();

private:
    void deleteIdleView();
    Q_DISABLE_COPY(HsBackupRestoreState)

private:	
	QScopedPointer<HsDatabase> mDatabase;
    
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSBACKUPRESTORESTATE_H
