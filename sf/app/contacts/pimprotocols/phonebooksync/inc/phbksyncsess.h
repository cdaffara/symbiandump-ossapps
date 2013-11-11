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
// Header file for Phonebook Synchroniser Session class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __PHBKSYNCSESS_H__
#define __PHBKSYNCSESS_H__

/**
 *  This is the Phonebook Sync Server side session class and is responsible
 *  for handling the client (RPhoneBookSession) requests, encoding/decoding
 *  the parameters and Contacts Item phonebook data across the API.
 *  Once the parameters are decoded the request is sent to the server where
 *  it will either be handled directly or forwarded to the Background Sync
 *  Engine. Once the request is completed, any return parameters are written
 *  back to the client if neccessary.
 */
class CPhoneBookSession : public CSession2
	{
public:
	void CreateL();
	void CompleteRequest(const RMessage2& aMessage, TInt aResult) const;

	inline CPhoneBookServer& Server();

private:
	~CPhoneBookSession();

	void ServiceL(const RMessage2& aMessage);
	void DoSynchronisationL(const RMessage2& aMessage);
	void GetContactFormatL(const RMessage2& aMessage);
	void GetSyncModeL(const RMessage2& aMessage);
	void SetSyncModeL(const RMessage2& aMessage);
	void DeleteCntFromICCL(const RMessage2& aMessage);
	void WriteCntToICCL(const RMessage2& aMessage);
	void ValidateContactL(const RMessage2& aMessage);
	void UpdateLookupTableL(const RMessage2& aMessage);
	void GetPhoneBookIdL(const RMessage2& aMessage);
	void GetCacheStateL(const RMessage2& aMessage);
	void DbgMarkHeapL(const RMessage2& aMessage);
	void DbgCheckHeapL(const RMessage2& aMessage);
	void DbgMarkEndL(const RMessage2& aMessage);
	void DbgFailNextL(const RMessage2& aMessage);
	void DoSynchronisationCancelL(const RMessage2& aMessage);
	void DeleteCntFromICCCancelL(const RMessage2& aMessage);
	void WriteCntToICCCancelL(const RMessage2& aMessage);
	void NotifyCacheStateChangeL(const RMessage2& aMessage);
	void NotifyCacheStateChangeCancelL(const RMessage2& aMessage);
	void GetLastSyncErrorL(const RMessage2& aMessage);
	void GetNumSlotsL(const RMessage2& aMessage);
	void GetNumFreeSlotsL(const RMessage2& aMessage);
	void GetFreeSlotsL(const RMessage2& aMessage);
	void GetSlotIdL(const RMessage2& aMessage);
	void ShutdownServerL(const RMessage2& aMessage);
	};

/**
 *  Returns a reference to the CPhoneBookServer class.
 */
inline CPhoneBookServer& CPhoneBookSession::Server()
	{
	return *static_cast<CPhoneBookServer*>(const_cast<CServer2*>(CSession2::Server()));
	} // CPhoneBookSession::Server


#endif // __PHBKSYNCSESS_H__

