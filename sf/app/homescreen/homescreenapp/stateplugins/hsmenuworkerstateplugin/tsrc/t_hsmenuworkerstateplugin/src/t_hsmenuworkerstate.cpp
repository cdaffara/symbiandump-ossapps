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
* Description:  Main test class for hsHomeScreenStateProvider library.
*
*/

#include "t_hsmenuworkerstateplugin.h"
#include "hsaddtohomescreenstate.h"
#include "hsmenuworkerstate.h"
#include "hsmenuevent.h"
#include "hsmenueventfactory.h"
#include "hsmenueventtransition.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::MenuWorkerState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState workerStateS;
        QVERIFY(workerStateS.parent() == 0);
        QVERIFY(workerStateS.objectName() == "homescreen.nokia.com/state/MenuWorkerState");

        HsMenuWorkerState *workerStateH0 = new HsMenuWorkerState();
        QVERIFY(workerStateH0->parent() == 0);
        QVERIFY(workerStateH0->objectName() == "homescreen.nokia.com/state/MenuWorkerState");

        HsMenuWorkerState *workerStateH1 = new HsMenuWorkerState(workerStateH0);
        QVERIFY(workerStateH1->parent() == workerStateH0);


        delete workerStateH1;
        delete workerStateH0;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

