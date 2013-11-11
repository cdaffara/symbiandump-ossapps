// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// IMAP4 utils.
// 
//

#include <e32base.h>
#include <msvstd.h>
#include <miuthdr.h>
#include <imcvutil.h>
#include <msventry.h>
#include <logwrap.h>

#include "impsutil.h"

CImap4Utils* CImap4Utils::NewL(CMsvServerEntry* aEntry) 
	{
	CImap4Utils* self=new (ELeave) CImap4Utils();
	CleanupStack::PushL(self);
	self->ConstructL(aEntry);
	CleanupStack::Pop();
	return self;
	}

CImap4Utils::~CImap4Utils()
	{
	delete iLogMessage;
	}

void CImap4Utils::ConstructL(CMsvServerEntry* aEntry)
	{
	iEntry = aEntry;
	
	// get the logger (discard errors)
	TRAPD(err,iLogMessage=CImLogMessage::NewL(iEntry->FileSession()));
	}

void CImap4Utils::ClearLogMessage()
	{
	if (iLogMessage)
		iLogMessage->LogEvent().SetLink(KLogNullLink);
	}

// Helper function to set up the iLogEntry object 
void CImap4Utils::SetUpLogMessageL(TMsvId aId)
	{
	if (iLogMessage)
		{
		TBuf<KLogMaxSharedStringLength> string;

		// if the entry isn't there then setup what we can of the log
		// message anyway
		if (iEntry->SetEntry(aId) == KErrNone)
			{
			TMsvEmailEntry entry = iEntry->Entry();
			iLogMessage->LogEvent().SetRemoteParty(entry.iDetails);
			iLogMessage->LogEvent().SetSubject(entry.iDescription);
			}
		else
			{
			iLogMessage->GetString(string, R_LOG_REMOTE_UNKNOWN);
			iLogMessage->LogEvent().SetRemoteParty(string);

			iLogMessage->GetString(string, R_LOG_SUBJECT_NONE);
			iLogMessage->LogEvent().SetSubject(string);
			}
		
		// Set up the log event.
		iLogMessage->LogEvent().SetEventType(KLogMailEventTypeUid);

		iLogMessage->GetString(string, R_LOG_DIR_FETCHED);
		iLogMessage->LogEvent().SetDirection(string);

		iLogMessage->LogEvent().SetLink(aId);
		}
	}

// Send the log message set up by SetUpLogMessageL()
TBool CImap4Utils::SendLogMessageL(TInt aError, TRequestStatus &aStatus)
	{
	if (iLogMessage && iLogMessage->LogEvent().Link() != KLogNullLink)
		{
		// Run the iLogMessage operation.
		iLogMessage->Start(aError, aStatus);

		return ETrue;
		}

	return EFalse;
	}

// eof IMPSUTIL.CPP
