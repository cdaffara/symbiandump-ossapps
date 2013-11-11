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



#ifndef QCONTACTNAME_H
#define QCONTACTNAME_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

class QContactName : public QContactDetail
{
public:

    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactName, "Name")
    Q_DECLARE_LATIN1_LITERAL(FieldPrefix, "Prefix");
    Q_DECLARE_LATIN1_LITERAL(FieldFirst, "First");
    Q_DECLARE_LATIN1_LITERAL(FieldMiddle, "Middle");
    Q_DECLARE_LATIN1_LITERAL(FieldLast, "Last");
    Q_DECLARE_LATIN1_LITERAL(FieldSuffix, "Suffix");

};

#endif
