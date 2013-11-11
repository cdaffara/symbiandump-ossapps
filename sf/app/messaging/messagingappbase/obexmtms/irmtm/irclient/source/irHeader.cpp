// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// $Workfile: irHeader.cpp $
// $Author: Stevep $
// $Revision: 10 $
// $Date: 25/03/02 9:03 $
// 
//

//class include
#include <irheader.h>

//system includes
#include <s32strm.h> //RReadStream, RWriteStream
#include <msvstore.h>   //CMsvStore, RMsvWriteStream, RMsvReadStream
#include <obex.h>

//user includes
#include <obexinternalutils.h>

CIrHeader::CIrHeader():
	CObexMtmHeader(KUidStreamIrHeaders, KObexMessageStreamVersion)
/**
 * Default constructor, initialises the base class and sets the value of iIrProtocolInfo.iTransport
 * as appropriate for IR
 */
	{
	iIrProtocolInfo.iTransport = KIrTransport;
	iIrProtocolInfo.iClassName = KIrObexClassName;
	iIrProtocolInfo.iAttributeName = KIrObexAttributeName;
	}


EXPORT_C CIrHeader::~CIrHeader()
/**
 * Destructor. Empty
 */

	{
	}


EXPORT_C void CIrHeader::ConstructL()
/**
 * Second-phase constructor. Calls BaseConstructL to initialise the base members
 */
	{
	BaseConstructL();
	}

EXPORT_C CIrHeader* CIrHeader::NewL()
/**
 * Canonical NewL factory function
 */
	{
	CIrHeader* self = new (ELeave) CIrHeader;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	//self
	return self;
	}

EXPORT_C void CIrHeader::ExternalizeL(RMsvWriteStream& aWriteStream) const
/**
 * Externaliser. Writes data out to aWriteStream. Calls BaseExternalizeL() to write out
 * the base data first.
 * @param aWriteStream Stream to write data to
 * @leave KErrXXX Standard SymbianOS stream leave codes
 */
	{
	BaseExternalizeL(aWriteStream);

	//TBuf16
	ObexInternalUtils::ExternalizeL(iIrProtocolInfo.iTransport, aWriteStream);

	//TBuf8
	ObexInternalUtils::ExternalizeL(iIrProtocolInfo.iClassName, aWriteStream);
	ObexInternalUtils::ExternalizeL(iIrProtocolInfo.iAttributeName, aWriteStream);
	ObexInternalUtils::ExternalizeL(iIrProtocolInfo.iAddr, aWriteStream);
	}


EXPORT_C void CIrHeader::InternalizeL(RMsvReadStream& aReadStream)
/**
 * Internaliser. Reads data in from aReadStream. Calls BaseInternalizeL() to read in the base
 * data first.
 * @param aReadStream Stream to read data in from
 * @leave KErrXXX Standard SymbianOS stream leave codes
 */
	{
	BaseInternalizeL(aReadStream);

	//TBuf16
	ObexInternalUtils::InternalizeL(iIrProtocolInfo.iTransport, aReadStream);

	//TBuf8
	ObexInternalUtils::InternalizeL(iIrProtocolInfo.iClassName, aReadStream);
	ObexInternalUtils::InternalizeL(iIrProtocolInfo.iAttributeName, aReadStream);
	ObexInternalUtils::InternalizeL(iIrProtocolInfo.iAddr, aReadStream);
	}


EXPORT_C void CIrHeader::SetAddrL(const TDesC8& aAddr)
/**
 * Set the IR address of the recipient
 *
 * @param aAddr the address of the recipient (NB: only one recipient is currently supported)
 * @leave KErrXXX system-wide leave codes if assignment fails
 */
	{
	iIrProtocolInfo.iAddr.Copy(aAddr);
	}

EXPORT_C TPtrC8 CIrHeader::Addr() const
/**
 * Get the IR address of the recipient
 *
 * @return The address of the recipient (NB: only one recipient is currently supported)
 */
	{
	return iIrProtocolInfo.iAddr;
	}

EXPORT_C const TObexIrProtocolInfo& CIrHeader::IrProtocolInfo() const
/**
 * Get the IR protocol info of the recipient
 *
 * @return The protocol info of the recipient (NB: only one recipient is currently supported)
 */
	{
	return iIrProtocolInfo;
	}
