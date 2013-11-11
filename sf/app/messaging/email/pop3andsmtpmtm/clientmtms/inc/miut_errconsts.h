// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// miut_errconsts.h
//

/**
 * @file 
 * @internalComponent
 * @released
 */

#ifndef __MIUT_ERRCONSTS_H__
#define __MIUT_ERRCONSTS_H__

/**
@internalComponent
@released
*/
enum TImcmPanic
	{
	EOffOpNoBodyText = 0,
	EOffOpBadMtmTypeUid = 1,
	EOffOpNoCMsvEntrySet = 2,
	EOffOpEntryShouldBeFolder = 3,
	EOffOpTwoSameLinesNotAllowed = 4,
	EOffOpNoMemory = 5,
	EOffOpInvalidConstruction = 6,
	EOffOpListOutOfSync = 7,
	EOffOpNoMtmParameters = 8,
	EMiutBadStreamVersion = 9,
	EEntryIsNotMessage = 10,
	EMiutArrayHasOddNumberOfElements = 11,		// ie there should be even number of entries in array
	EMiutLocalServiceIdSet = 12,				// iServiceId should not be set to Local
	EPopcMTMNoBodyText = 13,
	EPopcMTMBadMtmTypeUid = 14,
	EPopcMTMNoCMsvEntrySet = 15,
	EPopcMTMOperationNULL = 16,
	EPopcMTMNotAServiceEntry = 17,
	EPopcUnknownNewEmailType = 18,
	EPopcUnknownSyncFunction = 19,
	ESmtcMTMNoBodyText = 20,
	ESmtcMTMBadMtmTypeUid = 21,
	ESmtcMTMNoCMsvEntrySet = 22,
	ESmtcMTMOperationNULL = 23,
	ESmtcMTMNotAServiceEntry = 24,
	ESmtcUnknownNewEmailType = 25,
	ESmtcUnknownSyncFunction = 26,
	ESmtcMTMStoringBadEntryType = 27,
	ESmtcMTMRestoringBadEntryType = 28,
	ESmtcMTMNotAMessageEntry = 29,
	EMiutActiveInFinalProgress = 30,
	EImpcNoBodyText = 31,
	EImpcBadMtmTypeUid = 32,
	EImpcNoCMsvEntrySet = 33,
	EImpcEntryShouldBeFolder = 34,
	EImpcTwoSameLinesNotAllowed = 35,
	EImpcNoMemory = 36,
	EImpcInvalidConstruction = 37,
	EImpcBadServiceEntry = 38,
	EImpcNoBaseMtm = 39,
	EImpcNoAddresseeList = 40,
	EImpcInvalidProgress = 41,
	EImpcNotSupported = 42,
	EImpcMTMNotAMessageEntry = 43,
	EImpcMTMNotAServiceEntry = 44,
	EImcmMessageEntryHasMoreThanOneFolder = 45,
	EImcmUnknownState = 46,
	EImcmFinalProgressFailed = 47,
	EImcmNoEikonEnvironment = 48,
	EImcmSetVersionNotSupported = 49,
	EImcmSystemDefaultCharsetNotSupported = 50,
 	EImcmHtmlConverterNotFound = 51,
	EImcmUnknownMtm = 52,
	EImEmailOpUnknownAttachmentType = 53,
	EImcmNonIdleRetrievalState = 54,
	EImcmIdleRetrievalState = 55,
	EImcmSyncDownloadRulesInvalidPos1 = 56,
	EImcmSyncDownloadRulesInvalidPos2 = 57,
	EImcmSyncDownloadRulesInvalidPos3 = 58,
	EImcmSyncDownloadRulesInvalidBearerTypes1 = 59,
	EImcmSyncDownloadRulesInvalidBearerTypes2 = 60,
	EImcmSyncDownloadRulesInvalidBearerTypes3 = 61,
	EImcmTransportBufferSizesInvalidPos1 = 62,
	EImcmTransportBufferSizesInvalidPos2 = 63,
	EImcmTransportBufferSizesInvalidPos3 = 64,
	EImcmTransportBufferSizesInvalidBearerTypes1 = 65,
	EImcmTransportBufferSizesInvalidBearerTypes2 = 66,
	EImcmTransportBufferSizesInvalidBearerTypes3 = 67,
	EImcmTransportBufferSizesInvalidFetchRequestSize = 68,
	EImcmTransportBufferSizesInvalidMaxOutstandingFetchResponses = 69,
	EImcmMobilityAccountsListInvalidPos1 = 70,
	EImcmMobilityAccountsListInvalidPos2 = 71,
	EImcmMobilityAccountsListInvalidPos3 = 72,
	EImcmMobilityAccountsListInvalidIAPListCount = 73
	};

/**
@internalComponent
*/
GLREF_C void gPanic(TImcmPanic aPanic);

#endif // __MIUT_ERRCONSTS_H__
