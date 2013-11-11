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
// $Workfile: IRCMTM.H $
// $Author: Stevep $
// $Revision: 6 $
// $Date: 20/11/01 11:19 $
// 
//


#if !defined (__IRCMTM_H__)
#define __IRCMTM_H__

#include <obexclientmtm.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "msvconsts.h"
#endif

class CIrClientMtm : public CObexClientMtm
/**
Infrared Client MTM.

Provides client-side functionality for infrared messaging. This is a thin implementation over the top of
CObexClientMtm.
 
@publishedPartner
@released
*/
	{
public:

	/**
	 * Canonical NewL factory function. 
	 *
	 * @param aRegisteredMtmDll Reference to registration data for MTM DLL.
	 * @param aMsvSession Reference to CMsvSession of the client requesting the object.
	 * @return Pointer to a new, constructed CIrClientMtm
	 * @leave Leaves if no memory is available.
	 */

	IMPORT_C static CIrClientMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession);

	// Contains the timeout values for connect and put operations
	struct STimeouts
		{
		TInt iConnectTimeout; //< Connection attempt timeout in microseconds
		TInt iPutTimeout;	  //< Put attempt timeout in microseconds
		};

	//Enumeration for commands to be passed to InvokeAsyncFunctionL
	enum TAsyncCmds
		{
		EIrcCmdSend //< Send via IR operation
		};

	// --- RTTI functions ---
	
	/**
	 * Starts an asynchronous messaging function as an active object. Only works for EIrcCmdSend.
	 *
	 * @param aFunctionId Identifier of the function to be invoked. Only supports EIrcCmdSend.
	 * @param aSelection Selection of message entries for the requested function to operate on.
	 * @param aParameter Buffer containing input and output parameters.
	 * @param aCompletionStatus Canonical TRequestStatus used for control of the active object.
	 * @return Pointer to a new asynchronously completing CMsvOperation. If failed, this is a completed operation with 
	 * status set to the relevant error code.
	 * @leave KErrXXX System-wide error codes
	 * @leave KErrNotSupported if aFunctionId != EIrcCmdSend
	 */
	
	CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId,const CMsvEntrySelection& aSelection, TDes8& aParameter, TRequestStatus& aCompletionStatus);

private:
	
	/**
	 * Constructor
	 *
	 * @param aRegisteredMtmDll Registration data for MTM DLL.
	 * @param aMsvSession CMsvSession of the client requesting the object.
	 */

	CIrClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession);
	
	/**
	 * Deletes the old header, then creates a new CIrHeader.	
	 *
	 *  @leave KErrXXX System-wide error codes if allocation fails
	 */
		
	virtual void InitialiseHeaderL();
	};



#endif // __IRCMTM_H__
