// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CSMSSENDMESSAGEOPERATION_H__
#define __CSMSSENDMESSAGEOPERATION_H__


#include <msvapi.h>

/**
Encapsulates the parameters to schedule the sms send operation.

@publishedAll
@released
*/
NONSHARABLE_CLASS (CSmsSendMessageOperation) : public CMsvSendOperation
	{
public:
	static CSmsSendMessageOperation* NewL(CMsvSession& aMsvSession, 
									   	  const CMsvEntrySelection& aSelection, 
									   	  const TDesC8& aParameter, 
									   	  TRequestStatus& aObserverRequestStatus);
private:		
	void ConstructL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter);
	CSmsSendMessageOperation(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus);
	// from CMsvSendOperation		
	virtual const TDesC8& TranslateProgress(const TDesC8& aProgress);
	};


#endif // __CSMSSENDMESSAGEOPERATION_H__

