// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __MOBILITYTESTPROPERTYMAP_H__
#define __MOBILITYTESTPROPERTYMAP_H__

#include "timmobilitytestmtmstate.h"
#include "mobilitytestpropertyvalue.h"
#include "tmsgtestpropertymapentry.h"

const TMsgTestPropertyMapEntry propertyMapList[] = 
	{
		{"KNTSInitialValueFlag", ENTSInitialValueFlag},
		{"KNTSPreferredCarrierUpgradeCall", ENTSPreferredCarrierUpgradeCall},
		{"KNTSPreferredCarrierDowngradeCall", ENTSPreferredCarrierDowngradeCall},
		{"KNTSNewCarrierActiveCall", ENTSNewCarrierActiveCall},
		{"KNTSErrorCall", ENTSErrorCall},
		{"KNTSMigrateToPreferredCarrierFlag", ENTSMigrateToPreferredCarrierFlag},
		{"KNTSIgnorePreferredCarrierFlag", ENTSIgnorePreferredCarrierFlag},
		{"KNTSNewCarrierAcceptedFlag", ENTSNewCarrierAcceptedFlag},
		{"KNTSNewCarrierRejectedFlag", ENTSNewCarrierRejectedFlag},
		{"KNTSPreferredCarrierUpgradeFlag", ENTSPreferredCarrierUpgradeFlag},
		{"KNTSPreferredCarrierDowngradeFlag", ENTSPreferredCarrierDowngradeFlag},
		{"KNTSCancelFlag", ENTSCancelFlag},
		{"KNTSMigrationCompleteMigrateOKFlag", ENTSMigrationCompleteMigrateOKFlag},
		{"KNTSMigrationCompleteCouldNotReconnectFlag", ENTSMigrationCompleteCouldNotReconnectFlag},
		{"KNTSMigrationCompleteMobilityErrorFlag", ENTSMigrationCompleteMobilityErrorFlag},				
		{"KNTSAcceptImmediatelyCall", ENTSAcceptImmediatelyCall},
		{"KNTSAcceptStopCurrentCall", ENTSAcceptStopCurrentCall},
		{"KNTSAcceptCompleteCurrentCall", ENTSAcceptCompleteCurrentCall},
		{"KNTSIgnoreCall", ENTSIgnoreCall},
		{"KNTSInitialReject",					KMobilityTestMtmStateInitialReject},
		{"KNTSImapConnect",					KMobilityTestMtmStateImapConnect},
		{"KNTSImapIdle",						KMobilityTestMtmStateImapIdle},
		{"KNTSImapCopyFromLocal1",			KMobilityTestMtmStateImapCopyFromLocal1},
		{"KNTSImapCopyFromLocal2",			KMobilityTestMtmStateImapCopyFromLocal2},
		{"KNTSImapCopyFromLocal3",			KMobilityTestMtmStateImapCopyFromLocal3},
		{"KNTSImapCopyToLocal1",				KMobilityTestMtmStateImapCopyToLocal1},
		{"KNTSImapCopyToLocal2",				KMobilityTestMtmStateImapCopyToLocal2},
		{"KNTSImapCopyToLocal3",				KMobilityTestMtmStateImapCopyToLocal3},
		{"KNTSImapCopyToLocal4",				KMobilityTestMtmStateImapCopyToLocal4},
		{"KNTSImapCopyToLocal5",				KMobilityTestMtmStateImapCopyToLocal5},
		{"KNTSImapCopyToLocal6",				KMobilityTestMtmStateImapCopyToLocal6},
		{"KNTSImapCopyToLocal7",				KMobilityTestMtmStateImapCopyToLocal7},
		{"KNTSImapCopyWithinService1",		KMobilityTestMtmStateImapCopyWithinService1},
		{"KNTSImapCopyWithinService2",		KMobilityTestMtmStateImapCopyWithinService2},
		{"KNTSImapCopyWithinService3",		KMobilityTestMtmStateImapCopyWithinService3},
		{"KNTSImapCopyWithinService4",		KMobilityTestMtmStateImapCopyWithinService4},
		{"KNTSImapCopyWithinService5",		KMobilityTestMtmStateImapCopyWithinService5},
		{"KNTSImapCopyWithinService6",		KMobilityTestMtmStateImapCopyWithinService6},
		{"KNTSImapCopyWithinService7",		KMobilityTestMtmStateImapCopyWithinService7},
		{"KNTSImapCopyWithinService8",		KMobilityTestMtmStateImapCopyWithinService8},
		{"KNTSImapCreateFolder",				KMobilityTestMtmStateImapCreateFolder},
		{"KNTSImapDelete1",					KMobilityTestMtmStateImapDelete1},
		{"KNTSImapDelete2",					KMobilityTestMtmStateImapDelete2},
		{"KNTSImapDelete3",					KMobilityTestMtmStateImapDelete3},
		{"KNTSImapDelete4",					KMobilityTestMtmStateImapDelete4},
		{"KNTSImapDelete5",					KMobilityTestMtmStateImapDelete5},
		{"KNTSImapDeleteFolder1",				KMobilityTestMtmStateImapDeleteFolder1},
		{"KNTSImapDeleteFolder2",				KMobilityTestMtmStateImapDeleteFolder2},
		{"KNTSImapDeleteFolder3",				KMobilityTestMtmStateImapDeleteFolder3},
		{"KNTSImapDeleteFolder4",				KMobilityTestMtmStateImapDeleteFolder4},
		{"KNTSImapDisconnect1",				KMobilityTestMtmStateImapDisconnect1},
		{"KNTSImapDisconnect2",				KMobilityTestMtmStateImapDisconnect2},
		{"KNTSImapRename1",					KMobilityTestMtmStateImapRename1},
		{"KNTSImapRename2",					KMobilityTestMtmStateImapRename2},
		{"KNTSImapSyncFolder1",				KMobilityTestMtmStateImapSyncFolder1},
 		{"KNTSImapSyncFolder2",				KMobilityTestMtmStateImapSyncFolder2},
 		{"KNTSImapSyncFolderTree",			KMobilityTestMtmStateImapSyncFolderTree},
		{"KNTSImapSyncEInboxEarlyDeletes",	KMobilityTestMtmImapSyncEInboxEarlyDeletes},
		{"KNTSImapSyncEInboxEarlyExpunge",	KMobilityTestMtmImapSyncEInboxEarlyExpunge},
		{"KNTSImapSyncEInboxSync",			KMobilityTestMtmImapSyncEInboxSync},
		{"KNTSImapSyncESyncTree",				KMobilityTestMtmImapSyncESyncTree},
		{"KNTSImapSyncESyncSubscriptions",	KMobilityTestMtmImapSyncESyncSubscriptions},
		{"KNTSImapSyncEFolderEarlyDeletes",	KMobilityTestMtmImapSyncEFolderEarlyDeletes},
		{"KNTSImapSyncEFolderEarlyExpunge",	KMobilityTestMtmImapSyncEFolderEarlyExpunge},
		{"KNTSImapSyncESyncFolder",			KMobilityTestMtmImapSyncESyncFolder},
		{"KNTSImapEInboxLateDeletes",			KMobilityTestMtmImapEInboxLateDeletes},
		{"KNTSImapSyncEFolderLateDeletes",	KMobilityTestMtmImapSyncEFolderLateDeletes},
		{"KNTSImapSyncEFolderLateExpunge",	KMobilityTestMtmImapSyncEFolderLateExpunge},
		{"KNTSPopConnecting",					KMobilityTestMtmStatePopConnecting},
		{"KNTSPopRefreshing",					KMobilityTestMtmStatePopRefreshing},
		{"KNTSPopTidying",					KMobilityTestMtmStatePopTidying},
		{"KNTSPopFindingFirstOfflineOp",		KMobilityTestMtmStatePopFindingFirstOfflineOp},
		{"KNTSPopCancellingOfflineOps",		KMobilityTestMtmStatePopCancellingOfflineOps},
		{"KNTSPopCopying",					KMobilityTestMtmStatePopCopying},
		{"KNTSPopMoving",						KMobilityTestMtmStatePopMoving},
		{"KNTSPopPopulating",					KMobilityTestMtmStatePopPopulating},
		{"KNTSPopTopPopulating",				KMobilityTestMtmStatePopTopPopulating},
		{"KNTSPopDeleting",					KMobilityTestMtmStatePopDeleting},
		{"KNTSPopConnectedAndIdle",			KMobilityTestMtmStatePopConnectedAndIdle},
		{"KNTSPopQuitting", KMobilityTestMtmStatePopQuitting},
		{"KNTSPopRetrieving", KMobilityTestMtmStatePopRetrieving},
		{"KNTSSmtpConnectingToSmtp",			KMobilityTestMtmStateSmtpConnectingToSmtp},
		{"KNTSSmtpWaitingForReply",			KMobilityTestMtmStateSmtpWaitingForReply},
		{"KNTSSmtpAuthorisingSmtp",			KMobilityTestMtmStateSmtpAuthorisingSmtp},
		{"KNTSSmtpAuthInProgress",			KMobilityTestMtmStateSmtpAuthInProgress},
		{"KNTSSmtpSendingStartTls",			KMobilityTestMtmStateSmtpSendingStartTls},
		{"KNTSSmtpSettingSecurity",			KMobilityTestMtmStateSmtpSettingSecurity},
		{"KNTSSmtpResetSmtp",					KMobilityTestMtmStateSmtpResetSmtp},
		{"KNTSSmtpClosingSmtp",				KMobilityTestMtmStateSmtpClosingSmtp},
		{"KNTSSmtpSendFile",					KMobilityTestMtmStateSmtpSendFile},
		{"", 0xffffffff} // This must always be the last entry in the list
	};

#endif //__MOBILITYTESTPROPERTYMAP_H__
