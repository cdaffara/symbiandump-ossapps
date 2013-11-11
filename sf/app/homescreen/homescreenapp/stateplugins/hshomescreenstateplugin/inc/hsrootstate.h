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

#ifndef HSROOTSTATE_H
#define HSROOTSTATE_H

#include <QState>
#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)


class HsRootState : public QState
{
    Q_OBJECT

public:
    HsRootState(QState *parent = 0);
    ~HsRootState();

signals:    
    void event_backupRestore();

private slots:    
    void action_startListenBURStatus();
    void action_stopListenBURStatus();

private:
    Q_DISABLE_COPY(HsRootState)

private:	
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSROOTSTATE_H
