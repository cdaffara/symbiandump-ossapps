// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SMSSPAN_H_
#define SMSSPAN_H_

#include <e32base.h>

const TInt KSmsSessionPriority = CActive::EPriorityStandard;

/**
@internalComponent
@released
*/
enum TSmssPanic
	{
//	KSmssPanicFileError,
	ESmssEntryNotSet,
	ESmssEntryAlreadySet,
	ESmssReportNotNull,
	ESmssWrongEntryMtmType,
	KSmssPanicUnexpectedCommand,
	KSmssPanicMtmAlreadyActive,
	KSmssPanicAlreadySending,
	KSmssPanicUnexpectedState,
	KSmssPanicUnexpectedErrorCode,
	KSmssPanicAlreadyUsingSimUtils,
	KSmssPanicNoMessagesInSelection,
	KSmssPanicDestinationFolderNotSet,
	KSmssPanicNoSmsServiceEntry,
	KSmssPanicBioTypeNotSet,
	KSmssPanicSendStateOutOfRange,
	KSmssPanicAlreadyActive,
	KSmssPanicLogIdNotSet,
	ESmssPanicPhoneNotFound,
	ESmssBadState,
	ESmssNoClass2Folder
	};

GLREF_C void Panic(TSmssPanic aPanic);

#endif
