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

#ifndef PBAPFOLDERNODEPB_H
#define PBAPFOLDERNODEPB_H

#include "pbapfoldernode.h"
#include "pbapcontactdbviews.h"
#include "pbappbhandlecache.h"
#include "pbappbexporter.h"
#include "pbapexporter.h"

//forward declarations
class MVirtualFolderClient;
class CPbapAppHeader;
class CContactIdArray;
class MPbapExporterCallback;

	
class CFolderNodePb : public CFolderNode, public MPbHandleCacheObserver,
										public MPbapContactDbViewsCallback,
										public MPbapExporterCallback
	{
public:
	static CFolderNodePb* NewL(MVirtualFolderClient& aClient);
	~CFolderNodePb();
	
private:
	CFolderNodePb(MVirtualFolderClient& aClient);
	void ConstructL();

	TInt HandleCount();
	
	void SearchAndSortItemsL();
	void ExportIndexSortedListingL();
	void ExportSearchAndSortResultsL();

	void Reset();
	
private:
	// from CFolderBase
	virtual void CancelGet();
	virtual void GetComplete();
	
	// from CFolderNode
	virtual TInt DoGetItem(TInt aHandle);
	virtual TInt DoGetListing();
	virtual TInt DoGetCount();
	virtual TInt DoGetFolder();
	
	// from MPbHandleCacheObserver
	virtual void HandleCacheChanged();
	
	// from MPbapContactDbViewsCallback
	virtual void HandleSearchAndSortComplete(TInt aError);
	
	// from MPbapExporterCallback
	virtual void HandleExportComplete(TInt aError);

private:
	CPbapPbHandleCache*	iHandleCache;
	TBool iPhonebookChanged;
	CContactIdArray* iSearchResults;
	
	CPbapPbExporter* iAsyncExporter;
	RArray<TInt> iCacheIndexesToExport;
	};


#endif //PBAPFOLDERNODEPB_H
