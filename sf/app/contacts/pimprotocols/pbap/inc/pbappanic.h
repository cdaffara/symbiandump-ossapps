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

#ifndef PBAPPANIC_H
#define PBAPPANIC_H
_LIT(KPbapServerPanic, "PBAP Server");

enum TPbapServerPanicCode
	{
	EPbapServerPanicBadDescriptor,
	EPbapServerPanicIllegalFunction,
	EPbapServerPanicUnexpectedError,
	EPbapServerPanicSDPRecordExists,
	EPbapServerPanicInvalidViewSortOrder,
	EPbapServerPanicNotReadyForExport,
	EPbapServerPanicInvalidTargetHeader,
	EPbapServerPanicExportInProgress,
	EPbapServerPanicNoCurrentFolder,
	EPbapServerPanicUnrecoverableObexError,
	EPbapServerPanicNameViewExists,
	EPbapServerPanicPhoneticViewExists,
	EPbapServerPanicInvalidViewSearchState,
	EPbapServerPanicInvalidFieldSearchState,
	EPbapServerPanicIndexOutOfBoundry,
	EPbapServerPanicOwnCardNotFound,
	EPbapServerPanicNullObserver,
	
	EVirtualFolderAlreadyInPlace,
	EVirtualFolderSubtreeAlreadyExists,
	EVirtualFolderExportInProgress,
	EVirtualFolderSearchInProgress,
	EVirtualFolderCachedIndexes,
	EVirtualFolderNullAppHeader,
	EVirtualFolderAppHeaderExists,
	EVirtualFolderIncompleteViews,
	EVirtualFolderNotReadyToExport,
	EVirtualFolderExportAlreadyPending,
	EVirtualFolderChExportAlreadyExists,
	EVirtualFolderPbExportAlreadyExists,
	
	EPbapChViewInconsistency,
	EPbapChCountExporterFetchingFails
	};


void Panic(TPbapServerPanicCode aPanic);

#endif // PBAPPANIC_H
