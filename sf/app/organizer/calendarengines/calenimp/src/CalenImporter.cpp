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
* Description:   The class converts from a stream to one or more agenda
*                entries.
*
*/


//debug
#include "calendarengines_debug.h"

// INCLUDE FILES
#include "CalenImporter.h"


#include "AgnExternalInterface.h"
#include <AgnImportObserver.h>

#include <caldataexchange.h>
#include <calentry.h>
#include <caldataformat.h>
#include <featmgr.h>

#include <coemain.h>
//#include <AgnVersit.h>  // not used because of symbian's Sphinx release



// LOCAL CONSTANTS AND MACROS
_LIT8( KICalMimeType, "text/calendar" );


// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
EXPORT_C CCalenImporter* CCalenImporter::NewL( CCalSession& aSession )
    {
    TRACE_ENTRY_POINT;
    
    CCalenImporter* self = new (ELeave) CCalenImporter( aSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    
    TRACE_EXIT_POINT;
    return self;
    }

// Destructor.
EXPORT_C CCalenImporter::~CCalenImporter()
    {
    TRACE_ENTRY_POINT;
    
    delete iICalInterface;
    delete iVCalDataExchange;
    FeatureManager::UnInitializeLib();
    
    TRACE_EXIT_POINT;
    }

// C++ default constructor.
CCalenImporter::CCalenImporter( CCalSession& aSession ) : 
    iSession( aSession )
    {
    TRACE_ENTRY_POINT;
    
    //Debug counter, used for incrementing the file number
    #ifdef TRACE_TO_FILE_IMPORT
        iFileNumber = 0;
    #endif
    
    iImportMode=ECalenImportModeNormal;
    
    TRACE_EXIT_POINT;
    }

// By default Symbian OS constructor is private.
void CCalenImporter::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    FeatureManager::InitializeLibL(); 
    
    TRACE_EXIT_POINT;
    }

// Possibility to set the import mode for importer to use e.g. japanese charset.
EXPORT_C void CCalenImporter::SetImportMode(TCalenImportMode aImportMode)
    {
    TRACE_ENTRY_POINT;
    
    iImportMode=aImportMode;
    
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CCalenImporter::ImportVCalendarL
// Import vCalendar entries from aReadStream to aArray. 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenImporter::ImportVCalendarL( 
    RReadStream& aReadStream, 
    RPointerArray<CCalEntry>& aArray )
    {
    TRACE_ENTRY_POINT;
        
    //Debug - Write the vCal to file
    #ifdef TRACE_TO_FILE_IMPORT
        DebugPrintFunction( aReadStream );
    #endif
    
    if( ! iVCalDataExchange )
        {
        iVCalDataExchange = CCalDataExchange::NewL( iSession );
        }

// check if extended flag is on and japanese variant is on.
    if ( (iImportMode==ECalenImportModeExtended) && (FeatureManager::FeatureSupported(KFeatureIdJapanese) ))
        {
        // if Japanese flag is set we will pass Japanese charset flag to ImportL
        iVCalDataExchange->ImportL( KUidVCalendar, aReadStream, aArray,KCalDataExchangeDefaultShiftJIS);
            
        }
// we use normal method (ECalenImportModeNormal)
    else
        {
    	iVCalDataExchange->ImportL( KUidVCalendar, aReadStream, aArray );
        }
        
	TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenImporter::ImportICalendarL
// Import iCalendar entries from aReadStream to aArray. 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenImporter::ImportICalendarL( 
    RReadStream& aReadStream, 
    RPointerArray<CCalEntry>& aArray )
    {
    TRACE_ENTRY_POINT;
    
    //Debug - Write the iCal to file
    #ifdef TRACE_TO_FILE_IMPORT
        DebugPrintFunction( aReadStream );
    #endif
    
    if( ! iICalInterface )
        {
        iICalInterface = CAgnExternalInterface::NewL( KICalMimeType );
        }        
	iICalInterface->ImportL( aArray, aReadStream, 0, *this ); 
	
	TRACE_EXIT_POINT;   
    }

MAgnImportObserver::TImpResponse CCalenImporter::AgnImportErrorL(
    TImpError /*aType*/,
    const TDesC8& /*aUid*/,
    const TDesC& /*aContext*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EImpResponseContinue;
    }


#ifdef TRACE_TO_FILE_IMPORT
    //Wrapper function to write the readstream to file
    void CCalenImporter::DebugPrintFunction( RReadStream& aReadStream ) 
        {
        TRAPD( error, WriteReadStreamToFileL( aReadStream ) );
        
        //Even if WriteReadStreamToFileL leaves we want to reset the file
        //because we don't know which function left
        TRAP( error, ResetReadStreamL( aReadStream ));
        }
        
    //Writes the stream to a file
    void CCalenImporter::WriteReadStreamToFileL( RReadStream& aReadStream ) 
        {
        TBuf<50> fileName;
        _LIT(KDefaultFileName, "C://calenImp_%d.txt");
        fileName.Format( KDefaultFileName, iFileNumber );

        RFs aFs;
        User::LeaveIfError(aFs.Connect());
        CleanupClosePushL(aFs);
        aFs.MkDirAll(KDefaultFileName);

        RFileWriteStream rs;
        User::LeaveIfError(rs.Replace(aFs, fileName, EFileWrite));
        CleanupClosePushL(rs);

        rs.WriteL( aReadStream );
        rs.CommitL();

        CleanupStack::PopAndDestroy(&rs);
        CleanupStack::PopAndDestroy(&aFs);
        
        //Increase the filenumber with one, a new file is created for each vCal/iCal
        ++iFileNumber;
        }

    //Resets the stream pointer to point to begining of stream
    void CCalenImporter::ResetReadStreamL( RReadStream& aReadStream ) 
        {
        //Set the read pointer to point to the beginning of the stream
        TStreamPos pos( 0 );
        MStreamBuf* streamBuf = aReadStream.Source();
        streamBuf->SeekL( MStreamBuf::ERead, pos );
        }
#endif

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// E32Dll(TDllReason)
// Entry point function for Symbian OS Apps
// Returns: KErrNone: No error
// ---------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

// End of File
