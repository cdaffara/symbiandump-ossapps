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
* Description: Private declaration of Contact History Model API
*
*/

#ifndef NMCONTACTHISTORYMODELPRIVATE_H
#define NMCONTACTHISTORYMODELPRIVATE_H

#include <QObject>
#include <QSharedData>
#include <qtcontacts.h>

#include "nmcontacthistorymodel.h"

QTM_USE_NAMESPACE

class NmContactHistoryModelItemData : public QSharedData
{
public:

NmContactHistoryModelItemData() :
    mContactId(0xDEAD)
{

}

~NmContactHistoryModelItemData()
{
    mItems.clear();
}

    unsigned int mContactId;
    QList<NmContactHistoryModelSubItem> mItems;
};

class NmContactHistoryModelPrivate : public QObject
{

    enum NameOrder
    {
        LastNameFirstName = 0x0,
        LastNameCommaFirstName = 0x1,
        FirstNameLastName = 0x2
    };

Q_OBJECT

public:

    NmContactHistoryModelPrivate(const NmContactHistoryModelType modelType);
    ~NmContactHistoryModelPrivate();

signals:

    void queryCompleted(int err);

private:

    void queryDatabases(const QString &query);

    // Helper functions that are used by queryDatabases();
    bool queryMruDatabase(const QString &query);
    QList<QContactLocalId> queryContactDatabase(const QString &query);

    void populateListWithMruItems(const QString &query);

    void populateListWithContactItems(const QList<QContactLocalId> cnt_ids,
                                      const QString &query);


    int modelRowCount(const QModelIndex &parent) const;
    int privateDataCount() const;
    QVariant data(const QModelIndex &index, int role) const;
    void refreshDataModel();

    bool fillMruMatchList();
    QString obeyContactOrder(const QString &firstName, const QString &lastName) const;

private:

    // Member variables
    NmContactHistoryModelType mType;
    QList<NmContactHistoryModelItem> mModelItemList;
    QList< QSharedPointer<NmContactHistoryModelItemData> > mPrivateItemList;
    QContactManager *mContactManager;
    QHash<QString, QString> mMruList;
    QMap<QString, QString> mMruMatches;
    bool mModelReady;
    TUint32 mNameOrder;

private:  // Friend definitions

    friend class NmContactHistoryModel;
    friend class TestNmContactHistoryModelPrivate;
};

#endif //NMCONTACTHISTORYMODELPRIVATE_H
