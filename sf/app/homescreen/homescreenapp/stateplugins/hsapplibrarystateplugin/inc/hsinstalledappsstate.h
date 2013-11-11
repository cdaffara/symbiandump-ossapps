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
 * Description: Menu Installed state.
 *
 */

#ifndef HSINSTALLEDAPPSSTATE_H
#define HSINSTALLEDAPPSSTATE_H

#include <QState>

#include "hsbaseviewstate.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbView;
class HbAction;
class HbMenu;
class HbAbstractViewItem;
class QPointF;
class HsMenuViewBuilder;
class HsMenuItemModel;
class HsMainWindow;

class HsInstalledAppsState: public HsBaseViewState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsInstalledAppsState(HsMenuViewBuilder &menuViewBuilder,
                         HsMainWindow &mainWindow,
                         QState *parent = 0);

    ~HsInstalledAppsState();
    
protected:
    void onEntry(QEvent *event);

private slots:

    void backFromInstalledView();
    void openInstallationLog();
    void stateEntered();
    void stateExited();
    void setEmptyLabelVisibility(bool visibility);
    void updateLabel();
    void latestOnTopMenuAction();
    void oldestOnTopMenuAction();
    void launchDetails(const QModelIndex &index);
    
private:

    void construct();
    void setMenuOptions();
    void setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags);
    Hs::HsSortAttribute mSortAttribute;
    HbAction *mLatestOnTopMenuAction; //not own
    HbAction *mOldestOnTopMenuAction; //not own
    int mCollectionId; // An id of a collection to which the state machine will return.
    QString mCollectionType; // A type of a collection to which the state machine will return.
};

#endif // HSINSTALLEDAPPSSTATE_H
