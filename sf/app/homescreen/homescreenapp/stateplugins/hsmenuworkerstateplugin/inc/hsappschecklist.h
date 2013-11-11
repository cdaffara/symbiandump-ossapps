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

#ifndef APPSCHECKLIST_H
#define APPSCHECKLIST_H

#include <QModelIndexList>
#include <hsmenuservice.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

//forward declarations
class HbDialog;
class HbAction;
class HbListView;
class QStandardItemModel;
class HsMenuItemModel;

class HsAppsCheckList: public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsAppsCheckList();

    virtual ~HsAppsCheckList();

    void cleanUp();

    void setSortOrder(Hs::HsSortAttribute sortAttribute);

    void showAppsCheckboxList(Hs::HsSortAttribute sortAttribute);

signals:

    void commit(const QList<int> &appList);

protected slots:

    void appsSelectionDialogFinished(HbAction* finishedAction);

    void selectedItemsChanged();

private:

    void constructControls();

    QList<int> getSortedItemsList(const QModelIndexList &modelIndexList);

private:

    HbDialog *mAppsSelectDialog;

    HbAction *mActionConfirm; // delete with dialog

    HbListView *mListView; // delete with dialog

    HsMenuItemModel *mModel; // delete with dialog

    Hs::HsSortAttribute mSortAttribute;

};

#endif /* APPSCHECKLIST_H */
