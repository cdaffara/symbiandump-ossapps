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

#ifndef PBAPEXPORTER_H
#define PBAPEXPORTER_H

#include "pbaprequest.h"
#include <cntdb.h>

class TDesC8;
class CContactItem;
class CLogEvent;
class MVirtualFolderClient;

class MPbapExporter
	{
public:
	virtual void StartExport() =0;
	virtual void ExportListingBeginL() =0;
	virtual void ExportListingEntryL(TInt aHandle, const TDesC& aName) =0;
	virtual void ExportListingEndL() =0;	
	virtual void ExportCallHistoryL(const CLogEvent& aEvent, TVCardVersion aFormat, TUint64 aFilter) =0;
	virtual void ExportContactL(TContactItemId aContactId, TVCardVersion aFormat, TUint64 aFilter) =0;
	virtual void ExportEmptyVCardL(TVCardVersion aFormat) =0;
	virtual void ExportPhonebookSizeL(TInt aCount) =0;
	virtual void ExportNewMissedCallsL(TInt aCount) =0;
	virtual void FinaliseExportL() =0;
	virtual void CancelExport() =0;
	};
	
	
class MPbapExporterCallback
	{
public:
	// Exporter is deleted asynchronously
	virtual void HandleExportComplete(TInt aError) =0;
	};

class CPbapExporterBase : public CActive
	{
protected:
	inline CPbapExporterBase(MVirtualFolderClient& aClient, MPbapExporterCallback& aCallback);

protected:
	MVirtualFolderClient& iClient;
	MPbapExporterCallback& iCallback;
	};
	
inline CPbapExporterBase::CPbapExporterBase(MVirtualFolderClient& aClient, MPbapExporterCallback& aCallback)
: CActive(EPriorityStandard),
  iClient(aClient),
  iCallback(aCallback)
	{
	}
#endif //PBAPEXPORTER_H
