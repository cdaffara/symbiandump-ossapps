/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This file contains instantiation of iCal importer and
*                exporter.Importer and exporter does conversion between 
*                iCal entry and symbian agnda entry.
*
*/



// Class include.
#include "AgnVersit2.h"					//CAgnVersit2

// User includes.
#include "AgnVersit2Importer.h"			//CAgnVersit2Importer
#include "AgnVersit2Exporter.h"			//CAgnVersit2Exporter
#include "AgnVersit2StringProvider.h"	//CAgnVersit2StringProvider

#include "calendarengines_debug.h"

/**
Two stage constructor for AgnVersit2 objects. This is used during plugin
instantiation and should not be called directly from client applications.
@return A new CAgnVersit2 object.
@publishedAll
*/
CAgnVersit2* CAgnVersit2::NewL()
	{
	TRACE_ENTRY_POINT;
	
	CAgnVersit2* self = new (ELeave) CAgnVersit2;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	
	return self;
	}
	
/**
Destructor.
@internalTechnology
*/
CAgnVersit2::~CAgnVersit2()
	{
	TRACE_ENTRY_POINT;
	
	delete iImporter;
	delete iExporter;
	delete iStringProvider;
	
	TRACE_EXIT_POINT;
	}
	
/**
Takes an array of agenda entries and a stream of iCalendar formatted text, and
adds entries to the array representing the components in the stream. This
implements CAgnExternalInterface::ImportL.
@see CAgnExternalInterface::ImportL()
@param aEntries An array onto which new entries will be appended.
@param aReadStream A stream from which to read the iCalendar information. The
client is responsible for opening the stream before calling this function, and
is also responsible for closing it again.
@param aFlags not used in this implementation.
@param aObserver A mechanism for error reporting and handling.
@publishedAll
*/
void CAgnVersit2::ImportL( RPointerArray<CCalEntry>& aEntries,
                           RReadStream& aReadStream,
                           TUint /*aFlags*/,
                           MAgnImportObserver& aObserver )
	{
	TRACE_ENTRY_POINT;
	
	if (!iImporter)
		{
		iImporter =  CAgnVersit2Importer::NewL(*iStringProvider);
		}

	iImporter->ImportL(aEntries, aReadStream, aObserver);
	
	TRACE_EXIT_POINT;
	}
	
/**
This implements CAgnExternalInterface::ExportL.
@see CAgnExternalInterface::ExportL()
@param aEntry Calendar entry to be exported.
@param aWriteStream Stream to write output to. The client is responsible for
opening the stream before calling this function, and is also responsible for
closing it again.
@param aFlags Not used in this implementation.
@param aObserver Export observer to handle errors.
@publishedAll
*/
void CAgnVersit2::ExportL( const CCalEntry& aEntry,
                           RWriteStream& aWriteStream,
                           TUint aFlags,
                           MAgnExportObserver& aObserver )
	{
	TRACE_ENTRY_POINT;
	
	if (!iExporter)
		{
		iExporter = CAgnVersit2Exporter::NewL(*iStringProvider);
		}
	
	iExporter->ExportL(aEntry, aWriteStream, aFlags, aObserver);
	
	TRACE_EXIT_POINT;
	}

/**
This implements CAgnExternalInterface::ExportL.
@see CAgnExternalInterface::ExportL()
@param aEntries array of Calendar entries to be exported.
@param aWriteStream Stream to write output to. The client is responsible for
opening the stream before calling this function, and is also responsible for
closing it again.
@param aFlags Not used in this implementation.
@param aObserver Export observer to handle errors.
@publishedAll
*/
void CAgnVersit2::ExportL( RPointerArray<CCalEntry>& aEntries,
                           RWriteStream& aWriteStream,
                           TUint aFlags,
                           MAgnExportObserver& aObserver )
	{
	TRACE_ENTRY_POINT;
	
	if (!iExporter)
		{
		iExporter = CAgnVersit2Exporter::NewL(*iStringProvider);
		}
	
	iExporter->ExportL(aEntries, aWriteStream, aFlags, aObserver);	
	
	TRACE_EXIT_POINT;
	}
	
/**
Default constructor.
@internalTechnology
*/
CAgnVersit2::CAgnVersit2()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
	
/**
Second phase construction.
@internalTechnology
*/
void CAgnVersit2::ConstructL()
	{
	TRACE_ENTRY_POINT;
	
	iStringProvider = CAgnVersit2StringProvider::NewL();
	// Initialise the importer and exporter straight away in debug mode
	// This prevents problems when memory testing
	__DEBUG_ONLY(iImporter =  CAgnVersit2Importer::NewL(*iStringProvider));
	__DEBUG_ONLY(iExporter =  CAgnVersit2Exporter::NewL(*iStringProvider));
	
	TRACE_EXIT_POINT;
	}

// End of file.
