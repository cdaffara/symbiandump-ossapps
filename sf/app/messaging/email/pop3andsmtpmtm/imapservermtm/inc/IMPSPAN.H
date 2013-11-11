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

#if !defined(__IMPSPAN_H__)
#define __IMPSPAN_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

/**
@internalComponent
@released
*/
enum TImpsPanic
	{
	ETxtcNoBodyText,
	EUnknownTag,
	EOpenWhenNotClosed,
	ESelectWhenNotReady,
	EFetchWhenNotSelected,
	EUnknownState,
	EAlreadySending,
	ESendingAlready,
	EConnectWhenConnected,
	EIOWhenNotReady,
	EAtomStackOverflow,
	EAtomStackUnderflow,
	ECantChangeUID,
	ECantChangeMsvId,
	EMsgnrOutOfRange,
	EIndexEmpty,
	ECreateWhenNotReady,
	ERenameWhenNotReady,
	EDeleteWhenNotReady,
	ESubscribeWhenNotReady,
	ECloseWhenNotSelected,
	ENotLoggedOn,
	ECopyNotFromSelectedFolder,
	ESyncWhenNotSelected,
	ENotImplementedYet,
	ECommandCompleteInUnknownState,
	ERunLInUnknownState,
	EUnknownEncodingMethod,
	EInvalidStatus,
	ECouldntFindFolder,
	ECopyWhenNotSelected,
	EDeleteOfUnknownType,
	ECannotGetMoveEntry,
	EUnknownMsvType,
	EInvalidService,
	EInvalidMsvTypeToCommand,
	EInvalidInbox,
	EBadUseOfImap4Op,
	EBadUseOfOffLineOp,
	KStorableAtomAlreadyUsed,
	KStorableAtomNotSet,
	KInvalidDummyComplete,
	KIncorrectNoOfUidsFromUidSearch,
	KSettingSeenFlagToExistingState,
	KErrorBuildingStoreFlagsCommand,
	KMaxBufferLengthExceeded,
	ECancelFetchWhenNotFetching
	};

GLREF_C void gPanic(TImpsPanic aPanic);

#endif
