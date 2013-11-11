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

#include <QDebug>
#include "qcontactmanager_moc.h"


QContactManager_Moc::QContactManager_Moc()
{   

    mExpectedResult = -1;
    qDebug()<<"QContactManager_Moc constructed";
}

QContactManager_Moc::~QContactManager_Moc()
{
    mList.clear();
}

/* get the contact*/
QContact QContactManager_Moc::contact(const QContactLocalId& contactId) const
{
	Q_UNUSED(contactId)
	return mContact;
}

/* match */
QList<QContactLocalId> QContactManager_Moc::contacts(const QContactFilter& filter, 
									 const QList<QContactSortOrder>& sortOrders) const
{
Q_UNUSED(filter)
Q_UNUSED(sortOrders)

	mList.clear();
	qDebug()<<"Moc - mExpectedResult is: " << mExpectedResult;
	if (mExpectedResult == 1)
		{
		mList << cnt1;
		qDebug()<<"Moc - match == 1 - list count is: " << mList.count();
		}
	else if (mExpectedResult == 2)
		{
		mList << cnt1;
		mList << cnt2;
		}
	return mList;
}

//--- moc functions

void QContactManager_Moc::setExpectedResult(int matchNumber)
    {
    mExpectedResult = matchNumber;
    qDebug()<<"Moc - mExpectedResult is: " << mExpectedResult;
    }
