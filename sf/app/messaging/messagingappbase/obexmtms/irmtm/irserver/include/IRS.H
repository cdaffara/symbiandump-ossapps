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
// $Workfile: IRS.H $
// $Author: Stevep $
// $Revision: 3 $
// $Date: 9/11/01 15:29 $
// 
//


#if !defined(__IRS_H__)
#define __IRS_H__

#include <obexservermtm.h>
#include <ircmtm.h> // STimeouts
#include <obexconstants.h>

class CIrHeader;

class CIrSrvMtm : public CObexSrvMtm
/**
@internalTechnology
@released
*/
	{
public:

	/**
	 * NewL factory function. Calls ReleaseLibrary() if construction fails.
	 *
	 * @param aRegisteredMtmDll Registration data for MTM DLL.
	 * @param aEntry Context on which to operate.
	 * @return Pointer to a newly constructed CIrSrvMtm.
	 * @leave KErrXXX System-wide error codes
	 */

	IMPORT_C static CIrSrvMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry*  aEntry);
	
	/**
	 * Instantiates and initiates a CIrServerSemdOperation class to begin sending the specified OBEX object
	 * via Infrared. Called in response to InvokeAsyncFunctionL() in the client MTM, and passes through
	 * all the arguments passed in to that function. Only supports aCommand==EIrMtmCmdSend
	 * @param aSelection Entry to operate on.
	 * @param aCommand Command to start. Only EIrMtmCmdSend is supported.
	 * @param aParameter Package buffer containing timeout, port and password info needed for the send operation.
	 * @param aStatus Used to notify observer of completion.
	 * @leave KErrNotSupported if aCommand!=EIrMtmCmdSend
	 * @leave KErrXXX if aCommand!= EIrMtmCmdSend
	 */
	
	IMPORT_C virtual void  StartCommandL (	CMsvEntrySelection& aSelection, 
											TInt aCommand, 
											const TDesC8& aParameter, 
											TRequestStatus& aStatus); 

	/**
	 * Destructor. Deletes the header member
	 */

	IMPORT_C ~CIrSrvMtm();
	
	/**
	 * Obtains progress information, which, if valid, results in calling Send()
	 */
	
	IMPORT_C void DoRunL();

private:

	/**
	 * Constructor. Calls CObexSrvMtm's constructor in initialisation list.
	 * @param aRegisteredMtmDll Registration data for MTM DLL.
	 * @param aEntry Context on which to operate.
	 */

	CIrSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry);
	
	/**
	 * Creates a server send operation to perform the send
	 */

	void SendL();

private:
	CIrClientMtm::STimeouts iTimeouts; //< Timeout structure contains connect timeout and a put timeout
	CIrHeader* iHeader; //< Infrared OBEX header information
	TBool iFirstSendAttempt; //< Boolean value ETrue if it is the first send attempt
	TObexIrProtocolInfo iProtocolInfo; //< Protocol info to initialise the CObexClient. Non-const because the CObexClient's NewL() takes a non-const reference
	};

#endif  // __IRS_H__
