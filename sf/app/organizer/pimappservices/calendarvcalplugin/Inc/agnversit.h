// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/

#ifndef __AGNVERSIT_H__
#define __AGNVERSIT_H__

#include <e32base.h>
#include <e32std.h>
#include <vcal.h>
#include <tz.h>
#include <caldataexchange.h>

#include "agmvcal.h"
#include "calversit.h"

class TTzRule;
class CAgnVCalConverter;

class CCalImportExportActive : public CActive 
	{
public:
	CCalImportExportActive(CAgnVCalConverter& aConverter);
	~CCalImportExportActive();
	void Start();

	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	CAgnVCalConverter& iConverter;
	};

class CAgnVCalConverter : public CAgnVersit
	{
public:

	enum TState
		{
		EIdle,
		EImporting,
		EExporting,
		};

	static CAgnVCalConverter* NewL();

//from MAgnVersit
	void ImportVCalL(TInt aFlags, RReadStream& aReadStream, RPointerArray<CCalEntry>& aEntryArray);
	void ExportVCalL(RWriteStream& aWriteStream, const RPointerArray<CCalEntry>& aEntryArray, const Versit::TVersitCharSet aCharSet);

	~CAgnVCalConverter();
	
	void ImportVCalAsyncL(TInt aFlags, RReadStream& aReadStream, RPointerArray<CCalEntry>& aEntryArray, MCalDataExchangeCallBack& aObserver);
	void ExportVCalAsyncL(RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray, MCalDataExchangeCallBack& aObserver, const Versit::TVersitCharSet aCharSet);
//To be called by CCalImportExportActive
	void NextExportStepL();
	void NextImportStepL();
	void CancelImportExport(TInt aError = KErrNone);
	CAgnVCalConverter::TState AsyncState() const;
private:
	void ConstructL();

	CTzRules* ImportTzRulesL(CParserVCal& aCalParser) const;
	CTzRules* DaylightRulesL(CParserVCal& aCalParser, TTimeIntervalMinutes aStdOffset) const;
	TTzRule TzRuleL(TTimeIntervalMinutes aOldTzOffset, TTimeIntervalMinutes aNewTzOffset, TTime aStartTime, TTime aEndTime) const;
	TTimeIntervalMinutes StandardTimeOffsetL(CParserVCal& aCalParser) const;
	TBool ValidDaylightProperty(CParserProperty& aDaylightProperty) const;
	void ConvertEntryToVCalAndExportL(CParserVCal* aVcal, const RPointerArray<CCalEntry>& aEntryArray, RWriteStream& aWriteStream);
	void ExportEntryBySameTzL(RWriteStream& aWriteStream);
	void ExportEntryWithoutTzRuleL(RWriteStream& aWriteStream);
	void FindEntryWithoutTzRuleL(RPointerArray<CCalEntry>& aSameTzEntry);
	void FindSameTzEntryL(RPointerArray<CCalEntry>& aSameTzEntry);
	void CheckCompletion();
	void InitialiseSameTzEntryArrayL();
private:
	TBool iTzConnected;
	RTz iTzServer;

	// for asynchronous import / export
	MCalDataExchangeCallBack* 	iObserver;

	CCalImportExportActive*		iImportExportActive;
	RPointerArray<CCalEntry>* 	iEntryArray;		//Not owned
	RPointerArray<CCalEntry>* 	iSametzEntries;		//Not owned. The entries with same tz rule or do have tz rules
	RPointerArray<CParserVCal>* iVCalArray;			//Array of vCal used when importing asynchronously. 
	RArray<TInt> 				iEntryIndexToExport;//Each of its element is the index of the entries in the array of iEntryArray
	
	CVCalToAgendaEntryConverter* 	iImportConverter;
	CAgendaEntryToVCalConverter* 	iExportConverter;
	Versit::TVersitCharSet 			iCharSet;

	RWriteStream* 	iWriteStream;
	CParserVCal* 	iVCal;
	TState 			iAsyncState;

	TInt 			iTotalNumEntry;		//The number of entries
	TInt 			iTotalCounter;		//The counter out of total entries
	TInt 			iSameTzCounter;		//The counter out of number of entries with the particular tz
	TInt 			iMaxNumEntryInStep;	//The maxum number of entries to import\export in one step
	};

/**
@internalComponent
@released 
*/
enum TAgnVersitPanic
	{
	EAgnVersitPanicNotNull,
	EAgnVersitPanicNoEntryArray,
	EAgnVersitPanicNoAAlarmProps,
	EAgnVersitPanicWrongTimeType,
	EAgnVersitWrongAttribute,
	EAgnVersitPanicWrongArgument,
	EAgnVersitPanicImportExportFailure
	};

extern void Panic(TAgnVersitPanic aPanic);

#endif
