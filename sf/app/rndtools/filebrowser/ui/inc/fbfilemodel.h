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

#ifndef FBFILEMODELH_H_
#define FBFILEMODELH_H_

#include <QAbstractListModel>

// Forward declarations
class EngineWrapper;
class QModelIndex;
class QFileIconProvider;

class FbFileModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FbFileModel(EngineWrapper *engineWrapper, QObject *parent = 0);
    virtual ~FbFileModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    EngineWrapper *mEngineWrapper;
    QFileIconProvider *mFileIconProvider;
};



#endif /* FBFILEMODELH_H_ */
