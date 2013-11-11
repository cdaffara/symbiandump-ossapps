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

#ifndef COLLECTIONANAMESTATE_H
#define COLLECTIONANAMESTATE_H

#include <QState>
#include <QTextCursor>

#include "hsaddappstocollectionstate.h"
#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HbAction;
class HsShortcutService;
class HsCollectionNameDialog;

class HsCollectionNameState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsCollectionNameState(QState *parent = 0);

    virtual ~HsCollectionNameState();

signals:

    void commit(const QString &collectionName);

    void commitCheckList(const QString &collectionName);

    void cancel();

    void exit();

private slots:

    void dialogFinished(HbAction* finishedAction);

    void cleanUp();

protected:

    void onEntry(QEvent *event);

private:

    void construct();

private:

    int mItemId;

    HsCollectionNameDialog *mCollectionNameDialog;

};

#endif /* COLLECTIONANAMESTATE_H */
