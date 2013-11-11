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

#ifndef HSDELETECOLLECTIONITEMSTATE_H
#define HSDELETECOLLECTIONITEMSTATE_H

#include <QState>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class QAction;
class HbAction;
class HsShortcutService;
class HsMenuService;

class HsDeleteCollectionItemState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsDeleteCollectionItemState(QState *parent = 0);

    virtual ~HsDeleteCollectionItemState();

protected:

    void onEntry(QEvent *event);

private slots:
    void removeApplicationFromCollection();
    void cleanUp();

signals:

    void exit();

private:

    void construct();

private:

    int mItemId;

    int mCollectionId;
};

#endif //HSDELETECOLLECTIONITEMSTATE_H
