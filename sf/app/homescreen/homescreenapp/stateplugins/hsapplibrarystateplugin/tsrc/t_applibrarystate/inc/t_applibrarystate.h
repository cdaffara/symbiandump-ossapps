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
* Description:  Tests for hsapplibrarystateplugin.
*
*/

#ifndef T_ALLCOLLECTIONSSTATE_H
#define T_ALLCOLLECTIONSSTATE_H

#include <QtTest/QtTest>

class HbMainWindow;
class HbAbstractViewItem;
class QStateMachine;

/**
* @test Test class for homescreen::hsutils modules.
*/
class MenuStatesTest : public QObject
{
    Q_OBJECT

private slots:

    /*
    * Tests
    */
    // ApplicationLibraryState
    void ApplicationLibraryState_construction();
    void ApplicationLibraryState_backSteppingAction();
    void ApplicationLibraryState_clearToolbarLatch();
    void ApplicationLibraryState_historySlots();

    //OviStore
#ifdef Q_OS_SYMBIAN
#ifdef QT_EXTENSIONS
    void ApplicationLibraryState_oviStoreAction();
    void OperatorHandler_icon();
    void OperatorHandler_text();
    void OperatorHandler_action();
    void OperatorHandler_entry();
    void OperatorHandler_noCrWriteAccess();
    void OperatorHandler_storesPresent();
    void OperatorHandler_first();
    void OperatorHandler_type();
    void OperatorHandler_createAction();
#endif//NO_QT_EXTENSIONS
#endif//Q_OS_SYMBIAN

    //HsBaseViewState
    void HsBaseViewState_createApplicationLaunchFailMessage();

    void cleanup();
};
#endif // T_ALLCOLLECTIONSSTATE_H
