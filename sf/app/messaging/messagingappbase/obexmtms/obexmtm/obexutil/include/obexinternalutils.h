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
// $Workfile: obexinternalutils.h $
// $Author: Stevep $
// $Revision: 1 $
// $Date: 12/11/01 11:36 $
// 
//

#ifndef __OBEXINTERNALUTILS_H
#define __OBEXINTERNALUTILS_H

#include <e32base.h>
class RReadStream;
class RWriteStream;

class ObexInternalUtils : public CBase
/**
Utility class containing on static member functions to provide internalize/externalize functionality to
HBufC and HBufC8.

@internalTechnology
@released
*/
	{
public:
	//Enumeration for panic codes
	enum TObexInternalUtilsPanic
		{
		EObexBadStreamVersion //< Panic if stream version is incorrect
		};
public:
	
	/**
	 * Externalizes a HBufC to a stream.
	 *
	 * @param aBuf The HBufC to be externalized
	 * @param aStream The stream to be written to
	 * @leave KErrXXX Leaves with standard EPOC stream leave codes
	 */
	
	IMPORT_C static void ExternalizeL(const HBufC* aBuf, RWriteStream& aStream);

	/**
	 * Internalizes a HBufC from a stream.
	 *
	 * @param aBuf The HBufC to be internalized
	 * @param aStream The stream to be read from
	 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
	 */

	IMPORT_C static void InternalizeL(HBufC*& aBuf, RReadStream& aStream);

	/**
	 * Externalizes a HBufC8 to a stream.
	 *
	 * @param aBuf The HBufC8 to be externalized
	 * @param aStream The stream to be written to
	 * @leave KErrXXX Leaves with standard EPOC stream leave codes
	 */
	
	IMPORT_C static void ExternalizeL(const HBufC8* aBuf, RWriteStream& aStream);
	
	/**
	 * Internalizes a HBufC8 from a stream.
	 *
	 * @param aBuf The HBufC8 to be internalized
	 * @param aStream The stream to be read from
	 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
	 */ 
	
	IMPORT_C static void InternalizeL(HBufC8*& aBuf, RReadStream& aStream);

	/**
	 * Externalizes a TDes to a stream.
	 *
	 * @param aBuf The TDes to be externalized
	 * @param aStream The stream to be written to
	 * @leave KErrXXX Leaves with standard EPOC stream leave codes
	 */
	
	IMPORT_C static void ExternalizeL(const TDes& aBuf, RWriteStream& aStream);
	
	/**
	 * Internalizes a TDes from a stream.
	 *
	 * @param aDes The TDes to be internalized
	 * @param aStream The stream to be read from
	 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
	 * @leave KErrOverflow if the incoming data is too big for the TBuf provided
	 */
	
	IMPORT_C static void InternalizeL(TDes& aBuf, RReadStream& aStream);

	/**
	 * Externalizes a TDes8 to a stream.
	 *
	 * @param aBuf The TDes8 to be externalized
	 * @param aStream The stream to be written to
	 * @leave KErrXXX Leaves with standard EPOC stream leave codes
	 */

	IMPORT_C static void ExternalizeL(const TDes8& aBuf, RWriteStream& aStream);
	
	/**
	 * Internalizes a TDes8 from a stream.
	 *
	 * @param aBuf The TDes8 to be internalized
	 * @param aStream The stream to be read from
	 * @leave KErrXXX Leaves with standard EPOC stream and memory allocation leave codes
	 * @leave KErrOverflow if the incoming data is too big for the TBuf provided
	 */
	
	IMPORT_C static void InternalizeL(TDes8& aBuf, RReadStream& aStream);

	/**
	 * Indicates Panic originates from Obex MTM
	 *
	 * @param aPanic The type of TObexInternalUtilsPanic which has taken place
	 */

	IMPORT_C static void Panic(TObexInternalUtilsPanic aPanic);
	};

#endif//__OBEXINTERNALUTILS_H
