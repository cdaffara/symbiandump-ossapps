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
#include "qcontactdetail.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"

Q_DEFINE_LATIN1_LITERAL(QContactName::DefinitionName, "Name");
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::DefinitionName, "PhoneNumber");
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::FieldNumber, "PhoneNumber");
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldFirst, "First");
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldLast, "Last");
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QContactDetail::QContactDetail
// -----------------------------------------------------------------------------
//
QContactDetail::QContactDetail()
{
}
// -----------------------------------------------------------------------------
// QContactDetail::QContactDetail
// -----------------------------------------------------------------------------
//
QContactDetail::QContactDetail(const QString& definitionName)
{
	mDefinition = definitionName;
}
// -----------------------------------------------------------------------------
// D'tor
// -----------------------------------------------------------------------------
//
QContactDetail::~QContactDetail()
{
}

QString QContactDetail::definitionName () const
{
	return mDefinition;
}
// -----------------------------------------------------------------------------
// Copy c'tor
// -----------------------------------------------------------------------------
//
QContactDetail::QContactDetail(const QContactDetail& other)
{
	mValue = other.value("");
}
// -----------------------------------------------------------------------------
// Operator =
// -----------------------------------------------------------------------------
//
QContactDetail& QContactDetail::operator=(const QContactDetail& other)
{
    mValue = other.value("");
    return *this;
}
// -----------------------------------------------------------------------------
// QContactDetail
// -----------------------------------------------------------------------------
//
QString QContactDetail::value(const QString& key) const
{
	Q_UNUSED(key)
	return mValue;
}

// -----------------------------------------------------------------------------
// QContactDetail
// -----------------------------------------------------------------------------
//
void QContactDetail::setValue(const QString& value)
{
	mValue = value;
}
