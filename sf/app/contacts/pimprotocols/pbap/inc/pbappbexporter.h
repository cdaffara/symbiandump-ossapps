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

#ifndef PBAPPBEXPORTER_H
#define PBAPPBEXPORTER_H

#include <e32base.h>
#include "pbaprequest.h"
#include "pbappbhandlecache.h"
#include "pbapexporter.h"

// forward declarations
class CPbapExporterBase;

typedef RArray<TInt> RIndexArray;

/**
Asynchronous exporter base class
*/
class CPbapPbExporter : public CPbapExporterBase
	{
public:
	~CPbapPbExporter();

protected:
	CPbapPbExporter(MVirtualFolderClient& aClient, MPbapExporterCallback& aCallback);
	void ConstructL();
	
	virtual TBool DoExportItemL() =0;

private:
	void Queue();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	};


/**
Exports Pb vCard listings
*/
class CPbapPbListingExporter : public CPbapPbExporter
	{
public:
	static CPbapPbListingExporter* NewL(MVirtualFolderClient& aClient, 
										MPbapExporterCallback& aCallback, 
										CPbapPbHandleCache& aHandles, 
										RIndexArray& aCacheIndexesToExport);

protected:
	// from CPbapPbExporter
	virtual TBool DoExportItemL();

private:
	CPbapPbListingExporter(MVirtualFolderClient& aClient, 
						MPbapExporterCallback& aCallback, 
						CPbapPbHandleCache& aHandles, 
						RIndexArray& aCacheIndexesToExport);

private:
	CPbapPbHandleCache& iHandles;
	RIndexArray& iCacheIndexesToExport;
	TInt iCurrentIndex;
	};


/**
Exports Pb phonebook vCards
*/
class CPbapPbItemExporter : public CPbapPbExporter
	{
public:
	static CPbapPbItemExporter* NewL(MVirtualFolderClient& aClient, 
									MPbapExporterCallback& aCallback, 
									CPbapPbHandleCache& aHandles, 
									TInt aStartIndex, 
									TInt aCount, 
									TVCardVersion aFormat, 
									TUint64 aFilter);

protected:
	// from CPbapPbExporter
	virtual TBool DoExportItemL();

private:
	CPbapPbItemExporter(MVirtualFolderClient& aClient, 
						MPbapExporterCallback& aCallback, 
						CPbapPbHandleCache& aHandles, 
						TInt aStartIndex, 
						TInt aCount, 
						TVCardVersion aFormat, 
						TUint64 aFilter);

private:
	CPbapPbHandleCache& iHandles;
	TInt iStartIndex;
	TInt iCount;
	TVCardVersion iFormat;
	TUint64 iFilter;
	TInt iCurrentIndex;
	};



/**
Exports Pb phonebook size
*/
class CPbapPbCountExporter : public CPbapPbExporter
	{
public:
	static CPbapPbCountExporter* NewL(MVirtualFolderClient& aClient, 
									MPbapExporterCallback& aCallback, 
									TInt aCount);

protected:
	// from CPbapPbExporter
	virtual TBool DoExportItemL();

private:
	CPbapPbCountExporter(MVirtualFolderClient& aClient, 
						MPbapExporterCallback& aCallback, 
						TInt aCount);

private:
	TInt iCount;
	};
	
#endif //PBAPPBEXPORTER_H
