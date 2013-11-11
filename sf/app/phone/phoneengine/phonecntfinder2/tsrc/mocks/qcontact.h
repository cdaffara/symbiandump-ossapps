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

#ifndef QCONTACT_H
#define QCONTACT_H

#include <QString>
#include <QList>
#include <QMetaType>

#include <qcontactid.h>
#include "qcontactdetail.h"
#include "qtcontactsglobal.h"
// This is not available yet
// #include "quniqueid.h"


class QContact
{
public:
    QContact();

    ~QContact();

    QContact(const QContact& other);
	QContact& operator=(const QContact& other);

	//QContactId id() const;
    
    QContactLocalId localId() const;

    QList<QContactDetail> details(const QString& definitionId = QString()) const;

public: //for testing
    void setDetails(const QList<QContactDetail>& details);

private:
    QList<QContactDetail> mDetails;
 
};
Q_DECLARE_METATYPE(QContact)
#endif //__QCONTACT_H_


