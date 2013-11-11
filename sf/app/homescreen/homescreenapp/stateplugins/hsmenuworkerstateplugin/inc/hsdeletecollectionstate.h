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

#ifndef HSDELETECOLLECTIONSTATE_H
#define HSDELETECOLLECTIONSTATE_H

#include <QState>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class QAction;
class HbAction;
class HbMessageBox;
class HsMenuService;

class HsDeleteCollectionState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsDeleteCollectionState(QState *parent = 0);

    virtual ~HsDeleteCollectionState();

protected:

    void onEntry(QEvent *event);

private slots:

    void deleteCollection();

    void cleanUp();

signals:

    void exit();

private:

    void construct();

private:
    int mItemId;
};

#endif //HSDELETECOLLECTIONSTATE_H
