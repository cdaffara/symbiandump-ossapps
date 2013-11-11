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

#ifndef T_MENUVIEWBUILDER_H
#define T_MENUVIEWBUILDER_H

#include <QtTest/QtTest>
#include <QPoint>

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
    void HsSearchViewBuilder_construction();
    void HsSearchViewBuilder_uiObjects();
    void HsSearchViewBuilder_setSearchLabledContext();
    void HsSearchViewBuilder_loadViewEmptySection();
    void HsSearchViewBuilder_loadViewListSection();

    void HsSearchViewBuilder_searchViewConstruction();
    void HsSearchViewBuilder_activatedProxySlot();
    void HsSearchViewBuilder_longPressedProxySlot();
    void HsSearchViewBuilder_showHide();
    void HsSearchViewBuilder_hideVkb();


    void cleanup();

    void testSlot();

private:
    bool mSlotActivated;

};
#endif // T_MENUVIEWBUILDER_H
