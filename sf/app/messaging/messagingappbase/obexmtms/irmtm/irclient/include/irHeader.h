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
// $Workfile: irHeader.h $
// $Author: Stevep $
// $Revision: 4 $
// $Date: 9/11/01 15:29 $
// 
//


#if !defined (__IRHEADER_H__)
#define __IRHEADER_H__


#include <e32std.h>
#include <obexheader.h>    //CObexMtmHeader
#include <obex.h>	//TObexIrProtocolInfo

class CMsvStore;
class RMsvReadStream;
class RMsvWriteStream;


// Stream version -- increment by one each time the externalized data format changes
// for a significant release.
const TInt32 KIrMessageStreamVersion = 1;//<Stream version
const TUid KUidStreamIrHeaders  = {0x1000AABD};    //< Stream UID for the CIrHeaders object
_LIT(KIrTransport, "IrTinyTP");
_LIT8(KIrObexClassName, "OBEX");
_LIT8(KIrXferClassName, "OBEX:IrXfer");
_LIT8(KIrObexAttributeName, "IrDA:TinyTP:LsapSel");

class CIrHeader : public CObexMtmHeader
/**
Base class for OBEX object headers. Allows them to be stored and restored to and from the
CMsvStore.
 
@internalTechnology
@released
*/
	{
public:

	/**
	 * Destructor. Empty
	 */

    IMPORT_C ~CIrHeader();
    
	/**
	 * Second-phase constructor. Calls BaseConstructL to initialise the base members
	 */
	
	IMPORT_C void ConstructL();
    
	/**
	 * Canonical NewL factory function
	 */
	
	IMPORT_C static CIrHeader* NewL();
	
	//Setters/Getters
	
	/**
	 * Get the IR protocol info of the recipient
	 *
	 * @return The protocol info of the recipient (NB: only one recipient is currently supported)
	 */
	
	IMPORT_C virtual const TObexIrProtocolInfo& IrProtocolInfo() const;
	
	/**
	 * Get the IR address of the recipient
	 *
	 * @return The address of the recipient (NB: only one recipient is currently supported)
	 */
	
	IMPORT_C virtual TPtrC8 Addr() const;
	
	/**
	 * Set the IR address of the recipient
	 *
	 * @param aAddr the address of the recipient (NB: only one recipient is currently supported)
	 * @leave KErrXXX system-wide leave codes if assignment fails
	 */

	IMPORT_C virtual void SetAddrL(const TDesC8& aAddr);

	//Called by Store/Restore--must be implelented in the base class, and must call
	//BaseExternalizeL/BaseInternaliseL first

	/**
	 * Internaliser. Reads data in from aReadStream. Calls BaseInternalizeL() to read in the base
	 * data first.
	 *
	 * @param aReadStream Stream to read data in from
	 * @leave KErrXXX Standard EPOC stream leave codes
	 */

    IMPORT_C virtual void InternalizeL(RMsvReadStream& aReadStream);
    
	/** 
	 * Externaliser. Writes data out to aWriteStream. Calls BaseExternalizeL() to write out
	 * the base data first.
	 *
	 * @param aWriteStream Stream to write data to
	 * @leave KErrXXX Standard SymbianOS stream leave codes
	 */
	
	IMPORT_C virtual void ExternalizeL(RMsvWriteStream& aWriteStream) const;

private:

	/**
	 * Default constructor, initialises the base class and sets the value of iIrProtocolInfo.iTransport
	 * as appropriate for IR
	 */

	CIrHeader();

private:
	TObexIrProtocolInfo iIrProtocolInfo; //< Protocol info to initialise the CObexClient. Non-const because the CObexClient's NewL() takes a non-const reference
	};

#endif	//!defined (__IRHEADER_H__)
