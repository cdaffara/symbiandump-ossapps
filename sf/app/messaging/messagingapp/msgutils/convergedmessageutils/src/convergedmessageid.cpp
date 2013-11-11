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
 * Description: This class is for convergedmessage id description
 *
 */

// System include files

// User include files
#include "convergedmessageid.h"

// ====================== LOCAL FUNCTIONS ========================


// ====================== MEMBER FUNCTIONS =======================

//----------------------------------------------------------------
// ConvergedMessageId::ConvergedMessageId
// @see header
//----------------------------------------------------------------

ConvergedMessageId::ConvergedMessageId(int id)
:mId(id)
{
	// do nothing
}

//----------------------------------------------------------------
// ConvergedMessageId::ConvergedMessageId
// @see header
//----------------------------------------------------------------
ConvergedMessageId::ConvergedMessageId(
		const ConvergedMessageId &other)
{
	mId = other.getId();
}

//----------------------------------------------------------------
// ConvergedMessageId::operator==
// @see header
//----------------------------------------------------------------
bool ConvergedMessageId::operator==(
		const ConvergedMessageId &other) const
{
	return (mId==other.getId());
}

//----------------------------------------------------------------
// ConvergedMessageId::operator=
// @see header
//----------------------------------------------------------------
ConvergedMessageId& ConvergedMessageId::operator=(
		const ConvergedMessageId &other)
{
	mId = other.getId();
	return *this;
}

//----------------------------------------------------------------
// ConvergedMessageId::getId
// @see header
//----------------------------------------------------------------
int ConvergedMessageId::getId() const
{
	return mId;
}

//----------------------------------------------------------------
// ConvergedMessageId::setId
// @see header
//----------------------------------------------------------------
void ConvergedMessageId::setId(int id)
{
	this->mId = id;
}

//----------------------------------------------------------------
// ConvergedMessageId::serialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageId::serialize(QDataStream &stream) const
{
	// pud id into the stream
	QString id = QString::number(mId);
	stream << id;
}

//----------------------------------------------------------------
// ConvergedMessageId::deserialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageId::deserialize(QDataStream &stream)
{
	// read back from stream
	QString id;
	stream >> id;
	mId = id.toInt();
}
// eof

