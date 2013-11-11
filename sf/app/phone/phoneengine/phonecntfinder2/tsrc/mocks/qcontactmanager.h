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
#ifndef QCONTACTMANAGER_H
#define QCONTACTMANAGER_H

#include <QList>
#include <QString>
#include <qcontact.h>
#include <qcontactfilter.h>
#include <qcontactsortorder.h>
#include <qtcontactsglobal.h>

/*
*  This is very simple copy of the one function from QContactManager interface that contactfinder2 is using
*/


class QContactManager 
{

public:
    QContactManager() { };
	QContactManager(const QString &name) {Q_UNUSED(name) };
    virtual ~QContactManager() {};

public:
 
	virtual QContact contact(const QContactLocalId& contactId) const
	{
	Q_UNUSED(contactId);
	QContact cnt;
	return cnt;
	
	};// retrieve a contact
	
	virtual QList<QContactLocalId> contacts(const QContactFilter& filter, 
									 const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const
	{
	Q_UNUSED(filter)
	Q_UNUSED(sortOrders)
	QList<QContactLocalId> list;
	return list;
	};

};

#endif
