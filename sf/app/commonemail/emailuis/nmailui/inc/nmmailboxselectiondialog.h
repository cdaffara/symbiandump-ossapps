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
* Description:
*
*/

#ifndef NMMAILBOXSELECTIONDIALOG_H_
#define NMMAILBOXSELECTIONDIALOG_H_

#include <QObject>
#include <QAbstractItemModel> // For QModelIndex.
#include <nmcommon.h>

class HbDialog;
class HbListView;
class HbAction;
class QGraphicsItem;
class QStandardItemModel;
class NmMailboxListModel;
class NmMailboxMetaData;


class NmMailboxSelectionDialog : public QObject
{
    Q_OBJECT

public:

    NmMailboxSelectionDialog(NmMailboxListModel &mailboxListModel,
                             QGraphicsItem *parent = 0);
    virtual ~NmMailboxSelectionDialog();
    void open();

private slots:

    void itemActivated(QModelIndex index);
    void dialogClosed(HbAction *action);

signals:

    void selectionDialogClosed(NmId &mailboxId);

private:

    bool initializeUi();
    bool populateListItems();
    NmMailboxMetaData *mailboxMetaData(int index) const;

private:

    HbListView *mMailboxListView; // Content view, owned
    HbDialog *mMailboxSelectionDialog; // The dialog, owned
    QStandardItemModel *mContentItemModel; // Item model for list view content, owned
    NmMailboxListModel &mMailboxListModel; // Mailbox list model
    QGraphicsItem *mParent; // The parent for this dialog.
    NmId mMailboxId; // Selected mailbox ID
    QObjectList mWidgetList;
};


#endif /* NMMAILBOXSELECTIONDIALOG_H_ */

// End of file.
