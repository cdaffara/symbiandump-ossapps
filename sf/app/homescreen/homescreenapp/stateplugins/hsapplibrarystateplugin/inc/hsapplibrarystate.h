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
 * Description: Menu Application Library state.
 *
 */

#ifndef HSAPPLIBRARYSTATE_H
#define HSAPPLIBRARYSTATE_H

#include <QState>
#include <hbdocumentloader.h>
#include "hsmenustates_global.h"
#include "hsmenuview.h"
#include "hsmainwindow.h"
#include "hsmenumodewrapper.h"
#include "hsapp_defs.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HbAction;
class HbView;
class HbMainWindow;
class QActionGroup;

class HsAllAppsState;
class HsAllCollectionsState;
class HsCollectionState;
class HsInstalledAppsState;
class HsMenuModeTransition;
class HsMenuModeWrapper;


class HsAppLibraryState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsAppLibraryState(QState *parent = 0);

    virtual ~HsAppLibraryState();

signals:

    void collectionEntered();

    void toHomescreenState();

    void initialize();

public slots:

    void clearToolbarLatch();

    int oviStoreAction();

    void allAppsStateEntered();

    void allCollectionsStateEntered();
    
    void collectionStateEntered();

protected:

    void onEntry(QEvent *event);

private:

    void construct();

    void constructToolbar();

private:

    HsMenuViewBuilder mMenuViewBuilder;

    HsAllAppsState *mAllAppsState;

    HsMenuModeTransition *mHistoryTransaction;

    HsAllCollectionsState *mAllCollectionsState;

    HsCollectionState *mCollectionState;

    HsInstalledAppsState *mInstalledAppsState;

    HsMenuModeWrapper mMenuMode;
    
    HsMainWindow mMainWindow;
    
};

#endif //HSAPPLIBRARYSTATE_H
