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
// $Workfile: btHeader.h $
// $Author: Stevep $
// $Revision: 5 $
// $Date: 23/01/02 13:31 $
// 
//


#if !defined (__BTHEADER_H__)
#define __BTHEADER_H__


#include <e32std.h>
#include <obexheader.h>		//CObexMtmHeader
#include <obex.h>			//TObexBluetoothProtocolInfo

class CMsvStore;
class RMsvReadStream;
class RMsvWriteStream;


// Stream version -- increment by one each time the externalized data format changes
// for a significant release.
const TInt32 KBtMessageStreamVersion = 1;//<Stream version
const TUid KUidStreamBtHeaders  = {0x1000AABC};    //< Stream UID for the CBtHeaders object
_LIT(KBtTransport, "RFCOMM");

class CBtHeader : public CObexMtmHeader
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
	
    IMPORT_C ~CBtHeader();
    
	/**
	 * Second-phase constructor. Calls BaseConstructL to initialise the base members
	 */
	
	IMPORT_C void ConstructL();
    
	/**
	 * Canonical NewL factory function
	 */

	IMPORT_C static CBtHeader* NewL();
	//Setters/Getters
	
	
	/**
	 * Get the BT protocol info of the recipient
	 *
	 * @return The protocol info of the recipient (NB: only one recipient is currently supported)
	 */

	IMPORT_C virtual const TObexBluetoothProtocolInfo& BtProtocolInfo() const;
	
	/**
	 * Get the BT address of the recipient
	 *
	 *  @return The address of the recipient (NB: only one recipient is currently supported)
	 */
	
	IMPORT_C virtual TPtrC8 Addr() const;
	
	/**
	 * Set the BT address of the recipient.  The RfComm service slot will be obtained through 
	 * an SDP lookup in the server side MTM.
	 *
	 *  @param aAddr the address of the recipient (NB: only one recipient is currently supported)
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
	 * Default constructor, initialises the base class and sets the value of iBtProtocolInfo.iTransport
	 * as appropriate for BT
	 */

	CBtHeader();

private:
	TObexBluetoothProtocolInfo iBtProtocolInfo;		//< Protocol info to be internalised/externalised
	};

#endif	//!defined (__BTHEADER_H__)
