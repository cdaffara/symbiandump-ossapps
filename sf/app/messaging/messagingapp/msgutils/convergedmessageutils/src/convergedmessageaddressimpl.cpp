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
#include "convergedmessageaddressimpl.h"

// ====================== LOCAL FUNCTIONS ========================


// ====================== MEMBER FUNCTIONS =======================

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::ConvergedMessageAddressImpl
// @see header
//----------------------------------------------------------------
ConvergedMessageAddressImpl::ConvergedMessageAddressImpl(
		const QString &address,
		const QString &alias)
: mRealAddress(address),mAlias(alias)
{
	//do nothing
}

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::~ConvergedMessageAddressImpl
// @see header
//----------------------------------------------------------------
ConvergedMessageAddressImpl::~ConvergedMessageAddressImpl()
{
	//do nothing
}

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::setAddress
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddressImpl::setAddress( 
		const QString &address )
{
	mRealAddress = address;
}

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::address
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageAddressImpl::address() const
{
	return mRealAddress;
}

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::setAlias
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddressImpl::setAlias(const QString &alias)
{
	mAlias = alias;
}

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::alias
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageAddressImpl::alias() const
{
	return mAlias;
}

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::serialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddressImpl::serialize(
		QDataStream &stream) const
{
	stream << mRealAddress;
	stream << mAlias;
}

//----------------------------------------------------------------
// ConvergedMessageAddressImpl::deserialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageAddressImpl::deserialize(QDataStream &stream)
{
	stream >> mRealAddress;
	stream >> mAlias;
}

//eof



