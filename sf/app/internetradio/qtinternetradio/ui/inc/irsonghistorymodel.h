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

#ifndef IRSONGHISTORYMODEL_H_
#define IRSONGHISTORYMODEL_H_

#include <QAbstractItemModel>
 
class IRQSongInfo;
class IRQMetaData;
class songHistoryWrapper;

class IRSongHistoryModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit IRSongHistoryModel(QObject *aParent = NULL);
    ~IRSongHistoryModel();
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    
    IRQSongInfo* getSongHistoryInfo(int aIndex);
    void clearList();
    bool checkSongHistoryUpdate();   
    void setOrientation(Qt::Orientation aOrientation);
    bool deleteOneItem(int aIndex);
    
signals:
    void modelChanged();   

private:
    void getAllList();

private:
    QList<IRQSongInfo *> mSongHistoryList;     
    Qt::Orientation mOrientation;
    songHistoryWrapper *mHistoryWrapper;
};

#endif 
