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
#ifndef IR_CATEGORY_MODEL_H
#define IR_CATEGORY_MODEL_H

#include <QAbstractItemModel>

class IRQBrowseCategoryItem;

class IRCategoryModel : public QAbstractListModel
{
	Q_OBJECT

public:
    explicit IRCategoryModel(QObject *aParent = 0);
    ~IRCategoryModel();
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    
    const QString& categoryName(const QModelIndex &aIndex) const;
    
signals:
    void dataChanged();
    
private slots:
    void categoryReceived(QList<IRQBrowseCategoryItem*> *aItems);
    
private:
    void destroyAllItems();
    
private:
    QList<IRQBrowseCategoryItem*> *iItems;
};


#endif // IR_CATEGORY_MODEL_H
