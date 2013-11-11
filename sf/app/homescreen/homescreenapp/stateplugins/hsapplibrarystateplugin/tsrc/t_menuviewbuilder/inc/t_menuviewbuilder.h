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
    void HsMenuViewBuilder_construction();
    void HsMenuViewBuilder_view();
    void HsMenuViewBuilder_listView();
    void HsMenuViewBuilder_label();
    void HsMenuViewBuilder_allAppsAction();
    void HsMenuViewBuilder_allCollectionsAction();
    void HsMenuViewBuilder_searchAction();
    void HsMenuViewBuilder_oviStoreAction();
    void HsMenuViewBuilder_operatorAction();
    void HsMenuViewBuilder_toolBar();
    void HsMenuViewBuilder_toolBarExtension();

    //Menu view
    void MenuView_toolBarExtension();
    void HsMenuView_scrolling();
    void MenuView_slots();
    void MenuView_reset();
    void MenuView_showHideSearchPanel();
    // HsListViewItem
    void HsListViewItem_updateChildItems();
    void HsListViewItem_createItem();
    void HsListViewItem_polish();
    void HsProgressBar_timerEvent();
    void HsProgressBar_setTargetProgressValue();

    void cleanup();

    /**
     * special slots for  tests
     */
    void testslot_activated(const QModelIndex &aIndex);
    void testslot_longpressed(HbAbstractViewItem *item,
                              const QPointF &coords);
private:

    HbAbstractViewItem *mItem;

    QModelIndex mIndex;
    QPointF mPoint;
};
#endif // T_MENUVIEWBUILDER_H
