// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <caldataexchange.h>
#include "caldataexchangeimpl.h"

/** Destructor
@publishedAll
@released
@capability None
*/
EXPORT_C CCalDataExchange::~CCalDataExchange()
	{
	delete iImpl;
	}

CCalDataExchange::CCalDataExchange()
	{
	}
	
void CCalDataExchange::ConstructL(CCalSession& aSession)
	{
	iImpl = new (ELeave) CCalDataExchangeImpl(aSession);
	}
	
/** Allocates and constructs a CCalDataExchange object.
@publishedAll
@released
@capability None
@param aSession The handle to the calender session.
@return A pointer to the new synchronisation object.
*/		
EXPORT_C CCalDataExchange* CCalDataExchange::NewL(CCalSession& aSession)	
	{
	CCalDataExchange* self = new (ELeave) CCalDataExchange();
	CleanupStack::PushL(self);
	self->ConstructL(aSession);
	CleanupStack::Pop(self);
	return self;
	}


/** Imports data of aDataFormat type and returns an array of CCalEntries corresponding to this data

Currently only VCal 1.0 (with extensions) is supported.
The vCalendar is read from the read stream specified and its entities are converted 
into CCalEntries and passed back in the array parameter aCalEntryArray.
@publishedAll
@released
@capability WriteUserData
@param aDataFormat Specifies the format of the data to be imported.  Currently this must be KUidVCalendar (see CalDataFormat.h)
@param aReadStream The read stream which contains the (vCalendar) data.
@param aCalEntryArray On return, a list of the calender entries which were imported. 
The list contains the vEvents followed by the vTodos
and are specific to the data format
*/
EXPORT_C void CCalDataExchange::ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	iImpl->ImportL(aDataFormat, aReadStream, aCalEntryArray);
	}  
	
/** Imports data of aDataFormat type and returns an array of CCalEntries corresponding to this data

Currently only VCal 1.0 (with extensions) is supported.
The vCalendar is read from the read stream specified and its entities are converted 
into CCalEntries and passed back in the array parameter aCalEntryArray.
@publishedAll
@released
@capability WriteUserData
@param aDataFormat Specifies the format of the data to be imported.  Currently this must be KUidVCalendar (see CalDataFormat.h)
@param aReadStream The read stream which contains the (vCalendar) data.
@param aCalEntryArray On return, a list of the calender entries which were imported. 
The list contains the vEvents followed by the vTodos
and are specific to the data format
@param aFlags controls the behaviour of the parser during the import process.
*/
EXPORT_C void CCalDataExchange::ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, TInt aFlags)
	{
	iImpl->ImportL(aDataFormat, aReadStream, aCalEntryArray, aFlags);
	}   
	
/** Imports data of aDataFormat type and returns an array of a selected number of CCalEntries corresponding to this data.

Currently only VCal 1.0 (with extensions) is supported.
The vCalendar is read from the read stream specified and its entities are converted 
into CCalEntries and passed back in the array parameter aCalEntryArray.
@publishedAll
@released
@capability WriteUserData
@param aDataFormat Specifies the format of the data to be imported.  Currently this must be KUidVCalendar (see CalDataFormat.h)
@param aReadStream The read stream which contains the (vCalendar) data.
@param aCalEntryArray On return, a list of the calender entries which were imported. 
The list contains the vEvents followed by the vTodos
and are specific to the data format
@param aFlags controls the behaviour of the parser during the import process.
@param aNumEntries Specifies how many entries are to be imported at once.
*/
EXPORT_C void CCalDataExchange::ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, TInt aFlags, TInt aNumEntries)
	{
	iImpl->ImportL(aDataFormat, aReadStream, aCalEntryArray, aFlags, aNumEntries);
	}
	
/** Exports data of aDataFormat type from an array of CCalEntries corresponding to this data

Currently only VCal 1.0 (with extensions) is supported.
The entry is exported either as a vTodo (if it is a to-do), or as a vEvent. The exported 
property values are converted from Unicode into the character set indicated by the last 
parameter (UTF-8 by default).
@publishedAll
@released
@capability ReadUserData
@param aDataFormat Specifies the format of the data to be exported.  Currently this must be KUidVCalendar (see CalDataFormat.h)
@param aWriteStream The stream to which the calender entries should be externalised.
@param aCalEntryArray The array of calender entries to export. 
*/	
EXPORT_C void CCalDataExchange::ExportL(TUid aDataFormat, RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	iImpl->ExportL(aDataFormat, aWriteStream, aCalEntryArray);
	}
	
/** Asynchronously imports data of aDataFormat type and returns an array of CCalEntries corresponding to this data

Currently only VCal 1.0 (with extensions) is supported.
The vCalendar is read from the read stream specified and its entities are converted 
into CCalEntries and passed back in the array parameter aCalEntryArray.
@publishedAll
@released
@capability WriteUserData
@param aDataFormat Specifies the format of the data to be imported.  Currently this must be KUidVCalendar (see CalDataFormat.h)
@param aReadStream The read stream which contains the (vCalendar) data.
@param aCalEntryArray On return, a list of the calender entries which were imported. 
The list contains the vEvents followed by the vTodos
and are specific to the data format
@param aObserver Callback class to indicate the progress of the import and when it has completed.
@param aFlags controls the behaviour of the parser during the import process.
*/
EXPORT_C void CCalDataExchange::ImportAsyncL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, MCalDataExchangeCallBack& aObserver, TInt aFlags)
	{	
	iImpl->ImportAsyncL(aDataFormat, aReadStream, aCalEntryArray, aObserver, aFlags);
	}

/** Asynchronously exports data of aDataFormat type from an array of CCalEntries corresponding to this data

Currently only VCal 1.0 (with extensions) is supported.
The entry is exported either as a vTodo (if it is a to-do), or as a vEvent. The exported 
property values are converted from Unicode into the character set indicated by the last 
parameter (UTF-8 by default).
@publishedAll
@released
@capability ReadUserData
@param aDataFormat Specifies the format of the data to be exported.  Currently this must be KUidVCalendar (see CalDataFormat.h)
@param aWriteStream The stream to which the calender entries should be externalised.
@param aCalEntryArray The array of calender entries to export. 
@param aObserver Callback class to indicate the progress of the import and when it has completed.
*/	
EXPORT_C void CCalDataExchange::ExportAsyncL(TUid aDataFormat, RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray, MCalDataExchangeCallBack& aObserver)
	{
	iImpl->ExportAsyncL(aDataFormat, aWriteStream, aCalEntryArray, aObserver);
	}

