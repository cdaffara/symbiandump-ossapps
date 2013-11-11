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
    /**
     * initialize
     */
    void initTestCase();
    void init();

    /*
    * Tests
    */
    void construction();
    void listItemLongPressed();
    void contextMenuAction();
    void listItemActivated();
    void createNewCollection();
    void createArrangeCollection();
#ifdef Q_OS_SYMBIAN
    void openTaskSwitcher();
#endif//Q_OS_SYMBIAN
    void addModeSlots();
    void addModeEnteredExited();
    void normalModeEnteredExited();

    /**
     * cleanup
     */
    void cleanup();

private:
    HbMainWindow *mWindow;
};
#endif // T_ALLCOLLECTIONSSTATE_H
