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

// System include files

// User include files
#include "convergedmessageaddress.h"
#include "convergedmessageaddressimpl.h"

// ====================== LOCAL FUNCTIONS ========================


// ====================== MEMBER FUNCTIONS =======================

//----------------------------------------------------------------
// ConvergedMessageAddress::ConvergedMessageAddress
// @see header
//----------------------------------------------------------------
ConvergedMessageAddress::ConvergedMessageAddress(
		const QString &address,
		const QString &alias)
{
	mConvergedMessageAddressImpl = new ConvergedMessageAddressImpl (
			address,
			alias);
}

//----------------------------------------------------------------
// ConvergedMessageAddress::~ConvergedMessageAddress
// @see header
//----------------------------------------------------------------
ConvergedMessageAddress::~ConvergedMessageAddress()
{
	delete mConvergedMessageAddressImpl;
}

//----------------------------------------------------------------
// ConvergedMessageAddress::setAddress
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddress::setAddress( 
		const QString &address )
{
	mConvergedMessageAddressImpl->setAddress(address);
}

//----------------------------------------------------------------
// ConvergedMessageAddress::address
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageAddress::address() const
{
	return mConvergedMessageAddressImpl->address();
}

//----------------------------------------------------------------
// ConvergedMessageAddress::setAlias
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddress::setAlias(const QString &alias)
{
	mConvergedMessageAddressImpl->setAlias(alias);
}

//----------------------------------------------------------------
// ConvergedMessageAddress::alias
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageAddress::alias() const
{
	return mConvergedMessageAddressImpl->alias();
}

//----------------------------------------------------------------
// ConvergedMessageAddress::serialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddress::serialize(QDataStream &stream) const
{
	mConvergedMessageAddressImpl->serialize(stream);
}

//----------------------------------------------------------------
// ConvergedMessageAddress::deserialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddress::deserialize(QDataStream &stream)
{
	mConvergedMessageAddressImpl->deserialize(stream);
}

//eof



