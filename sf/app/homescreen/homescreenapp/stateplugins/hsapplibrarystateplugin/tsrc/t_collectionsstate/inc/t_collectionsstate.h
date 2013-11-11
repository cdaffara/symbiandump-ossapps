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

#ifndef T_COLLECTIONSSTATE_H
#define T_COLLECTIONSSTATE_H

#include <QtTest/QtTest>
#include <HbMainWindow>

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
    void updateLabel();

    void contextMenuConstruct();
    void contextMenuConstructNonEmptyCollection();
    void addAppsAction();
    void renameAction();
    void deleteAppsAction();
    void addElementToHomeScreen();
    void addCollectionShortcutToHomeScreenAction();
    void latestOnTopMenuAction();
    void oldestOnTopMenuAction();
    void createArrangeCollection();
#ifdef Q_OS_SYMBIAN
    void openTaskSwitcher();
#endif//Q_OS_SYMBIAN
    void listItemActivated();
    void listItemLongPressed();
    void contextMenuAction();
    void disableSearchAction();


    void init();
    /**
     * cleanup
     */
    void cleanup();
private:
    HbMainWindow *mWindow;
};
#endif // T_COLLECTIONSSTATE_H
