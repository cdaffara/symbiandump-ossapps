/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  HbDataFormModel stub
*
*/

#ifndef HBDATAFORMMODEL_H
#define HBDATAFORMMODEL_H

#include <QAbstractItemModel>

#include <hbglobal.h>
#include <hbdataformmodelitem.h>

class HbDataFormModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    HbDataFormModel(QObject *parent = 0); 
    virtual ~HbDataFormModel();

    HbDataFormModelItem* itemFromIndex(const QModelIndex &index) const;
    
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public:
    
    HbDataFormModelItem* mItemReturnValue;
    
};

#endif //HBDATAFORMMODEL_H

