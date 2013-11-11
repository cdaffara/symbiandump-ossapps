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

#ifndef IRMAINMODEL_H
#define IRMAINMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class IRPlayList;

class IRMainModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit IRMainModel(IRPlayList *aPlayList, QObject *aParent = 0);
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;

    void checkUpdate();
    
private:
    void initModel();
    
private:
    IRPlayList *iPlayList;
    QStringList iPrimaryTexts;
};

#endif
