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
// $Workfile: AddObexAttachmentState.h $
// $Author: Markw $
// $Revision: 2 $
// $Date: 24/01/02 10:48 $
// 
//

#ifndef __ADDOBEXATTACHMENTSTATE_H__
#define __ADDOBEXATTACHMENTSTATE_H__

#include "harness.h"        //MBaseTestState

// forward declarations
class CObexClientTest;


//
// CAddObexAttachmentAsEntryState
//
class CAddObexAttachmentAsEntryState : public CBase, public MBaseTestState
/*
 * a state class that encapsulates the addition of a single obex
 * attachment to an existing message as a child entry
 */
	{
public:
	CAddObexAttachmentAsEntryState(const TDesC& aFileName, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	CObexClientTest& iClientTest;
    TFileName iFileName;
	};


//
// CAddObexAttachmentByNameState
//
class CAddObexAttachmentByNameState : public CBase, public MBaseTestState
/*
 * a state class that encapsulates the addition of a single obex
 * attachment to an existing message by adding the attachment's name to the message body
 */
	{
public:
	CAddObexAttachmentByNameState(const TDesC& aFileName, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	CObexClientTest& iClientTest;
    TFileName iFileName;
	};

//
// CAddObexAttachmentByNameWithHeadersState
//
class CAddObexAttachmentByNameWithHeadersState : public CBase, public MBaseTestState
/*
 * a state class that encapsulates the addition of a single obex
 * attachment to an existing message by adding the attachment's name to the message body,
 * the headers held in the client test will be added also
 */
	{
public:
	CAddObexAttachmentByNameWithHeadersState(const TDesC& aFileName, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	CObexClientTest& iClientTest;
	TFileName iFileName;
//	HBufC8* iFileName;
	};



#endif // __ADDOBEXATTACHMENTSTATE_H__
