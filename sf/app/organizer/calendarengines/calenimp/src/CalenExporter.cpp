/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Exports Agenda entry to vCalendar or iCalendar data
 *
*/


//debug
#include "calendarengines_debug.h"

// INCLUDE FILES
#include "CalenExporter.h"

#include "AgnExternalInterface.h"
#include <AgnExportObserver.h>

#include <caldataexchange.h>
#include <calsession.h>
#include <caldataformat.h>
#include <calentry.h>				// CCalEntry
#include <f32file.h>				// RFs
#include <s32file.h>				// RFileWriteStream, RFileReadStream


#include <e32base.h>
#include <f32file.h>    // for TParse
#include <s32mem.h>

#include <coemain.h>
#include <featmgr.h>

// LOCAL CONSTANTS AND MACROS

_LIT8( KICalMimeType, "text/calendar" );

enum TAgnExportMethod
	{
	EExportAll = 			0xFFFFFFFF,
	EVEventPublish =		KAgnExportDtStart		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportSequence		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVEventRequest = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportDtStart 		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportSummary 		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVEventReply = 			KAgnExportAttendee 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportUID 			|
							KAgnExportSequence		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportSummary		|
							KAgnExportXProp			|
							KAgnExportTzRules,

	EVEventAdd = 			KAgnExportDtStart 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportSummary 		|
							KAgnExportUID			|
							KAgnExportAttendee		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVEventCancel = 		KAgnExportAttendee 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportSummary		|
							KAgnExportXProp			|
							KAgnExportTzRules,

	EVEventRefresh = 		KAgnExportAttendee 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportXProp,

	EVEventCounter = 		KAgnExportDtStart 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportSummary 		|
							KAgnExportUID			|
							KAgnExportAttendee		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVEventDeclineCounter = KAgnExportOrganizer 	|
							KAgnExportDtStamp		|
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportSequence		|
							KAgnExportXProp,

	EVTodoPublish = 		KAgnExportDtStart		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportPriority		|
							KAgnExportSequence		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVTodoRequest = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportDtStart		|
							KAgnExportOrganizer		|
							KAgnExportPriority		|
							KAgnExportSequence		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVTodoReply = 			KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportRecurrenceId	|
							KAgnExportSequence		|
							KAgnExportStatus		|
							KAgnExportSummary		|
							KAgnExportXProp			|
							KAgnExportTzRules,

	EVTodoAdd = 			KAgnExportOrganizer		|
							KAgnExportDtStamp		|
							KAgnExportPriority		|
							KAgnExportSequence		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportAttendee		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVTodoCancel = 			KAgnExportAttendee		|
							KAgnExportUID			|
							KAgnExportOrganizer		|
							KAgnExportSequence		|
							KAgnExportDtStamp		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportPriority		|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules,

	EVTodoRefresh = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportXProp,

	EVTodoCounter = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportPriority		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportSequence		|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,

	EVTodoDeclineCounter = 	KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportSequence		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules
	};



// ============================ MEMBER FUNCTIONS ===============================

EXPORT_C CCalenExporter* CCalenExporter::NewL( CCalSession& aSession )
	{
	TRACE_ENTRY_POINT;
	
	CCalenExporter* self = new (ELeave) CCalenExporter( aSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    
    TRACE_EXIT_POINT;
    return self;
	}

void CCalenExporter::ConstructL()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

EXPORT_C CCalenExporter::~CCalenExporter()
	{
	TRACE_ENTRY_POINT;
	
	delete iICalInterface;
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenExporter::CCalenExporter
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CCalenExporter::CCalenExporter( CCalSession& aSession ) :
    iSession( aSession )
    {
    TRACE_ENTRY_POINT;
    
    //Debug counter, used for incrementing the file number
    #ifdef TRACE_TO_FILE_EXPORT
        iFileNumber = 0;
        iExported = EFalse;
    #endif
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenExporter::ExportVCalL
// Export one CCalEntry to write stream.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenExporter::ExportVCalL( const CCalEntry& aEntry, RWriteStream& aWriteStream )
    {
    TRACE_ENTRY_POINT;
    
	CCalDataExchange* exporter = CCalDataExchange::NewL( iSession );
	CleanupStack::PushL( exporter );

	RPointerArray< CCalEntry > array;
	CleanupClosePushL(array);
	array.Append( &aEntry );
	exporter->ExportL( KUidVCalendar, aWriteStream, array );
	CleanupStack::PopAndDestroy( 2 );
	
	//Debug - Write the iCal to file
    #ifdef TRACE_TO_FILE_EXPORT
        DebugPrintFunction( aEntry, EVCal );
    #endif
    
	TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenExporter::ExportICalL
// Export one CCalEntry to write stream.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenExporter::ExportICalL( const CCalEntry& aEntry, RWriteStream& aWriteStream  )
    {
    TRACE_ENTRY_POINT;
    
	TInt flags = GetExportFlagsL( aEntry );
	if( !iICalInterface )
	    {
        iICalInterface = CAgnExternalInterface::NewL( KICalMimeType );
	    }
	iICalInterface->ExportL( aEntry, aWriteStream, flags, *this);
	
	//Debug - Write the iCal to file
    #ifdef TRACE_TO_FILE_EXPORT
       DebugPrintFunction( aEntry, EICal );
    #endif
    
	TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenExporter::ExportVCalL
// Export multiple CCalEntries to write stream.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenExporter::ExportVCalL(
    const RPointerArray<CCalEntry>& aArray,
    RWriteStream& aWriteStream )
    {
    TRACE_ENTRY_POINT;
    
    if( aArray.Count() > 1 )
        {
        User::Leave( KErrNotSupported );
        }

    ExportVCalL( *aArray[0], aWriteStream );
    
    
    //Debug - Write the vCal to file
    #ifdef TRACE_TO_FILE_EXPORT
        DebugPrintFunction( *aArray[0], EVCal );
    #endif
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenExporter::ExportICalL
// Export multiple CCalEntries to write stream.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenExporter::ExportICalL(
    const RPointerArray<CCalEntry>& aArray,
    RWriteStream& aWriteStream )
    {
    TRACE_ENTRY_POINT;

    if( aArray.Count() > 1 )
        {
        User::Leave( KErrNotSupported );
        }

    ExportICalL( *aArray[0], aWriteStream );
    
    //Debug - Write the iCal to file
    #ifdef TRACE_TO_FILE_EXPORT
        DebugPrintFunction( *aArray[0], EICal );
    #endif
    
    TRACE_EXIT_POINT;
    }

TInt CCalenExporter::GetExportFlagsL(const CCalEntry& aEntry)
	{
    TRACE_ENTRY_POINT;
	
	switch (aEntry.EntryTypeL())
		{
		case CCalEntry::EEvent:
			{
			switch (aEntry.MethodL())
				{
				case CCalEntry::EMethodNone:
					TRACE_EXIT_POINT;
					return EExportAll;
				case CCalEntry::EMethodPublish:
					TRACE_EXIT_POINT;
					return EVEventPublish;
				case CCalEntry::EMethodRequest:
					TRACE_EXIT_POINT;
					return EVEventRequest;
				case CCalEntry::EMethodReply:
					TRACE_EXIT_POINT;
					return EVEventReply;
				case CCalEntry::EMethodAdd:
					TRACE_EXIT_POINT;
					return EVEventAdd;
				case CCalEntry::EMethodCancel:
					TRACE_EXIT_POINT;
					return EVEventCancel;
				case CCalEntry::EMethodRefresh:
					TRACE_EXIT_POINT;
					return EVEventRefresh;
				case CCalEntry::EMethodCounter:
					TRACE_EXIT_POINT;
					return EVEventCounter;
				case CCalEntry::EMethodDeclineCounter:
				    TRACE_EXIT_POINT;
					return EVEventDeclineCounter;
				}
			}
		break;
		case CCalEntry::ETodo:
			{
			switch (aEntry.MethodL())
				{
				case CCalEntry::EMethodNone:
					TRACE_EXIT_POINT;
					return EExportAll;
				case CCalEntry::EMethodPublish:
					TRACE_EXIT_POINT;
					return EVTodoPublish;
				case CCalEntry::EMethodRequest:
					TRACE_EXIT_POINT;
					return EVTodoRequest;
				case CCalEntry::EMethodReply:
					TRACE_EXIT_POINT;
					return EVTodoReply;
				case CCalEntry::EMethodAdd:
					TRACE_EXIT_POINT;
					return EVTodoAdd;
				case CCalEntry::EMethodCancel:
				    TRACE_EXIT_POINT;
					return EVTodoCancel;
				case CCalEntry::EMethodRefresh:
					TRACE_EXIT_POINT;
					return EVTodoRefresh;
				case CCalEntry::EMethodCounter:
					TRACE_EXIT_POINT;
					return EVTodoCounter;
				case CCalEntry::EMethodDeclineCounter:
				    TRACE_EXIT_POINT;
					return EVTodoDeclineCounter;
				}
			}
		}
	
	TRACE_EXIT_POINT;
	return EExportAll;
	}

MAgnExportObserver::TExpResponse CCalenExporter::AgnExportErrorL(
    TExpError /*aType*/,
    const TDesC8& /*aUid*/,
    const TDesC& /*aContext*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EExpResponseContinue;
    }


#ifdef TRACE_TO_FILE_EXPORT
    //Wrapper function to write the CCalEntry to file
    void CCalenExporter::DebugPrintFunction( const CCalEntry& aEntry, const TDebugExportType& aType ) 
        {
        //Very ugly code, if you know how to use the buffer of RWriteStream update this function!!        
        //This function will be called from ExportV/ICal, therefore a boolean to check if it
        //has been imported already so debug function wont be called again and cause a loop.
        if( !iExported )
            {
            TRAPD( error, WriteStreamToFileL( aEntry, aType  ) );
            iExported = ETrue;
            }
        else
            {
            iExported = EFalse;
            }
        }
        
    //Writes the CCalEntry to file
    void CCalenExporter::WriteStreamToFileL( const CCalEntry& aEntry, const TDebugExportType& aType ) 
        {
        TBuf<50> fileName;
        _LIT(KDefaultFileName, "C://calenExp_%d.txt");
        fileName.Format( KDefaultFileName, iFileNumber );

        RFs aFs;
        User::LeaveIfError(aFs.Connect());
        CleanupClosePushL(aFs);
        aFs.MkDirAll(KDefaultFileName);

        RFileWriteStream rs;
        User::LeaveIfError(rs.Replace(aFs, fileName, EFileWrite));
        CleanupClosePushL(rs);
        
        //check if vCal otherwise export as iCal     
        if( aType == EVCal )
            {
            ExportVCalL( aEntry, rs );
            }
        else
            {
            ExportICalL( aEntry, rs );
            }
        
        rs.CommitL();

        CleanupStack::PopAndDestroy(&rs);
        CleanupStack::PopAndDestroy(&aFs);
        
        ++iFileNumber;
        }
#endif
//  End of File
