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

#ifndef PBAPCHEXPORTER_H
#define PBAPCHEXPORTER_H

#include <e32base.h>
#include "pbapoperation.h"
#include "pbaprequest.h"
#include "pbapexporter.h"
#include "pbapchview.h"

// forward declarations
class CLogEvent;
class CLogFilter;
class CLogFilterList;
class CPbapLogViewEvent;
class CPbapExporterBase;


class CPbapChExporter : public CPbapExporterBase
	{
public:
	~CPbapChExporter();

protected:
	CPbapChExporter(MVirtualFolderClient& aClient, 
					MPbapExporterCallback& aCallback,
					CPbapChView& aViewToExport,
					CPbapChView* aReadViewToExport = NULL, 
					TUint aStartIndex = 0,
					TInt aCount = 0);
	void ConstructL();
	
	/** Notify that call history events are about to be fetched from the log
	@param aNumberOfEvents number of log events which will be fetched
	@return EFalse to abort reading*/
	virtual TBool PrepareForFetchingL(TUint aNumberOfEvents);
	
	/** successfully fetched another call history event from the log
	@return EFalse if no more events should be fetched */
	virtual TBool EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex) =0;
	
	/** Notify that fetching of call history events has completed*/
	virtual void FetchingCompleteL() =0;
		
private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	void StartExportL();
	void FetchLogEventsL();
	void FinaliseFetchLogEventsL();

private:
	enum TState
		{
		EIdle,
		EStartExport,
		EFetchingLogEvents
		};
		
private:	
	CPbapLogViewEvent* iLogView;
	CPbapLogViewEvent* iReadEventView;
	
	TInt iCurrentEventIndex;
	
	TState iState;
	
protected:
	TUint iStartIndex;
	TInt iCount;
	};


class CPbapChListingExporter : public CPbapChExporter
	{
public:
	static CPbapChListingExporter* NewL(MVirtualFolderClient& aClient, 
										MPbapExporterCallback& aCallback, 
										CPbapChView& aViewToExport,
										CPbapChView* aReadViewToExport,
										TUint aStartIndex, 
										TInt aCount);

protected:
	// from CPbapChExporter
	virtual TBool PrepareForFetchingL(TUint aNumberOfEvents);
	virtual TBool EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex);
	virtual void FetchingCompleteL();

private:
	CPbapChListingExporter(MVirtualFolderClient& aClient, 
						MPbapExporterCallback& aCallback, 
						CPbapChView& aViewToExport,
						CPbapChView* aReadViewToExport,
						TUint aStartIndex, 
						TInt aCount);
	};


class CPbapChItemExporter : public CPbapChExporter
	{
public:
	static CPbapChItemExporter* NewL(MVirtualFolderClient& aClient, 
									MPbapExporterCallback& aCallback, 
									CPbapChView& aViewToExport,
									CPbapChView* aReadViewToExport,
									TUint aStartIndex, 
									TInt aCount, 
									TVCardVersion aFormat, 
									TUint64 aFilter);

protected:
	// from CPbapChExporter
	virtual TBool EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex);
	virtual void FetchingCompleteL();

private:
	CPbapChItemExporter(MVirtualFolderClient& aClient, 
						MPbapExporterCallback& aCallback, 
						CPbapChView& aViewToExport,
						CPbapChView* aReadViewToExport,
						TUint aStartIndex, 
						TInt aCount, 
						TVCardVersion aFormat, 
						TUint64 aFilter);

private:
	TVCardVersion iFormat;
	TUint64 iFilter;
	};


class CPbapChSingleItemExporter : public CPbapChExporter
	{
public:
	static CPbapChSingleItemExporter* NewL(MVirtualFolderClient& aClient,
										   MPbapExporterCallback& aCallback,
										   CPbapChView& aViewToExport,
										   TInt aIndex,
										   TVCardVersion aFormat,
										   TUint64 aFilter);

protected:
	// from CPbapChExporter
	virtual TBool PrepareForFetchingL(TUint aNumberOfEvents);
	virtual TBool EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex);
	virtual void FetchingCompleteL();

private:
	CPbapChSingleItemExporter(MVirtualFolderClient& aClient,
							  MPbapExporterCallback& aCallback,
							  CPbapChView& aViewToExport,
							  TInt aIndex,
							  TVCardVersion aFormat,
							  TUint64 aFilter);

private:
	TInt iIndex;
	TVCardVersion iFormat;
	TUint64 iFilter;
	};

	
class CPbapChCountExporter : public CPbapChExporter
	{
public:
	static CPbapChCountExporter* NewL(MVirtualFolderClient& aClient, 
									MPbapExporterCallback& aCallback,
									CPbapChView& aViewToExport,
									CPbapChView* aReadViewToExport);
									

protected:
	// from CPbapChExporter
	virtual TBool PrepareForFetchingL(TUint aNumberOfEvents);
	virtual TBool EventFetchedL(const CLogEvent& aEvent, TInt aEventIndex);
	virtual void FetchingCompleteL();

private:
	CPbapChCountExporter(MVirtualFolderClient& aClient, 
						MPbapExporterCallback& aCallback,
						CPbapChView& aViewToExport,
						CPbapChView* aReadViewToExport);
	};

#endif // PBAPCHEXPORTER_H
