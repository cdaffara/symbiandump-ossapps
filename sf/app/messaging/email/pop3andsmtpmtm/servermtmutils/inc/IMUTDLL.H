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
//

#if !defined (__IMUTDLL_H__)
#define __IMUTDLL_H__

#if !defined (__E32STD_H__)
#include <e32std.h>
#endif

#if !defined (__BARSC_H__)
#include <barsc.h>		// RResourceFile
#endif

_LIT(KImutDllName,"IMUT");

/**
@internalComponent
@released
*/
enum TImutErrorReason {	KPanicNoRfc822Header = 1,
						KPanicNoRichText = 2,
						EPanicNullTokenizer = 3,
						KPanicBoundaryTooLong = 4,
						KPanicUnknownSendingMethod = 5,
						KPanicInvalidSMTPLine = 6,
						KPanicAttachmentPathNotSet = 7,
						KPanicNoMimeClassAvailable = 8,
						KPanicUnknownAlgorithm = 9,
						KPanicSetAlgorithmLNotCalled = 10,
						KPanicNoResourceFile = 11,
						KPanicDescriptorToSmall = 12,
						KPanicReadLengthTooLarge = 13,
						KPanicFileNotFlushed = 14,
						KPanicEntryIsNotMessage = 15,
						KPanicMessageEntryNotCalled = 16,
						KPanicMessageEntryIdHasChanged = 17,
						KPanicServiceIdNotValid = 18,
						KPanicIncorrectResetState = 19,
						KPanicNoMimeHeader = 20,
						KPanicInvalidDefaultCharacterSet = 21,
						KPanicUnknownFolderType = 22,
						KPanicInvalidEncodingType = 23,
						EPanicInvalidHeaderState = 24,
						EImskSocketNotConnected = 25,
						EImskSocketStillActive = 26,
						EImskSocketOpen = 27,
						EImskUnknownState = 28,
						EImskInvalidPortNumber = 29,
						EImskInvalidSocketEncryptionMethod = 30,
						EImskSecurityFailed = 31,
						EImskDatabaseTypeUnspecified = 32,
						EImskNotSupported = 33,
	 					EImskUnknownBearer = 34,
						EImskNoTLSResponseString = 35,
						EPanicInvalidCafState = 36,
						EPanicSchedulerAlreadyStarted = 37,
						EPanicCannotStopScheduler = 38,
						EPanicDataCommitted = 39,
						EPanicInvalidMobilityState = 40,
						EPanicNoSSLDomainName = 41
						};

// and some error codes
const TInt KErrCorruptHeader = 1;

GLDEF_C void gPanic( TImutErrorReason aReason );

GLREF_C void OpenResourceFileL(RResourceFile& aFile, RFs& anFs);

GLDEF_C void CloseResourceFile( TAny* aResourceFile );


#endif
