// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agnversit.h"
#include <vtzrules.h>
#include <ecom/implementationproxy.h>
#include <calrrule.h>
#include <calentry.h>
#include <caldataformat.h>

/**
@internalComponent
*/
template<class T> void CalCleanup<T>::PushResetAndDestroyL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, &aRef));
	}
	
/**
@internalComponent
*/
template<class T> void CalCleanup<T>::PushResetL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(Reset, &aRef));
	}

/**
@internalComponent
*/	
template <class T> void CalCleanup<T>::ResetAndDestroy(TAny* aPtr)
	{
	T* self = static_cast<T*>(aPtr);
	if (self)
		{
		self->ResetAndDestroy();
		}
	}
	
/**
@internalComponent
*/	
template <class T> void CalCleanup<T>::Reset(TAny* aPtr)
	{
	T self = static_cast<T>(aPtr);
	if (self)
		{
		self->Reset();
		}
	}
	
/** Global function for ease of use of CalCleanup template class
@internalComponent
*/
template<class T> void CleanupResetAndDestroyPushL(T& aRef)
	{
	CalCleanup<T>::PushResetAndDestroyL(aRef);
	}
	
/** Global function for ease of use of CalCleanup template class
@internalComponent
*/
template<class T> void CleanupResetPushL(T& aRef)
	{
	CalCleanup<T>::PushResetL(aRef);
	}


/**
 * Imports a <code>vCalendar</code> consisting of one or more
 * <code>vCalendar</code> entities.
 *
 * The <code>vCalendar</code> is read from the read stream specified and
 * its entities are converted into EPOC agenda entries and returned in
 * the array <code>aEntryArray</code>.
 *
 * @param     "RReadStream& aReadStream"
 *            The read stream which contains the
 *            <code>vCalendar</code>.
 * @param     "CArrayPtr<CCalEntry>* aEntryArray"
 *            On return, a list of the agenda entries which were
 *            imported from the <code>vCalendar</code>. The list
 *            contains the <code>vEvents</code> followed by the
 *            <code>vTodos</code>.
 */
void CAgnVCalConverter::ImportVCalL(TInt aFlags, RReadStream& aReadStream, RPointerArray<CCalEntry>& aEntryArray)

	{
	// Create a new converter object
	CVCalToAgendaEntryConverter* converter = new(ELeave)CVCalToAgendaEntryConverter();
	CleanupStack::PushL(converter);

	// Create a parser to internalize the entries
	CParserVCal* vCal = CParserVCal::NewL();
	CleanupStack::PushL(vCal);

	// Japan uses SHIFT-JIS as the default charset contrary to 
	// the vCalendar Specification which says that the default 
	// charset should be US-ASCII. 
	// In order to provide interoperability with this market
	// segment, allow a flag to override the default charset in versit.
	if(aFlags & KCalDataExchangeDefaultShiftJIS)
	    {
	    vCal->SetDefaultCharSet(Versit::EShiftJISCharSet);
	    vCal->SetFlags(CVersitParser::EUseDefaultCharSetForAllProperties);
	    }
	vCal->InternalizeL(aReadStream);

	
	// Set the time zone converter.  iTzConverter may be NULL if the optional
	// time zone conversion service is not available.  In this case all imported times 
	// will be exported as local time.
	if (iTzConnected)
		{
		converter->SetTzConverter(&iTzServer);
		}
	
	if (aFlags & KCalDataExchangeImportStatusAsVCalendar)
		{
		converter->SetImportVCalendarValues(ETrue);
		}
	else
		{
		converter->SetImportVCalendarValues(ETrue);
		}

	converter->SetTzRules(ImportTzRulesL(*vCal)); // converter takes ownership of rules collection.
	
	CArrayPtr<CVersitParser>* entities = vCal->ArrayOfEntities(EFalse); //doesn't take ownership
	if (entities)
		{
		TInt count = entities->Count();
		for (TInt i=0; i<count; i++)
			{
			CVersitParser& parser = *((*entities)[i]);			
			converter->ImportVCalL(parser, aEntryArray);
			} 
		}

	CleanupStack::PopAndDestroy(vCal);
	CleanupStack::PopAndDestroy(converter);
	}


/**
 * Exports a list of agenda entries as multiple vCalendar entities to the
 * write stream specified.
 *
 * The entries are exported either as vTodos, if they are to-dos,
 * otherwise as vEvents
 *
 * @param     "RWriteStream& aWriteStream"
 *            The stream to which the agenda entries should be
 *            externalised as vCalendar entities.
 * @param     "CArrayPtr<CCalEntry>* aEntryArray"
 *            Pointer to the array of agenda entries.
 * @param     "const Versit::TVersitCharSet aCharSet"
 *             The character encoding type
 */
void CAgnVCalConverter::ExportVCalL(RWriteStream& aWriteStream, const RPointerArray<CCalEntry>& aEntryArray, const Versit::TVersitCharSet aCharSet)

	{
	if(aEntryArray.Count()>0)
		{
		iCharSet = aCharSet;
		// Create a new converter object
		if(!iExportConverter)
			{
			iExportConverter = new(ELeave)CAgendaEntryToVCalConverter;
			}
		iEntryArray = const_cast<RPointerArray<CCalEntry>*>(&aEntryArray);
		// Set the time zone converter.  iTzConverter may be NULL if the optional
		// time zone conversion service is not available.  In this case all times will 
		// be exported as local time.
		if (iTzConnected)
			{
			iExportConverter->SetTzConverter(&iTzServer);
			}
			
		ExportEntryBySameTzL(aWriteStream);
		}
	}
	
void CAgnVCalConverter::ExportEntryBySameTzL(RWriteStream& aWriteStream)
	{//To export entries into a serveral vCal according to their tz rules
	if(!iSametzEntries)
		{
		iSametzEntries = new (ELeave) RPointerArray<CCalEntry>(4);
		}
		
	iSametzEntries->Reset();
	InitialiseSameTzEntryArrayL();
	//Find entries which belong to the first tz
	FindSameTzEntryL(*iSametzEntries);
	CParserVCal* vCal = NULL;
	//Expoet entries by grouping them into different tz rules 
	while(iSametzEntries->Count()>0)
		{//Pull entries with the same time zone in the same array and export them
		vCal = CParserVCal::NewL(); // takes ownership of entrylist
		CleanupStack::PushL(vCal);
		vCal->SetDefaultCharSet(iCharSet);
		ConvertEntryToVCalAndExportL(vCal, *iSametzEntries, aWriteStream);
		CleanupStack::PopAndDestroy(vCal);
		iSametzEntries->Reset();
		FindSameTzEntryL(*iSametzEntries);//this result in fill in iSametzEntries with same tz rule and remove the their indexes from iEntryIndexToExport
		}
	
	if(iEntryIndexToExport.Count()>0)
		{
		//Export the rest of entryies(those are entries without tz informations)
		ExportEntryWithoutTzRuleL(aWriteStream);
		}
	}
		
void CAgnVCalConverter::ExportEntryWithoutTzRuleL(RWriteStream& aWriteStream)
	{//This function only should be called when all entries in iEntryIndexToExport are either no-repeating or floating entry
	CParserVCal* vCal = CParserVCal::NewL(); // takes ownership of entrylist
	CleanupStack::PushL(vCal);
	vCal->SetDefaultCharSet(iCharSet);
	const TInt KCount = iEntryIndexToExport.Count();
	for (TInt ii = 0; ii < KCount; ++ii)
		{
		CCalEntry* entry = (*iEntryArray)[iEntryIndexToExport[ii]];
		if (entry)
			{
			TCalRRule rule;
			__ASSERT_DEBUG(!(entry->GetRRuleL(rule)) || entry->StartTimeL().TimeMode() == TCalTime::EFloating, User::Invariant());
			iExportConverter->ExportEntryL(entry, *vCal);
			}
		}
	vCal->ExternalizeL(aWriteStream);
	CleanupStack::PopAndDestroy(vCal);
	}

void CAgnVCalConverter::ConvertEntryToVCalAndExportL(CParserVCal* aVcal, const RPointerArray<CCalEntry>& aEntryArray, RWriteStream& aWriteStream)
	{
	iExportConverter->NextVCalendarL();
	TInt count = aEntryArray.Count();

	for (TInt ii = 0; ii < count; ii++)
		{
		CCalEntry* entry = aEntryArray[ii];
		if (entry)
			{
			iExportConverter->AddTimeZonePropertiesL(*aVcal, entry);
			iExportConverter->ExportEntryL(entry, *aVcal);
			}
		}
	
	aVcal->ExternalizeL(aWriteStream);
	}
	
void CAgnVCalConverter::ImportVCalAsyncL(TInt aFlags, RReadStream& aReadStream, 
		RPointerArray<CCalEntry>& aEntryArray, MCalDataExchangeCallBack& aObserver)
	{
	// Create a new converter object
	if(iAsyncState!=EIdle)
		{
		User::Leave(KErrInUse);
		}
		
	iImportConverter = new (ELeave) CVCalToAgendaEntryConverter();
	iVCalArray = new (ELeave) RPointerArray<CParserVCal>();

	// Read all the entries in the file. The parser will only read one.
	iTotalNumEntry = 0;
	TInt errcode = KErrNone;
	while (errcode != KErrEof)
   	 	{
 		// Create a parser to internalize the entries
		CParserVCal* vCal = CParserVCal::NewL();
		CleanupStack::PushL(vCal);
		// Japan uses SHIFT-JIS as the default charset contrary to 
		// the vCalendar Specification which says that the default 
		// charset should be US-ASCII. 
		// In order to provide interoperability with this market
		// segment, allow a flag to override the default charset in versit.
		if(aFlags & KCalDataExchangeDefaultShiftJIS)
		    {
		    vCal->SetDefaultCharSet(Versit::EShiftJISCharSet);
		    vCal->SetFlags(CVersitParser::EUseDefaultCharSetForAllProperties);
		    }
  	 	TRAP( errcode, vCal->InternalizeL(aReadStream) );
  	 	if (errcode ==KErrNone)
  	 		{
  	 		CArrayPtr<CVersitParser>* entities = vCal->ArrayOfEntities(EFalse); 
  	 		TInt numEntry = entities->Count();
	 		if(numEntry>0)
	 			{
	 			iVCalArray->AppendL(vCal);
	 			CleanupStack::Pop(vCal);
	 			iTotalNumEntry += numEntry;
	 			}
  	 		}
  	 	else if (errcode ==KErrEof)
  	 		{
  	 		CleanupStack::PopAndDestroy(vCal);
  	 		}
	  	else
	  	 	{
	  	 	User::LeaveIfError( errcode );
	  	 	}
     	}


	// Set the time zone converter.  iTzConverter may be NULL if the optional
	// time zone conversion service is not available.  In this case all imported times 
	// will be exported as local time.
	if (iTzConnected)
		{
		iImportConverter->SetTzConverter(&iTzServer);
		}

	iEntryArray = &aEntryArray;
	iTotalCounter = 0;
	iSameTzCounter = 0;
	iObserver = &aObserver;
	iAsyncState = EImporting;
	iMaxNumEntryInStep = iObserver->NumberOfEntriesToHandleAtOnce();
	if(iTotalNumEntry > 0)
		{
		CTzRules* tzRule = ImportTzRulesL(*((*iVCalArray)[0]));
		iImportConverter->SetTzRules(tzRule); // converter takes ownership of rules collection.
		}
	iImportExportActive->Start();
	}
	
void CAgnVCalConverter::FindSameTzEntryL(RPointerArray<CCalEntry>& aSameTzEntry)
	{
	//Fill in aSameTzEntry with those entries with the same tz rule in iEntryArray
	//Remove the index of the entry from iEntryIndexToExport
	//Note that the order of the entries being exported has to be maintained 
	CCalEntry* firstEntrywithTz = NULL;
	for (TInt ii = 0; ii < iEntryIndexToExport.Count();  ++ii)
		{
		TCalRRule repeatrule;//temp veraible
		CCalEntry* entry = (*iEntryArray)[iEntryIndexToExport[ii]];
		if(entry->GetRRuleL(repeatrule) && entry->StartTimeL().TimeMode() != TCalTime::EFloating)
			{//Otherwise time zone info is not needed to export
			if(firstEntrywithTz)
				{
				CTzRules* rules1 = entry->TzRulesL();
				CleanupStack::PushL(rules1);

				CTzRules* rules2 = firstEntrywithTz->TzRulesL();
				CleanupStack::PushL(rules2);
				
				if (rules1 && rules2 && rules1->IsEqualTo(*rules2))
					{
					//compare the time zone to see if it is the same as the "firstEntrywithTz"
					aSameTzEntry.AppendL(entry);
					iEntryIndexToExport.Remove(ii);
					--ii;
					}
				CleanupStack::PopAndDestroy(rules2);
				CleanupStack::PopAndDestroy(rules1);
				}
			else
				{
				firstEntrywithTz = entry;
				aSameTzEntry.AppendL(entry);
				iEntryIndexToExport.Remove(ii);
				--ii;
				}
			}
		}
	}
	
void CAgnVCalConverter::FindEntryWithoutTzRuleL(RPointerArray<CCalEntry>& aSameTzEntry)
	{
	// This functions should be only used for asynchronous exporting (use ExportEntryWithoutTzRuleL if it is for synchronous exporting)
	for (TInt ii = 0; ii< iEntryIndexToExport.Count();  ++ii)
		{//Pull those entries which do not need to export tz together (either non-repeating or floating entry)
		TCalRRule rule;
		CCalEntry* entry = (*iEntryArray)[iEntryIndexToExport[ii]];
		if(!entry->GetRRuleL(rule) || entry->StartTimeL().TimeMode() == TCalTime::EFloating)
			{
			aSameTzEntry.AppendL((*iEntryArray)[iEntryIndexToExport[ii]]);
			iEntryIndexToExport.Remove(ii);
			--ii;
			}
		}
	}

void CAgnVCalConverter::ExportVCalAsyncL(RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aEntryArray, MCalDataExchangeCallBack& aObserver, const Versit::TVersitCharSet aCharSet)
	{
	if(iAsyncState!=EIdle)
		{
		User::Leave(KErrInUse);
		}

	iEntryArray = &aEntryArray;
		
	// Create a new converter object
	if(!iExportConverter)
		{
		iExportConverter = new(ELeave)CAgendaEntryToVCalConverter;
		}

	// Set the time zone converter.  iTzConverter may be NULL if the optional
	// time zone conversion service is not available.  In this case all times will 
	// be exported as local time.
	if (iTzConnected)
		{
		iExportConverter->SetTzConverter(&iTzServer);
		}
	
	if(!iSametzEntries)
		{
		iSametzEntries = new(ELeave) RPointerArray<CCalEntry>;
		}
	iCharSet = aCharSet;
	iSametzEntries->Reset();
	InitialiseSameTzEntryArrayL();

	iVCal = CParserVCal::NewL();
	iVCal->SetDefaultCharSet(iCharSet);
	
	iTotalNumEntry = iEntryArray->Count();
	iTotalCounter = 0;
	iSameTzCounter = 0;
	iObserver = &aObserver;
	iAsyncState = EExporting;
	iWriteStream = &aWriteStream;
	iMaxNumEntryInStep = iObserver->NumberOfEntriesToHandleAtOnce();
	
	FindSameTzEntryL(*iSametzEntries);
	if(iSametzEntries->Count()==0)
		{
		FindEntryWithoutTzRuleL(*iSametzEntries);
		__ASSERT_DEBUG(iEntryIndexToExport.Count()==0, User::Invariant());
		}
	iExportConverter->NextVCalendarL();
	iImportExportActive->Start();
	}
	
void CAgnVCalConverter::InitialiseSameTzEntryArrayL()
	{// Initialise the the array with the indexes of each element in iEntryArray
	iEntryIndexToExport.Reset();
	const TInt KCount = iEntryArray->Count();
	for (TInt ii = 0; ii<KCount; ++ii)
		{
		iEntryIndexToExport.AppendL(ii);
		}
	}
	
void CCalImportExportActive::RunL()
	{
	switch (iConverter.AsyncState())
		{
		case CAgnVCalConverter::EImporting:
			iConverter.NextImportStepL();
			break;
		case CAgnVCalConverter::EExporting:
			iConverter.NextExportStepL();
			break;
		default:
			case CAgnVCalConverter::EIdle:
			Panic(EAgnVersitPanicImportExportFailure);
			break;
		}
	}
	
CAgnVCalConverter::TState CAgnVCalConverter::AsyncState() const 
	{
	return iAsyncState;
	}
	
void CAgnVCalConverter::CheckCompletion()
	{
	if (iTotalCounter == iTotalNumEntry)
		{
		iObserver->Completed();
		CancelImportExport();
		}
	else
		{
		iObserver->Progress((iTotalCounter * 100) / iTotalNumEntry);
		iImportExportActive->Start();
		}
	}
	
void CAgnVCalConverter::NextImportStepL()
	{
	if(iTotalNumEntry>0)
		{
		CArrayPtr<CVersitParser>* entities = (*iVCalArray)[0]->ArrayOfEntities(EFalse); //doesn't take ownership
		TInt numSameTzEntry = entities->Count();
		TInt count = ((numSameTzEntry - iSameTzCounter) > iMaxNumEntryInStep)?iMaxNumEntryInStep:(numSameTzEntry - iSameTzCounter); 
		RPointerArray<CCalEntry> importedEntries;
		CleanupResetAndDestroyPushL(importedEntries);

		for (TInt i = 0; i < count; i++)
			{
			CVersitParser& parser = *((*entities)[i + iSameTzCounter ]);
			iImportConverter->ImportVCalL(parser, importedEntries);
			for (TInt j = 0; j < importedEntries.Count(); j++)
				{
				iEntryArray->AppendL((importedEntries)[j]);
				}
			importedEntries.Reset();
			}
			
		CleanupStack::PopAndDestroy();//importedEntries
		iTotalCounter  += count;
		iSameTzCounter += count;

		if(iSameTzCounter == numSameTzEntry)
			{//Convert the next vCal into entries
			delete (*iVCalArray)[0];
			iVCalArray->Remove(0);
		
			if(iVCalArray->Count()>0)
				{
				CTzRules* tzRule = ImportTzRulesL(*((*iVCalArray)[0]));
				iImportConverter->SetTzRules(tzRule); // converter takes ownership of rules collection.
				iSameTzCounter = 0;
				}
			}
		}
		
	CheckCompletion();
	}
	
void CAgnVCalConverter::NextExportStepL()
	{
	if(iTotalNumEntry>0)
		{
		TInt numSameTzEntry = iSametzEntries->Count();
		//decide how many entries to export in this step
		TInt count = ((numSameTzEntry - iSameTzCounter) > iMaxNumEntryInStep)?iMaxNumEntryInStep:(numSameTzEntry - iSameTzCounter); 
		for (TInt i = 0; i < count; i++)
			{
			CCalEntry* entry = (*iSametzEntries)[i + iSameTzCounter];
			if (entry)
				{
				iExportConverter->AddTimeZonePropertiesL(*iVCal, entry);
				iExportConverter->ExportEntryL(entry, *iVCal);
				}
			}
			
		iTotalCounter += count;
		iSameTzCounter += count;

		if (iSameTzCounter == numSameTzEntry)
			{//Add time zone property 
			iExportConverter->NextVCalendarL();
			//calling AddTimeZone Properties for all entries
			//iExportConverter->AddTimeZonePropertiesL(*iVCal, (*iSametzEntries)[0]);
			iVCal->ExternalizeL(*iWriteStream);
			delete iVCal;
			iVCal = NULL;
			//Find out next group entries which are in the same time zone
			if(iEntryIndexToExport.Count()>0)
				{
				iVCal = CParserVCal::NewL();
				iVCal->SetDefaultCharSet(iCharSet);
				iSametzEntries->Reset();
				FindSameTzEntryL(*iSametzEntries);
				if(iSametzEntries->Count()==0)
					{
					FindEntryWithoutTzRuleL(*iSametzEntries);
					}
				iSameTzCounter = 0;
				}
			}
		}
	CheckCompletion();
	}

void CAgnVCalConverter::CancelImportExport(TInt aError)
/**
 * This function destroys import/export convertor objects and resets async state.
 * @param "TInt aError"
 *        Error during operation passed to the observer.
 */
	{
	// Propagate in case of error
	if(aError != KErrNone)
		{
		iObserver->Progress(aError);
		iObserver->Completed();
		}

	delete iImportConverter;
	iImportConverter = NULL;
	delete iExportConverter;
	iExportConverter = NULL;
	delete iVCal;
	iVCal = NULL;
	if(iVCalArray)
		{
		iVCalArray->ResetAndDestroy();
		}
	delete iVCalArray;
	iVCalArray = NULL;
	iAsyncState = EIdle;
	}
/**
 * Provides indication of validity of Daylight property during import
 *
 * At the moment, this means the following are all true:
 *    1) start year is not earlier than 1900
 *    2) end year is not later than 2100
 *    3) end year is not earlier than start year
 *
 * @param aDaylightProperty The property to be validated.
 *
 * @return ETrue if aDaylightProperty satisfies above conditions. EFalse otherwise.
 */
TBool CAgnVCalConverter::ValidDaylightProperty(CParserProperty& aDaylightProperty) const
    {		
    CParserPropertyValue* propertyValue = aDaylightProperty.Value();
 
    if (propertyValue)
       {
       CVersitDaylight* checkProperty = static_cast<CParserPropertyValueDaylight*>(propertyValue)->Value();

       if (checkProperty && checkProperty->iSavings) 
          {
          TInt startYear = checkProperty->iStartTime->iDateTime.Year();
          TInt endYear = checkProperty->iEndTime->iDateTime.Year();
          TTime minDateTime = TCalTime::MinTime();
          TTime maxDateTime = TCalTime::MaxTime();
          
          return ( (startYear	>= minDateTime.DateTime().Year()) &&
                   (endYear 	<= maxDateTime.DateTime().Year()) &&
                   (endYear 	>= startYear) );
          }
       }
 
    return EFalse;
    }

/**
 * Creates a collection of rules that describes a set UTC offsets and when they 
 * should be applied to local times within entities observed by the "home" 
 * system that created the vCalendar object.
 *
 * @param aCalParser A handle to the vCalendar parser.
 *
 * @return The time zone rules collection, NULL if no rules can be created.
 */
CTzRules* CAgnVCalConverter::ImportTzRulesL(CParserVCal& aCalParser) const
	{
    TBool acceptable = EFalse;
	TTimeIntervalMinutes stdOffset = StandardTimeOffsetL(aCalParser);

	// access properties but don't take ownership
	CArrayPtr<CParserProperty>* daylightProperties = 
    aCalParser.PropertyL(KVersitTokenDAYLIGHT, TUid::Uid(KVersitPropertyDaylightUid), EFalse);

    if (daylightProperties)
       {
       // check for an acceptable daylight property 
       // if there are none, treat this vCal as if it has no daylight properties at all
       const TInt KCount = daylightProperties->Count();
       for (TInt i = 0; i < KCount && ! acceptable; i++) 
           {	
           if (ValidDaylightProperty(*daylightProperties->At(i)))	
       		  {
              acceptable = ETrue;
              }
           }

       delete daylightProperties; 
       daylightProperties = NULL;
       }
        
    // Need either a TZ property or at least one acceptable DAYLIGHT property         
    // before TZ rules can be generated

	if ((stdOffset.Int() == KMaxTInt) && acceptable) 
	   {
       // no TZ property supplied, but a DAYLIGHT property is useable   
       // set initial TZ offset to 0
       stdOffset = 0;
       }

	CTzRules* rules = NULL;
	// if TZ offset is set, we have something to process
	if (stdOffset.Int() != KMaxTInt)
		{
		rules = DaylightRulesL(aCalParser, stdOffset);
		rules->SetInitialStdTimeOffset(stdOffset.Int());
		}
	return rules;
	}

static TInt CompareDaylightProperties(const CParserProperty& aLeftProperty, const CParserProperty& aRightProperty)
	{
	CParserPropertyValueDaylight* leftValue = static_cast<CParserPropertyValueDaylight*>(aLeftProperty.Value());
	CParserPropertyValueDaylight* rightValue = static_cast<CParserPropertyValueDaylight*>(aRightProperty.Value());

	if (leftValue->Value()->iStartTimeSortKey >= rightValue->Value()->iStartTimeSortKey)
		return 1;
	return -1;
	}

/**
 * Creates a collection of rules that describes a set of UTC offsets and when they 
 * should be applied to local times within entities observed by the "home" 
 * system that created the vCalendar object.
 *
 * @param aCalParser A handle to the vCalendar parser.
 * @param aStdOffset The standard time UTC offset.  Specify TTimeIntervalMinutes(KMaxTInt) if not known.
 * @return The time zone rules collection, NULL if no rules can be created.
 */
CTzRules* CAgnVCalConverter::DaylightRulesL(CParserVCal& aCalParser, TTimeIntervalMinutes aStdOffset) const
	{
	__ASSERT_DEBUG(aStdOffset.Int() != KMaxTInt, User::Invariant());
	
	CTzRules* rules = CTzRules::NewL(0, KMaxTUint);
	CleanupStack::PushL(rules);

	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* daylightProperties = aCalParser.PropertyL(KVersitTokenDAYLIGHT, TUid::Uid(KVersitPropertyDaylightUid), EFalse);
	
	TTime nextRuleStart = Time::MinTTime();	// start of the next std rule
	TTzTimeReference nextRuleTimeRef = ETzUtcTimeReference;

	TInt  prevRuleOffset = aStdOffset.Int();

	if (daylightProperties != NULL)
		{
		CleanupStack::PushL(daylightProperties);
		// re-order the daylight properties
		TLinearOrder<CParserProperty> daylightPropertySort(CompareDaylightProperties);
		RPointerArray<CParserProperty> newDaylightPropArray;
		TInt i;
		for (i = 0; i < daylightProperties->Count(); i++)
			{
            // insert only valid imported properties 
            if (ValidDaylightProperty(*daylightProperties->At(i)))
                {
  			    TInt err = newDaylightPropArray.InsertInOrder(daylightProperties->At(i), daylightPropertySort);
  			    if (err != KErrAlreadyExists)
  			    	{
  			    	User::LeaveIfError(err);
  			    	}
                }
			}
		CleanupStack::PopAndDestroy(daylightProperties);

		CVersitDaylight* daylightProperty = NULL;
		TInt n = newDaylightPropArray.Count();
		
		// For each DAYLIGHT property, create a standard time rule prior to the
		// daylight saving time period, and then create the daylight savings rule
		// following on from the standard time period.
		for (i = 0; i < n; ++i)	
			{
			daylightProperty = static_cast<CParserPropertyValueDaylight*>(newDaylightPropArray[i]->Value())->Value();
			
			if (daylightProperty && daylightProperty->iSavings)
				{
				TTime dstStartTime(daylightProperty->iStartTime->iDateTime);
				TTime dstEndTime(daylightProperty->iEndTime->iDateTime);
				TInt  dstOffset = daylightProperty->iOffset.Int() / 60; // Convert seconds to minutes.
				
				// standard time rule (STD)
				TTzRule stdRule = TzRuleL(prevRuleOffset, 	// old offset, minutes
							    		  aStdOffset, 		// new offset (=STD offset), minutes
										  nextRuleStart,	// from the end of prev rule
								dstStartTime-TTimeIntervalSeconds(1));	// to DST start time less 1sec
																		// (e.g. 01:59:59)
				// daylight saving time rule (DST)
				TTzRule dstRule = TzRuleL(aStdOffset,		// old offset (=STD offset), minutes
							    		  dstOffset,		// new offset (=DST offset), minutes
							    		  dstStartTime, 	// start
										  dstEndTime); 		// end time
				
				prevRuleOffset = dstOffset;				

				// 01:59:00->02:00:00; 01:59:59->02:00:00; 02:00:00->02:00:00; 01:29:59->01:30:00
				nextRuleStart = dstEndTime;
				nextRuleStart.RoundUpToNextMinute();
				if (nextRuleStart.DateTime().Minute()%10==9) nextRuleStart+=TTimeIntervalMinutes(1);

				// time references
				stdRule.iFrom.iTimeReference = nextRuleTimeRef;
				stdRule.iTimeReference = nextRuleTimeRef;
				if (daylightProperty->iStartTime->iRelativeTime!=TVersitDateTime::EIsUTC)
					{
					stdRule.iTo.iTimeReference = ETzWallTimeReference;
					dstRule.iFrom.iTimeReference = ETzWallTimeReference;
					dstRule.iTimeReference = ETzWallTimeReference;
					}
				if (daylightProperty->iEndTime->iRelativeTime!=TVersitDateTime::EIsUTC)
					{
					nextRuleTimeRef = ETzWallTimeReference;
					dstRule.iTo.iTimeReference = ETzWallTimeReference;
					}
				else
					{
					nextRuleTimeRef = ETzUtcTimeReference;
					}
			
				rules->AddRuleL(stdRule);	// add STD rule
				rules->AddRuleL(dstRule);	// add DST rule
				}
			}
		newDaylightPropArray.Reset();
		}
	
	// Create a standard time rule after the last daylight saving period.
	TTzRule lastStdRule = TzRuleL(prevRuleOffset, aStdOffset, nextRuleStart, Time::MaxTTime());
	lastStdRule.iFrom.iTimeReference = nextRuleTimeRef;
	lastStdRule.iTimeReference = nextRuleTimeRef;	
	rules->AddRuleL(lastStdRule);
	CleanupStack::Pop(rules);

	return rules;
	}
	

/**
 * Creates a rule that stores the UTC offset and when it should be
 * applied to local times within entities specified by the "home" 
 * system that created the vCalendar object.
 *
 * @param aOldTzOffset The UTC offset (minutes since midnight) before the rule applies.
 * @param aNewTzOffset The UTC offset (minutes since midnight) when the rule applies.
 * @param aStartTime   The time from when the rule applies.
 * @param aEndTime     The time to when the rule applies.
 */
TTzRule CAgnVCalConverter::TzRuleL(TTimeIntervalMinutes aOldTzOffset, TTimeIntervalMinutes aNewTzOffset, TTime aStartTime, TTime aEndTime) const
	{
	TTzRule rule;

	// There needs to be better boundary checking when converting from TTime to TUInt...
	// Ideally, the rule should accept TTimes.
	
	if (aStartTime != Time::MinTTime())
		{
		rule.iFrom.iTime = aStartTime;
		// rule.iFrom.iTimeReference defaults to ETzUtcTimeReference	
		rule.iMonth = aStartTime.DateTime().Month();
		rule.iDayOfMonth = aStartTime.DateTime().Day();
		rule.iTimeOfChange = aStartTime.DateTime().Hour() * 60 + aStartTime.DateTime().Minute();
		}
	
	if (aEndTime == Time::MaxTTime())
		{
		rule.iTo = TTimeWithReference::Max();
		// rule.iTo.iTimeReference defaults to ETzUtcTimeReference
		}
	else
		{
		rule.iTo = aEndTime;
		// rule.iTo.iTimeReference defaults to ETzUtcTimeReference
		}

	rule.iOldLocalTimeOffset = aOldTzOffset.Int();
	rule.iNewLocalTimeOffset = aNewTzOffset.Int();
	rule.iDayRule = ETzFixedDate;
	rule.iTimeReference = ETzUtcTimeReference;

	return rule;
	}


/**
 * The TZ property represents the standard time zone of the 
 * "home" system that created the vCalendar object.  If there are
 * more than one TZ properties, then only the first TZ property value
 * is returned.
 * 
 * @param  aCalParser A handle to the vCalendar parser.
 * @return TTimeIntervalMinutes(KMaxTInt) if there is no TZ property, TZ property value otherwise.
 */
TTimeIntervalMinutes CAgnVCalConverter::StandardTimeOffsetL(CParserVCal& aCalParser) const
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* tzCollection = aCalParser.PropertyL(KVersitTokenTZ, TUid::Uid(KVersitPropertyTimeZoneUid), EFalse);

	if (tzCollection == NULL)
		return TTimeIntervalMinutes(KMaxTInt);
	
	CleanupStack::PushL(tzCollection);
	TTimeIntervalSeconds stdTime = static_cast<CParserPropertyValueTimeZone*>((*tzCollection)[0]->Value())->Value();
	CleanupStack::PopAndDestroy(tzCollection);

	return TTimeIntervalMinutes(stdTime.Int() / 60);
	}
	
/**  
 * This function destroys the TAgnVCalConverter object created by the gate function,
 * and the CTzConverter object if it exists
 */
CAgnVCalConverter::~CAgnVCalConverter()
	{	
	delete iImportConverter;
	
	if (iVCalArray)
		{
		iVCalArray->ResetAndDestroy();
		}
	delete iVCalArray;
	
	delete iImportExportActive;
	delete iExportConverter;

	iTzServer.Close();
	if(iSametzEntries)
		{
		iSametzEntries->Close();
		delete iSametzEntries;
		}
		
	iEntryIndexToExport.Close();
	}

/**
 * Standard 2-phase construction
 */
CAgnVCalConverter* CAgnVCalConverter::NewL()
   	{
   	CAgnVCalConverter* self = new (ELeave) CAgnVCalConverter;
   	CleanupStack::PushL(self);
   	self->ConstructL();
   	CleanupStack::Pop(self);
   	return self;
   	}
  


// Export the implementation collection function
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102035F6, CAgnVCalConverter::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
  


void CAgnVCalConverter::ConstructL()
	{
	iTzConnected = EFalse;
	// Timezone conversion library is not guaranteed to be available.
	// If it is not available, then AgnVersit will ignore DST rules
	// and default to its previous behaviour
	const TInt result = iTzServer.Connect();
	if (result == KErrNone)
		{
		iTzConnected = ETrue;
		}
	else if (result != KErrNotFound)
		{
		User::Leave(result);
		}
	iImportExportActive = new (ELeave) CCalImportExportActive(*this);
	iAsyncState = EIdle;
	}
	
/**
 * Panic the thread with AGNVERSIT as the category
 * 
 * @param aPanic Panic number
 */
void Panic(TAgnVersitPanic aPanic)
	{
	_LIT(KAgnVersitPanicCategory,"AGNVERSIT");
	User::Panic(KAgnVersitPanicCategory,aPanic);
	}

/**
 * Standard Epoc32 Dll Entry point
 */


// Active object to control asynchronous import / export
CCalImportExportActive::CCalImportExportActive(CAgnVCalConverter& aConverter) :
	CActive(EPriorityLow), iConverter(aConverter)
	{
	CActiveScheduler::Add(this);
	}

CCalImportExportActive::~CCalImportExportActive()
	{
	Cancel();
	}
	
void CCalImportExportActive::Start()
	{
	if (!IsActive())
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}


void CCalImportExportActive::DoCancel()
	{
	iConverter.CancelImportExport();
	}

TInt CCalImportExportActive::RunError(TInt aError)
	{
	// Propogate error		
	iConverter.CancelImportExport(aError);
	return KErrNone;
	}
