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
 * Description:
 *
 */

#ifndef HSARRANGESTATE_H
#define HSARRANGESTATE_H

#include <QState>
#include <QVariant>
#include <hsmenuservice.h>
#include <QAbstractItemModel>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HbListWidget;
class HsMenuItemModel;
class HbDialog;
class HbAction;

class HsArrangeState: public QState
{
    Q_OBJECT

    // Friend classes
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsArrangeState(QState *parent = 0);

    virtual ~HsArrangeState();

protected:

    void onEntry(QEvent *event);

private slots:

    void stateExited();

    void arrangeDialogFinished(HbAction* finishedAction);

signals:
    void exit();

private:

    void construct();

    void fulfillEntriesList(HbListWidget& listWidget);

    void getArrangedEntriesIds(const HbListWidget& listWidget);

    void save(const HbListWidget& listWidget);

private:

    HbDialog *mDialog;

    HbListWidget *mEntriesList;

    HsMenuItemModel *mItemModel;

    int mTopItemId;
    
    int mCollectionId;

    QModelIndex mTopModelIndex;

    QList<int> mCollItemIdList;

    QList<int> mArrangedCollItemIdList;
    
    QObjectList mObjectList;

};

#endif /* HSARRANGESTATE_H */
