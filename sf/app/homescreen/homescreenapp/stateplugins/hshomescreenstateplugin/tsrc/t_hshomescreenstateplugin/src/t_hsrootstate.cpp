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

#include "t_hshomescreenstateplugin.h"
#include "hsrootstate.h"
#include "hsscene.h"

void HomeScreenStatePluginTest::testLoadRootStateConstructors()
{
    HsRootState *rs = new HsRootState;
    delete rs;
}

void HomeScreenStatePluginTest::testLoadRootStateOnEntryExit()
{
    HsRootState *rs = new HsRootState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(rs);
    sm->setInitialState(rs);    
    	
    QFinalState *fs = new QFinalState;
    sm->addState(fs);

    rs->addTransition(this, SIGNAL(finishStateMachine()), fs);

    sm->start();
    QCoreApplication::sendPostedEvents();
    emit finishStateMachine();

    sm->stop();
    delete sm;
}
