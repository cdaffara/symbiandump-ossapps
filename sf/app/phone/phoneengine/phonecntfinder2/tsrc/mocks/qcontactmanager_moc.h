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

#ifndef QCONTACTMANAGER_MOC_H
#define QCONTACTMANAGER_MOC_H

#include <e32base.h>
#include "qcontactmanager.h"
#include <qtcontactsglobal.h>
#include <QList>



class QContactManager_Moc : public QContactManager
{


public:
    QContactManager_Moc();
    virtual ~QContactManager_Moc();
    
public:
    
	QContact contact(const QContactLocalId& contactId) const;
	QList<QContactLocalId> contacts(const QContactFilter& filter, 
									 const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const;
    void setExpectedResult(int matchNumber);
 
private:
    int mExpectedResult;
    mutable QList<QContactLocalId> mList;
    QContactLocalId cnt1;
    QContactLocalId cnt2;
	QContact  mContact;
    
};

#endif
