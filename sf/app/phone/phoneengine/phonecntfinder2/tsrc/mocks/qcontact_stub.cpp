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
#include "qcontact.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QContact::QContact
// -----------------------------------------------------------------------------
//
QContact::QContact(  )
    {    
    }

// -----------------------------------------------------------------------------
// QContact::QContact
// -----------------------------------------------------------------------------
//
QContact::QContact( 
        const QContact & contact )
    {
    mDetails = contact.details("");
    }

// -----------------------------------------------------------------------------
// QContact::QContact
// -----------------------------------------------------------------------------
//
QContact& QContact::operator=(const QContact& other)
{
    mDetails = other.details();
    return *this;
}
// -----------------------------------------------------------------------------
// QContact::~QContact
// -----------------------------------------------------------------------------
//
QContact::~QContact(  )
    {   
    }

// -----------------------------------------------------------------------------
// QContact::uid
// -----------------------------------------------------------------------------
//
/*QContactId QContact::id(  ) const
    {
    QContactId temp;
    return temp;
    }*/

// -----------------------------------------------------------------------------
// QContact::localId
// -----------------------------------------------------------------------------
//
QContactLocalId QContact::localId(  ) const
    {
    return 3;
    }

// -----------------------------------------------------------------------------
// QContact::details
// -----------------------------------------------------------------------------
//
QList<QContactDetail> QContact::details(const QString& definitionId) const
{
	Q_UNUSED(definitionId)
	return mDetails;
	
}
// -----------------------------------------------------------------------------
// QContact::setDetails
// -----------------------------------------------------------------------------
//
void QContact::setDetails(const QList<QContactDetail>& details)
{   
    mDetails.clear();
    QContactDetail detail;
    qDebug() << "QContact::setDetails, list count is: " << details.count();
    foreach (detail, details)
    {
        mDetails.append(detail);
        qDebug() << "QContact::setDetails, mDetails count is: " << mDetails.count();
        qDebug() << "QContact::setDetails, mDetails[0].value is: " << mDetails[0].value("");
    }

}
