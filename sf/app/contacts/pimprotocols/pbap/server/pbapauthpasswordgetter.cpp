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

#include "pbapauthpasswordgetter.h"
#include "pbapserver.h"
#include "btaccesshostlog.h"


/*static*/ CPbapAuthPasswordGetter* CPbapAuthPasswordGetter::NewL(CPbapServer& aParent)
	{
	LOG_STATIC_FUNC
	CPbapAuthPasswordGetter* self = new (ELeave) CPbapAuthPasswordGetter(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbapAuthPasswordGetter::CPbapAuthPasswordGetter(CPbapServer& aParent)
: CActive(CActive::EPriorityUserInput), iParent(aParent)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CPbapAuthPasswordGetter::~CPbapAuthPasswordGetter()
	{
	LOG_FUNC
	Cancel();
	iNotifier.Close();
	}

void CPbapAuthPasswordGetter::ConstructL()
	{
	LOG_FUNC
	User::LeaveIfError(iNotifier.Connect());
	}

void CPbapAuthPasswordGetter::GetPassword(const TDesC& aRealm, const TBTDevAddr& aAddress)
	{
	LOG_FUNC
	
	// create parameter package
	TPbapAuthNotifierParamsPckg paramsPckg;
	paramsPckg().SetRealm(aRealm);
	paramsPckg().SetRemoteAddr(aAddress);
	
	// clear password buffer
	iResultsPckg().ResetPassword();

	// launch password notifier
	iNotifier.StartNotifierAndGetResponse(iStatus, KPbapAuthNotifierUid, paramsPckg, iResultsPckg);
	SetActive();
	}

void CPbapAuthPasswordGetter::RunL()
	{
	LOG_FUNC
	iNotifier.CancelNotifier(KPbapAuthNotifierUid);
	if (iResultsPckg().IsValidPassword())
		{
		iParent.HandlePasswordRetrieved(iStatus.Int(), iResultsPckg().Password());
		}
	else
		{
		iParent.HandlePasswordRetrieved(iStatus.Int(), KNullDesC);
		}
	}

void CPbapAuthPasswordGetter::DoCancel()
	{
	LOG_FUNC
	iNotifier.CancelNotifier(KPbapAuthNotifierUid);
	
	// inform parent that password will not be retrieved
	iParent.HandlePasswordRetrieved(KErrCancel, KNullDesC);
	}
