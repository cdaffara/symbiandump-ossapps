// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "t_testobserver.h"

/**
Constructor.
*/
CTestObserver::CTestObserver(CImapMailStore& aImapMailStore)
: iImapMailStore(aImapMailStore)
	{	
	}
	
/**
Destructor.
*/
CTestObserver::~CTestObserver()
	{
	}
	
void CTestObserver::CancelL()
	{
	iImapMailStore.CancelRequest(*this);
	}
	
void CTestObserver::StoreOperationComplete(TMsvId /*aId*/, TInt /*aErrorCode*/)
	{	
	}
	
/**
Fills in a CImHeader object
@param aImHeader the header to be filled.
*/

void CTestObserver::CreateMessageHeaderL(CImHeader& aImHeader)
	{
	aImHeader.Reset();
	aImHeader.SetSubjectL(KMiutSubject);
	aImHeader.SetImMsgIdL(KMiutMsgId);
	aImHeader.SetFromL(KMiutFrom);
	aImHeader.SetReplyToL(KMiutReplyTo);
	aImHeader.ToRecipients().AppendL(KMiutToRecipients1());
	aImHeader.ToRecipients().AppendL(KMiutToRecipients2);
	aImHeader.CcRecipients().AppendL(KMiutCcRecipients1);
	aImHeader.CcRecipients().AppendL(KMiutCcRecipients2);
	aImHeader.BccRecipients().AppendL(KMiutBccRecipients);
	}
	
void CTestObserver::MakeRequestL(TMsvId aId)
	{	
	CImHeader* header = CImHeader::NewLC(); 
	CreateMessageHeaderL(*header);	
	CleanupStack::Pop(header); 
	iImapMailStore.StorePartL(header,aId,*this);
	}

