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
#include <HbMainWindow>

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
    void construction();
#ifdef Q_OS_SYMBIAN
    void openTaskSwitcher();
    void openInstallationLog();
#endif//Q_OS_SYMBIAN
    void emptyLabelVisibility();
    void stateEnteredExited();
    void listItemLongPressed();
    void latestOldestOnTopMenuAction();
    /**
     * cleanup
     */

    void cleanup();
    
private:
    HbMainWindow mWindow;
};
#endif // T_ALLAPPSSTATE_H
