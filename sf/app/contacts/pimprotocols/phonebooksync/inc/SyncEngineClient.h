// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file for the Background Sync Engine client interface.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNCENGINECLIENT_H__
#define __SYNCENGINECLIENT_H__

#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>

#include "PhonebookManager.h"


/**
 *  Provides the front-end server with client access to a session with the
 *  Background Sync Engine. 
 */
class RSyncEngineSession : public RSessionBase
	{
public:
	RSyncEngineSession();

	TInt Connect(CPhoneBookManager* aPhoneBookManager);
	void Close();

   	void DoSynchronisation(TRequestStatus& aReqStatus, TUid aPhonebookUid);
	void DoSynchronisationCancel(TUid aPhonebookId);
	void DeleteContact(TRequestStatus& aReqStatus, TUid aPhonebookUid,
					   TContactItemId aId);
	void DeleteContactCancel(TUid aPhonebookUid);
   	void WriteContact(TRequestStatus& aReqStatus,
   					  TPtrC8& aTemplateIdAndBufferSizePtr,
   					  TPtr8& aSlotNumberPtr,
   					  TPtr8& aPhoneBookUidPtr,
   					  TPtr8& aICCEntryBufPtr);
	void WriteContactCancel(TUid aPhonebookId);

private:
	RSyncEngineSession(const RSyncEngineSession& aSession);

	TInt StartPhBkSyncEngine(CPhoneBookManager* aPhoneBookManager);

	RThread  iEngineThread;
	TRequestStatus  iEngineThreadStatus;
	};


#endif // __SYNCENGINECLIENT_H__
