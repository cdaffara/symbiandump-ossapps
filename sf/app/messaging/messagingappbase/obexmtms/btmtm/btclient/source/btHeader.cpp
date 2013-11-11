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
// $Workfile: btHeader.cpp $
// $Author: Stevep $
// $Revision: 10 $
// $Date: 25/03/02 7:54 $
// 
//

//class include
#include <btheader.h>

//system includes
#include <s32strm.h> //RReadStream, RWriteStream
#include <msvstore.h>   //CMsvStore, RMsvWriteStream, RMsvReadStream
#include <obex.h>

//user includes
#include <obexinternalutils.h>

CBtHeader::CBtHeader():
	CObexMtmHeader(KUidStreamBtHeaders, KObexMessageStreamVersion)
/**
 * Default constructor, initialises the base class and sets the value of iBtProtocolInfo.iTransport
 * as appropriate for BT
 */
	{
	iBtProtocolInfo.iTransport = KBtTransport;
	}

	
EXPORT_C CBtHeader::~CBtHeader()
/**
 * Destructor. Empty
 */
	{
	}


EXPORT_C void CBtHeader::ConstructL()
/**
 * Second-phase constructor. Calls BaseConstructL to initialise the base members
 */
	{
	BaseConstructL();
	}

EXPORT_C CBtHeader* CBtHeader::NewL()
/**
 * Canonical NewL factory function
 */
	{
	CBtHeader* self = new (ELeave) CBtHeader;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	//self
	return self;
	}

EXPORT_C void CBtHeader::ExternalizeL(RMsvWriteStream& aWriteStream) const
/**
 * Externaliser. Writes data out to aWriteStream. Calls BaseExternalizeL() to write out
 * the base data first.
 *
 * @param aWriteStream Stream to write data to
 * @leave KErrXXX Standard EPOC stream leave codes
 */
	{
	BaseExternalizeL(aWriteStream);

	//TBuf16
	ObexInternalUtils::ExternalizeL(iBtProtocolInfo.iTransport, aWriteStream);

	//TBuf8
	ObexInternalUtils::ExternalizeL(iBtProtocolInfo.iAddr, aWriteStream);
	}


EXPORT_C void CBtHeader::InternalizeL(RMsvReadStream& aReadStream)
/**
 * Internaliser. Reads data in from aReadStream. Calls BaseInternalizeL() to read in the base 
 * data first.
 *
 * @param aReadStream Stream to read data in from
 * @leave KErrXXX Standard EPOC stream leave codes
 */
	{
	BaseInternalizeL(aReadStream);

	//TBuf16
	ObexInternalUtils::InternalizeL(iBtProtocolInfo.iTransport, aReadStream);

	//TBuf8
	ObexInternalUtils::InternalizeL(iBtProtocolInfo.iAddr, aReadStream);
	}


EXPORT_C void CBtHeader::SetAddrL(const TDesC8& aAddr)
/**
 * Set the BT address of the recipient.  The RfComm service slot will be obtained through 
 * an SDP lookup in the server side MTM.
 *
 *  @param aAddr the address of the recipient (NB: only one recipient is currently supported)
 */
	{
// put comment here about incoming data format
	iBtProtocolInfo.iAddr.SetBTAddr(aAddr);
	}

EXPORT_C TPtrC8 CBtHeader::Addr() const
/**
 * Get the BT address of the recipient
 *
 *  @return The address of the recipient (NB: only one recipient is currently supported)
 */
	{
	return iBtProtocolInfo.iAddr;
	}


EXPORT_C const TObexBluetoothProtocolInfo& CBtHeader::BtProtocolInfo() const
/**
 * Get the BT protocol info of the recipient
 *
 * @return The protocol info of the recipient (NB: only one recipient is currently supported)
 */
	{
	return iBtProtocolInfo;
	}
