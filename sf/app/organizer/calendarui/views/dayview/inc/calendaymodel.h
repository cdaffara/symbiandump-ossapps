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
* Description: Single day item view model
*
*/

#ifndef CALENDAYMODEL_H
#define CALENDAYMODEL_H

//System includes
#include <QAbstractItemModel>
#include <QVariant>
#include <QDateTime>

#include <agendaentry.h>

//User includes

//Forward declarations
class MCalenServices;

//Constantss
const int CalenDayEntry = Qt::UserRole + 1; 

Q_DECLARE_METATYPE(AgendaEntry)

class CalenDayModel : public QAbstractListModel
{
    Q_OBJECT
public:
	CalenDayModel(const QDateTime &date, MCalenServices &services,
						QObject *parent = 0);
	
    // from QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    
	void refreshModel(const QDateTime &date);
	
	QDateTime modelDate() const;
	
	
private:
    void loadAndStoreInstances(const QDateTime& date);
    
private:
    MCalenServices &mServices;
    QVariantList mEntryList;
    QDateTime mDateTime;
};

#endif //CALENDAYMODEL_H

