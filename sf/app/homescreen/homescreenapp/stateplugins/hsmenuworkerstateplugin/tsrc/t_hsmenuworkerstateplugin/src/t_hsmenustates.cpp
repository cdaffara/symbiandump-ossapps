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

#include <QStateMachine>

#include <HbDialog>
#include <HbMainWindow>
#include <hbmainwindow.h>

#include "t_hsmenuworkerstateplugin.h"

#include "hsaddtohomescreenstate.h"
#include "hsshortcutservice.h"
#include "hsdialogcontroller.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::initTestCase()
{
#ifdef Q_OS_SYMBIAN
    startThread();
    QDir::setCurrent("C:/");
#endif //Q_OS_SYMBIAN
    mStateMachine = new QStateMachine;
    mShortcutService = QSharedPointer<HsShortcutService>
                       (HsShortcutService::instance(mStateMachine/*,0*/));
    mWindow = new HbMainWindow();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::cleanupTestCase()
{
#ifdef Q_OS_SYMBIAN
    stopThread();
#endif //Q_OS_SYMBIAN
    delete mStateMachine;
    //QWARN ( "Due to bug in hb wk36 we are we have to avoid deleting "
    //        "HbMainWindow, remove comment when fixed" )    
/*    delete mWindow;
    mWindow = NULL;*/
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::cleanup()
{
    qApp->processEvents();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
int threadFunction(void *params)
{
    Q_UNUSED(params);
#ifdef Q_OS_SYMBIAN
    while (ETrue) {
        User::ResetInactivityTime();//it should help for Viewserver11 panic
        User::After(5000000);
    }
#endif //Q_OS_SYMBIAN
    return 0;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::startThread()
{
#ifdef Q_OS_SYMBIAN
    User::LeaveIfError(iThread.Create(
                           _L("thread_kill_viewsrv11"),
                           threadFunction,
                           16384,
                           4000,
                           4000,
                           NULL));
    iThread.Resume();
#endif //Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::stopThread()
{
#ifdef Q_OS_SYMBIAN
    iThread.Close();
    QTest::qWait(3000);
#endif //Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::testSlot(HbAction* finished)
{
    Q_UNUSED(finished)
    mFinishedEmited = true;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
int MenuStatesTest::testExitSlot()
{
    mFinishedEmited = true;
    return 0;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::checkDialogController()
{

    QGraphicsScene *scene = mWindow->scene();
    QGraphicsItem *focusItem = scene->focusItem();
    QPointer<HbDialog> dialog =
        qobject_cast<HbDialog*>(focusItem->toGraphicsObject());

    QVERIFY(!dialog.isNull());

    QPointer<HsDialogController> controller(NULL);

    foreach (QObject* obj, dialog->children())
    {
        controller = qobject_cast<HsDialogController*>(obj);

        if (!controller.isNull())
        {
            break;
        }
    }

    QVERIFY(!controller.isNull());

    QSignalSpy spy(controller.data(), SIGNAL(dialogCompleted()));
    controller->dismissDialog();

    QTest::qWait(1000);

    QVERIFY(dialog.isNull());
    QVERIFY(controller.isNull());
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(MenuStatesTest)
