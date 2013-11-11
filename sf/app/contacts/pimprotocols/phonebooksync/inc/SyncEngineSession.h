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
// Header file for the Background Sync Engine session class. This class is
// responsible for passing the requests that have come from the main
// front-end server's engine handle to the main engine class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNCENGINESESSION_H__
#define __SYNCENGINESESSION_H__

#include "SyncEngineServer.h"


/**
 *  This is the Background Sync Engine side session class and is responsible
 *  for handling the client (RSyncEngineSession) requests, encoding/decoding
 *  the parameters and Contacts Item phonebook data across the API.
 *  Once the parameters are decoded the request is sent to the engine where
 *  it will be handled by one of the Active Objects. Once the request is
 *  completed, any return parameters are written back to the client if
 *  neccessary.
 *
 *  @internalComponent
 */
class CSyncEngineSession : public CSession2
	{
public:
	void CreateL();
	void CompleteRequest(const RMessage2& aMessage, TInt aResult) const;

	inline CSyncEngineServer& Server();

private:
	~CSyncEngineSession();

	void ServiceL(const RMessage2& aMessage);
	void DoSynchronisationL(const RMessage2& aMessage);
	void DeleteCntFromICCL(const RMessage2& aMessage);
	void WriteCntToICCL(const RMessage2& aMessage);
	void DoSynchronisationCancelL(const RMessage2& aMessage);
	void DeleteCntFromICCCancelL(const RMessage2& aMessage);
	void WriteCntToICCCancelL(const RMessage2& aMessage);
	};

/**
 *  Returns a reference to the CSyncEngineServer class.
 */
inline CSyncEngineServer& CSyncEngineSession::Server()
	{
	return *static_cast<CSyncEngineServer*>(const_cast<CServer2*>(CSession2::Server()));
	} // CSyncEngineSession::Server


#endif // __SYNCENGINESESSION_H__

