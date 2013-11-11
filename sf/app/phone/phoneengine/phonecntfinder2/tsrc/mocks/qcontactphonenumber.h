/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef QCONTACTPHONENUMBER_H
#define QCONTACTPHONENUMBER_H

#include <QString>

#include "qcontactdetail.h"
#include "qcontact.h"
#include <qtcontactsglobal.h>


/* Leaf class */
//! [0]
class QContactPhoneNumber : public QContactDetail
{
public:

    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactPhoneNumber, "PhoneNumber");
    Q_DECLARE_LATIN1_LITERAL(FieldNumber, "PhoneNumber");
    Q_DECLARE_LATIN1_LITERAL(FieldSubTypes, "SubTypes");
    Q_DECLARE_LATIN1_LITERAL(SubTypeLandline, "Landline");
    Q_DECLARE_LATIN1_LITERAL(SubTypeMobile, "Mobile");
    Q_DECLARE_LATIN1_LITERAL(SubTypeFacsimile, "Facsimile");
    Q_DECLARE_LATIN1_LITERAL(SubTypePager, "Pager");
    Q_DECLARE_LATIN1_LITERAL(SubTypeVoice, "Voice");
    Q_DECLARE_LATIN1_LITERAL(SubTypeModem, "Modem");
    Q_DECLARE_LATIN1_LITERAL(SubTypeVideo, "Video");
    Q_DECLARE_LATIN1_LITERAL(SubTypeCar, "Car");
    Q_DECLARE_LATIN1_LITERAL(SubTypeBulletinBoardSystem, "BulletinBoardSystem");
    Q_DECLARE_LATIN1_LITERAL(SubTypeMessagingCapable, "MessagingCapable");
    Q_DECLARE_LATIN1_LITERAL(SubTypeAssistant, "Assistant");
    Q_DECLARE_LATIN1_LITERAL(SubTypeDtmfMenu, "DtmfMenu");
	

};
//! [0]

#endif

