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
// $Workfile: obexinternalutils.cpp $
// $Author: Stevep $
// $Revision: 3 $
// $Date: 19/11/01 15:55 $
// 
//

//class include
#include <obexinternalutils.h>

//system includes
#include <s32strm.h> //RReadStream, RWriteStream

EXPORT_C void ObexInternalUtils::InternalizeL(HBufC*& aBuf, RReadStream& aStream)
/**
 * Internalizes a HBufC from a stream.
 *
 * @param aBuf The HBufC to be internalized
 * @param aStream The stream to be read from
 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
 */
    {
    TInt length = aStream.ReadInt32L();
    delete aBuf;
    aBuf = NULL;
    aBuf = HBufC::NewL(aStream, length);
    }

EXPORT_C void ObexInternalUtils::ExternalizeL(const HBufC* aBuf, RWriteStream& aStream)
/**
 * Externalizes a HBufC to a stream.
 *
 * @param aBuf The HBufC to be externalized
 * @param aStream The stream to be written to
 * @leave KErrXXX Leaves with standard EPOC stream leave codes
 */
    {
    aStream.WriteInt32L(aBuf->Length());
    aStream << *aBuf;
    }


EXPORT_C void ObexInternalUtils::InternalizeL(HBufC8*& aBuf, RReadStream& aStream)
/**
 * Internalizes a HBufC8 from a stream.
 *
 * @param aBuf The HBufC8 to be internalized
 * @param aStream The stream to be read from
 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
 */
    {
    TInt length = aStream.ReadInt32L();
    delete aBuf;
    aBuf = NULL;
    aBuf = HBufC8::NewL(aStream, length);
    }

EXPORT_C void ObexInternalUtils::ExternalizeL(const HBufC8* aBuf, RWriteStream& aStream)
/**
 * Externalizes a HBufC8 to a stream.
 *
 * @param aBuf The HBufC8 to be externalized
 * @param aStream The stream to be written to
 * @leave KErrXXX Leaves with standard EPOC stream leave codes
 */
    {
    aStream.WriteInt32L(aBuf->Length());
    aStream << *aBuf;
    }


EXPORT_C void ObexInternalUtils::InternalizeL(TDes& aDes, RReadStream& aStream)
/**
 * Internalizes a TDes from a stream.
 *
 * @param aDes The TDes to be internalized
 * @param aStream The stream to be read from
 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
 * @leave KErrOverflow if the incoming data is too big for the TBuf provided
 */
    {
	//read a HBufC from the stream
    TInt length = aStream.ReadInt32L();
 	HBufC* buf = HBufC::NewL(aStream, length);
	CleanupStack::PushL(buf);

	//check it'll fit into aBuf
	if (length <= aDes.MaxLength())
		{
		//copy it into aDes
		aDes.Copy(*buf);
		}
	else
		{
		//Doesn't fit so leave
		User::Leave(KErrOverflow);
		}

	CleanupStack::PopAndDestroy();	//buf
	}

EXPORT_C void ObexInternalUtils::ExternalizeL(const TDes& aDes, RWriteStream& aStream)
/**
 * Externalizes a TDes to a stream.
 *
 * @param aBuf The TDes to be externalized
 * @param aStream The stream to be written to
 * @leave KErrXXX Leaves with standard EPOC stream leave codes
 */
    {
    aStream.WriteInt32L(aDes.Length());
    aStream << aDes;
    }


EXPORT_C void ObexInternalUtils::InternalizeL(TDes8& aDes, RReadStream& aStream)
/**
 * Internalizes a TDes8 from a stream.
 *
 * @param aBuf The TDes8 to be internalized
 * @param aStream The stream to be read from
 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
 * @leave KErrOverflow if the incoming data is too big for the TBuf provided
 */
    {
	//read a HBufC from the stream
    TInt length = aStream.ReadInt32L();
 	HBufC8* buf = HBufC8::NewL(aStream, length);
	CleanupStack::PushL(buf);

	//check it'll fit into aBuf
	if (length <= aDes.MaxLength())
		{
		//copy it into aDes
		aDes.Copy(*buf);
		}
	else
		{
		//Doesn't fit so leave
		User::Leave(KErrOverflow);
		}

	CleanupStack::PopAndDestroy();	//buf
	}

EXPORT_C void ObexInternalUtils::ExternalizeL(const TDes8& aDes, RWriteStream& aStream)
/**
 * Externalizes a TDes8 to a stream.
 *
 * @param aBuf The TDes8 to be externalized
 * @param aStream The stream to be written to
 * @leave KErrXXX Leaves with standard EPOC stream leave codes
 */
    {
    aStream.WriteInt32L(aDes.Length());
    aStream << aDes;
    }

EXPORT_C void ObexInternalUtils::Panic(ObexInternalUtils::TObexInternalUtilsPanic aPanic)
/**
 * Indicates Panic originates from Obex MTM
 *
 * @param aPanic The type of TObexInternalUtilsPanic which has taken place
 */
	{
	_LIT(KPanicCategory, "ObexMTM");
	User::Panic(KPanicCategory, aPanic);
	}
