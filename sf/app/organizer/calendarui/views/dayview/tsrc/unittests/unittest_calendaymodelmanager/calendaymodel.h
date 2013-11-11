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
						QObject *parent = 0){
							Q_UNUSED(services);
							Q_UNUSED(parent);
							mModelRefreshed = 0;
							mModelDate = date;
						}
	
    // from QAbstractListModel
	int rowCount(const QModelIndex &parent = QModelIndex()) const{
		Q_UNUSED(parent);
		return 0;
	}
    QVariant data(const QModelIndex &index, int role) const{
		Q_UNUSED(index);
		Q_UNUSED(role);
		return 0;
    }
    
	void refreshModel(const QDateTime &date){
		mModelDate = date;
		mModelRefreshed++;
	}
	
	QDateTime modelDate(){
		return mModelDate;
	}
	
	//return info how much time model was refreshed
	quint32 modelRefreshed() {return mModelRefreshed;}
	
private:
	quint32 mModelRefreshed;
	QDateTime mModelDate;
};

#endif //CALENDAYMODEL_H

