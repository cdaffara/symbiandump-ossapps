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
* Description: Unit test helper for mpcollectiondatamodel
*
*/

#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QAbstractListModel>

class TestHelper : public QAbstractListModel
{
    Q_OBJECT

public:

    TestHelper(QObject *parent=0);
    virtual ~TestHelper();

    // These are dummy; We have to inherit from QAbstractListModel so that
    // we can invoke the protected member QAbstractItemModel::createIndex()
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    QModelIndex indexFor(int index);
};

#endif  // TESTHELPER_H




