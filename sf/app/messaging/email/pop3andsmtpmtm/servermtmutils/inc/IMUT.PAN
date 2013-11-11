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



#if !defined (__BARSC_H__)
#include <barsc.h>		// RResourceFile
#endif

enum TImutPanic
	{
	EImskSocketNotConnected,
	EImskSocketStillActive,
	EImskSocketOpen,
	EImskUnknownState,
	EImskInvalidPortNumber,
	EImskInvalidSocketEncryptionMethod,
						KPanicNoBody,
						EPanicNullTokenizer,
						KPanicBoundaryTooLong,
						KPanicUnknownAlgorithm,
						KPanicInvalidSMTPLine,
						KPanicAttachmentPathNotSet,
						KPanicNoMimeClassAvailable,
						KPanicSetAlgorithmLNotCalled,
						KPanicNoResourceFile,
						KPanicDescriptorToSmall,
						KPanicReadLengthTooLarge,
						KPanicFileNotFlushed,
						KPanicEntryIsNotMessage,
						KPanicMessageEntryNotCalled,
						KPanicMessageEntryIdHasChanged,
						KPanicServiceIdNotValid
	};


// and some error codes
const TInt KErrCorruptHeader = 1;

GLDEF_C void gPanic( TImutPanic aReason );

GLREF_C void OpenResourceFileL(RResourceFile& aFile, RFs& anFs);

GLDEF_C void CloseResourceFile( TAny* aResourceFile );
