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
 * Description: Menu All Collections state.
 *
 */

#ifndef HSALLCOLLECTIONSSTATE_H
#define HSALLCOLLECTIONSSTATE_H

#include "hsbaseviewstate.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class QPointF;
class HsMenuViewBuilder;
class HbAbstractViewItem;
class HbAction;
class HbMainWindow;
class HsMenuItemModel;
class HsMenuView;
class HsMainWindow;
class HsMenuModeWrapper;

class HsAllCollectionsState: public HsBaseViewState
{
    Q_OBJECT
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
public:
    HsAllCollectionsState(HsMenuViewBuilder &menuViewBuilder,
                          HsMenuModeWrapper &menuMode,
                          HsMainWindow &mainWindow,
                          QState *parent = 0);
    ~HsAllCollectionsState();
protected:
signals:
    void toAppLibraryState();

private slots:
    void createNewCollection();
    void createArrangeCollection();
    void normalModeEntered();
    void normalModeExited();
    void stateExited();
    void updateLabel();
private:
    void construct();
    void setMenuOptions();
    void setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags);
};

#endif // HSALLCOLLECTIONSSTATE_H
