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
// $Workfile: btsmtm.h $
// $Author: Stevep $
// $Revision: 4 $
// $Date: 9/11/01 15:36 $
// 
//


#if !defined(__BTS_H__)
#define __BTS_H__

#include <obexservermtm.h>
#include <btcmtm.h> // STimeouts

class CBtSrvMtm : public CObexSrvMtm
/**
@class

Provides thin implementation of a Bluetooth server MTM over the top of the CObexSrvMtm.

Implements StartCommandL() to respond to a send request from the client MTM by starting off a 
CBtServerSendOperation.
 
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
	 * @return Pointer to a newly constructed CBtSrvMtm.
	 * @leave KErrXXX System-wide error codes
	 */

	IMPORT_C static CBtSrvMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry*  aEntry);
	
	/**
	 * Instantiates and initiates a CBtServerSemdOperation class to begin sending the specified OBEX object
	 * via Bluetooth. Called in response to InvokeAsyncFunctionL() in the client MTM, and passes through
	 * all the arguments passed in to that function. Only supports aCommand==CBtClientMtm::EBtcCmdSend
	 *
	 * @param aSelection Entry to operate on.
	 * @param aCommand Command to start. Only CBtClientMtm::EBtcCmdSend is supported.
	 * @param aParameter Package buffer containing timeout, port and password info needed for the send operation.
	 * @param aStatus Used to notify observer of completion.
	 * @leave KErrNotSupported if aCommand!=CBtClientMtm::EBtcCmdSend
	 * @leave KErrXXX System-wide error codes
	 */
		
	IMPORT_C virtual void  StartCommandL (	CMsvEntrySelection& aSelection, 
											TInt aCommand, 
											const TDesC8& aParameter, 
											TRequestStatus& aStatus); 
private:
	
	/**
	 * Constructor. Calls CObexSrvMtm's constructor in initialisation list.
	 * @param aRegisteredMtmDll Registration data for MTM DLL.
	 * @param aEntry Context on which to operate.
	 */
	
	CBtSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry);

private:
	CBtClientMtm::STimeouts iTimeouts; //< Timeouts structure. One for connect and another for put.

	};

#endif  // __BTS_H__
