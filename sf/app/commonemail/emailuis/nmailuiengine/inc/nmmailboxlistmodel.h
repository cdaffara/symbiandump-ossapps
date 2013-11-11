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

#ifndef NMMAILBOXLISTMODEL_H_
#define NMMAILBOXLISTMODEL_H_

#include <QStandardItemModel>

#include "nmuienginedef.h"
#include "nmcommon.h"

class NmMailbox;
class NmDataManager;

class NmMailboxListModelItem;

class NMUIENGINE_EXPORT NmMailboxListModel : public QStandardItemModel
{
    Q_OBJECT
public:

    NmMailboxListModel(NmDataManager &dataManager, QObject *parent = 0);
    virtual ~NmMailboxListModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void refresh(QList<NmMailbox*> &mailboxList);
    void refreshModelItem(const NmId &mailboxId);

public slots:
    void handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds);

private:
    NmMailboxListModelItem *createMailboxItem(const NmMailbox *mailbox);

private:
    NmDataManager &mDataManager;    // not owned
};

#endif /* NMMAILBOXLISTMODEL_H_ */
