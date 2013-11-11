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
 * Description: Menu collection state.
 *
 */

#ifndef HSCOLLECTIONSTATE_H
#define HSCOLLECTIONSTATE_H

#include <QState>
#include <QAbstractItemModel>

#include "hsbaseviewstate.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAction;
class HbMenu;
class HbAbstractViewItem;
class HsMenuItemModel;
class HsMenuModeWrapper;
class HsMenuViewBuilder;
class HsMainWindow;

class HsCollectionState: public HsBaseViewState
{
    Q_OBJECT
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
public:
    HsCollectionState(HsMenuViewBuilder &menuView,
                      HsMenuModeWrapper &menuMode,
                      HsMainWindow &mainWindow,
                      QState *parent = 0);
    ~HsCollectionState();

protected:
    void onEntry(QEvent *event);

private slots:
    void addAppsAction();
    void addCollectionShortcutToHomeScreenAction();
    void renameAction();
    void deleteAction();
    void updateLabel();
    void stateEntered();

    void stateExited();
    void latestOnTopMenuAction();
    void oldestOnTopMenuAction();
    void contextMenuAction(HbAction *action);
    void handleEmptyChange(bool empty);
    void lockSearchButton(bool lock);
    void createArrangeCollection();
    void openInstalledViewFromCollection();

private:
    void construct();
    void constructMenu(bool isDynamic);
    void makeConnect();
    void makeDisconnect();
    void setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags);
    void setMenuOptions();
    void reconnectViewComponents(bool empty);

private:
    Hs::HsSortAttribute mSortAttribute;
    int mCollectionId;
    QString mCollectionType;
    HbAction *mLatestOnTopMenuAction; //not own
    HbAction *mOldestOnTopMenuAction; //not own
};

#endif // HSCOLLECTIONSTATE_H
