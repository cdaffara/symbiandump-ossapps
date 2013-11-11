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

#ifndef __CIMATTACHMENTWAITER_H__
#define __CIMATTACHMENTWAITER_H__

#include <e32base.h>

class CImEmailMessage;

/**
An Waiter object used to wait for an attachment to be added to an email message.

@publishedAll
@released
*/
class CImAttachmentWaiter : public CActive
	{
public:
	IMPORT_C static CImAttachmentWaiter* NewL();
	IMPORT_C ~CImAttachmentWaiter();
	IMPORT_C void StartWaitingL(TRequestStatus& aStatus, CImEmailMessage* aMailMessage, TBool aDeleteMessage = ETrue);
	
private:
	CImAttachmentWaiter();
	void Reset();
	
	// from CActive
	void RunL();
	void DoCancel();
	
private:
	TRequestStatus* iReportStatus;
	CImEmailMessage* iMailMessage;
	TBool iDeleteMessage;
	};

#endif // __CIMATTACHMENTWAITER_H__
