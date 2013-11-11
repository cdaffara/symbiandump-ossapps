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

#ifndef T_ALLAPPSSTATE_H
#define T_ALLAPPSSTATE_H

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
    //void initTestCase();
    //void cleanup();
    //void init();
    /*
    * Tests
    */
    //HsAllAppsState
    void construction();
    void addToCollection();
    void addToHomeScreen();
#ifdef Q_OS_SYMBIAN
    void openTaskSwitcher();
#endif//Q_OS_SYMBIAN
    void listItemLongPressed();
    void contextMenuAction();
    void sortOrder();
    void listItemActivated();
    void addModeAddActivated();
    void addModeShowContextMenu();
    void addModeEnteredExited();
#ifdef Q_OS_SYMBIAN
    void checkSoftwareUpdates();
#endif//Q_OS_SYMBIAN
    //HsAddModeProxyModel
    void addModeProxyModel_construction();
    void addModeProxyModel_filterAcceptsRow();
    
private:
    HbMainWindow* mMainWindow;
    static const char TEST_PARAM_NAME[17];
    static const char TEST_PARAM_VALUE[17];
};

#endif // T_ALLAPPSSTATE_H
