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

enum TPopsPanic
	{
	EImppNonExistentPopMessage = 0,
	EImppBabelMsgDoesNotExist = 1,
	EImppBadSessionState = 2,
	EImppOperationAlreadyPending = 3,
	EImppBabelMsgArrayNotDefined = 4,
	EImppBufferNotTerminated = 5,
	EImppImskBufferError = 6,
	EImppAlreadyActive = 7,
	EImppUnknownRefreshState = 8,
	ETopStartBeforeUidlStart = 9,
	EPopNotConnectedToMbox = 10,
	EPopCurrentContextIsNotService = 11,
	EPopCurrentContextIsNullEntry = 12,
	EPopFailedDebugAssert = 13,
	EPopInvalidState = 14,
	EPopNullPointer = 15,
	EPopNewCarrierUnknownState = 16,
	EPopSessionManagerInvalidState = 17,
	EPopUnexpectedMigrateState = 18,
	EPopRestartUnexpectedOp = 19,
	EPopCompleteOpUnexpectedOp = 20,
	EPopStopOpUnexpectedOp = 21,
	EPopUnexpectedNewCarrierActive = 22

	};

extern void Panic(TPopsPanic aPanic); // source in IMPP.cpp
