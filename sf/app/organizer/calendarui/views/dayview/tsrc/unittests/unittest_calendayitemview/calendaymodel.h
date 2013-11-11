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
						QObject *parent = 0)
	{
        Q_UNUSED(date)
        Q_UNUSED(parent)
        Q_UNUSED(services)
	};
	
	void Reset() 
    {
	    mEntryList.clear();
    }
	
	bool insertRows(int num, int row, const QModelIndex &aparent)
	{
	    Q_UNUSED(row);
	    Q_UNUSED(aparent);
	    AgendaEntry entry;
	    if ( !num ) {
	        entry.setType( AgendaEntry::TypeAppoinment ); //or reminder 
	        // non all day event
	        entry.setStartAndEndTime( QDateTime(QDate(2010,07,12),QTime(0,0)), 
                QDateTime(QDate(2010,07,12),QTime(1,0)) );
	    }
	    else {
	        entry.setType( AgendaEntry::TypeEvent );
	        // all day event
	        entry.setStartAndEndTime( QDateTime(QDate(2010,07,12),QTime(0,0)), 
	            QDateTime(QDate(2010,07,13),QTime(0,0)) );
	    }
	    mEntryList.append(QVariant::fromValue(entry));
	    return true;
	}
	
    // from QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const
	{
	    Q_UNUSED(parent)
        return mEntryList.count();
	}
    
	QVariant data(const QModelIndex &index, int role) const
    {
        if ( role == CalenDayEntry ) {
            return mEntryList.at(index.row());
        }
        return QVariant();
    }
    
	void refreshModel(const QDateTime &date) 
	{
	    Q_UNUSED(date)
	};
	
	QDateTime modelDate() const
	    {return QDateTime();}
	
	QVariantList mEntryList;
};

#endif //CALENDAYMODEL_H

