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

#include "cimaputils.h"
#include "cimaplogger.h"
#include "cimapcharconv.h"

/**
Create the IMAP utilities class.
There is only one instance of this class at any one time. If more than one
server MTM instance exists at any time, then they share this class.
This is a static function.
*/
EXPORT_C void CImapUtils::CreateL()
	{
	CImapUtils* self = NULL;

	TAny* tls = Dll::Tls();
	if (tls)
		{
		self = static_cast<CImapUtils*>(tls);
		++self->iRefCount;
		}
	else
		{
		self = new (ELeave) CImapUtils();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop();
		Dll::SetTls(self);
		self->iRefCount = 1;
		}
	}

/**
Delete the IMAP utilities class.
The class uses a reference count to ensure that there is only ever one instance
of the class at any time. Once the reference count hits 0, the class is destroyed.
This is a static function.
*/
EXPORT_C void CImapUtils::Delete()
	{
	TAny* tls = Dll::Tls();

	if (tls)
		{
		CImapUtils* self = static_cast<CImapUtils*>(tls);
		--self->iRefCount;
		if (self->iRefCount == 0)
			{
			delete self;
			Dll::SetTls(NULL);
			}
		}
	}

/**
Get a reference to the IMAP utilities class.
This is a static function.

@return Reference to IMAP utilities class.
*/
EXPORT_C CImapUtils& CImapUtils::GetRef()
	{
	TAny* tls = Dll::Tls();	

	__ASSERT_DEBUG(tls != NULL, User::Invariant());

	CImapUtils* utils = static_cast<CImapUtils*>(tls);
	return *utils;
	}

CImapUtils::CImapUtils()
	{
	}

void CImapUtils::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLogger = CImapLogger::NewL();
	iCharconv = CImapCharconv::NewL(iFs);
	}

CImapUtils::~CImapUtils()
	{
	delete iLogger;
	delete iCharconv;
	iFs.Close();
	}

/**
Gets a connected file system object.

@return a connected file system object.
*/
EXPORT_C RFs& CImapUtils::Fs()
	{
	return iFs;
	}

/**
Gets a reference to the logger utility class.

@return Reference to the logger utility class.
*/
EXPORT_C CImapLogger& CImapUtils::Logger()
	{
	return *iLogger;
	}

/**
Gets a reference to the character converter utility class.

@return reference to the character converter utility class.
*/
EXPORT_C CImapCharconv& CImapUtils::Charconv()
	{
	return *iCharconv;
	}

/**
The panic function for the imapserver.
@param aPanic The panic code.
*/
EXPORT_C void TImapServerPanic::ImapPanic(TImpsPanic aPanic)
	{
	_LIT(KImapServerPanic, "IMAPServer");
	User::Panic(KImapServerPanic(), aPanic);
	}
