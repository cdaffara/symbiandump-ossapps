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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/

#include<hbmessagebox.h>
#include<hbaction.h>

#include "hsuinstallfailedstate.h"
#include "t_hsmenuworkerstateplugin.h"
#include "hscontentservice.h"
#include "hsmenueventfactory.h"
#include "hsmenuevent.h"
#include "hsmenuitemmodel.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsUninstallFailedState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        parentState1.setObjectName(tr("testName1"));

        HsUninstallFailedState uni1(&parentState1);
        QString s1 = uni1.objectName();
        QVERIFY(s1 == tr("testName1/HsUninstallFailedState"));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsUninstallFailedState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    QState parentState;
    HsUninstallFailedState failedState(&parentState);
    
    int fakeError = 3;
    QVariantMap params;
    params.insert(Hs::uninstallError, fakeError);

    //test event
    QScopedPointer<HsMenuEvent> event(new HsMenuEvent(HsMenuEvent::UninstallationFailed, params));

    failedState.onEntry(event.data());

    checkDialogController();
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

