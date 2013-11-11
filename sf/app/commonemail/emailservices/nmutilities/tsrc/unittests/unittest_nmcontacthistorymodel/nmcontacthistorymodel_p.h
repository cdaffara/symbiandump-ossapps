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

#include "nmcontacthistorymodel.h"
#include "nmcontacthistorymodelcommon.h"

class NmContactHistoryModelItemData : public QSharedData
{
public:

NmContactHistoryModelItemData() :
    mContactId(0xDEAD)
{

}

~NmContactHistoryModelItemData()
{
}

    unsigned int mContactId;
};

class NmContactHistoryModelPrivate : public QObject
{
    enum nameOrder
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
    int modelRowCount(const QModelIndex &parent) const;
    int privateDataCount() const;
    QVariant data(const QModelIndex &index, int role) const;
    void refreshDataModel();

protected:
    void connectNotify ( const char * signal );

public:

    // Member variables
    NmContactHistoryModelType mType;
    QList< QString > mPrivateItemList;

    bool mSignalConnected;

private:  // Friend definitions

    friend class NmContactHistoryModel;
    friend class TestNmContactHistoryModel;
    friend class TestNmContactHistoryModelPrivate;
};

#endif //NMCONTACTHISTORYMODELPRIVATE_H
