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

#include<qscopedpointer.h>
#include<hbmessagebox.h>
#include<hbaction.h>
#include <HbParameterLengthLimiter>
#include <HbLabel>

#include "hsinstallationlogstate.h"
#include "t_hsmenuworkerstateplugin.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenuevent.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsInstallationLogState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        parentState1.setObjectName(tr("testName1"));

        HsInstallationLogState logState(&parentState1);
        QString s1 = logState.objectName();
        QVERIFY(s1 == tr("testName1/InstallationLogState"));
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
void MenuStatesTest::HsInstallationLogState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    QState parentState;
    HsInstallationLogState logState(&parentState);

    //test event
    QScopedPointer<HsMenuEvent> event(new HsMenuEvent(HsMenuEvent::ShowInstallationLog));

    logState.onEntry(event.data());

    checkDialogController();
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
