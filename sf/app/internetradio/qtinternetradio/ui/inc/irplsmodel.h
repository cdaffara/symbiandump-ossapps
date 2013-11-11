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

#ifndef IRPLSMODEL_H
#define IRPLSMODEL_H

#include <QAbstractListModel>

class IRPlayList;

class IRPlsModel : public QAbstractListModel
{
public:
    explicit IRPlsModel(IRPlayList *aPlayList, QObject *aParent = 0);
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    
    const QString& getFile(int aIndex) const;
    const QString& getTitle(int aIndex) const;
    
private:
    IRPlayList *iPlayList;
};

#endif
