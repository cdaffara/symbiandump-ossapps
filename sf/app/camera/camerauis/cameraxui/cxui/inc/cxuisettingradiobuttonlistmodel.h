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
#ifndef CXUISETTINGRADIOBUTTONLISTMODEL_H_
#define CXUISETTINGRADIOBUTTONLISTMODEL_H_

#include <QObject>
#include <QStringList>
#include <QAbstractListModel>
#include "cxuienums.h"

/*!
* Model class to configure custom radio button list
* to configure the number of lines per setting
*/
class CxuiSettingRadioButtonListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    CxuiSettingRadioButtonListModel();
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    void setItems(QStringList items);
    int listBoxType() const;
    void setListBoxType(int type);
    QStringList items() const;
    void resetModel();

private:
    QStringList mItems;
    int mListBoxType;
};

#endif /* CXUISETTINGRADIOBUTTONLISTMODEL_H_ */
