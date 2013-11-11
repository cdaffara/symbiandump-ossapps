/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CALENEXPORTER_H
#define CALENEXPORTER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>    // for TParse
#include <s32mem.h>
#include <AgnExportObserver.h>


// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;
class RWriteStream;
class CAgnExternalInterface;
class CCalDataExchange;
class CAgnExternalInterface;

// CLASS DECLARATION
/**
 *  Exports Agenda entry to VCalendar data
 *
 *  @lib Calendar.app
 *  @since 2.0
 */
NONSHARABLE_CLASS (CCalenExporter) : 
    public CBase, 
    public MAgnExportObserver
    {
    public:  // Constructors and destructor

    	/**
    	 *
    	 */
    	IMPORT_C static CCalenExporter* NewL( CCalSession& aSession );
    	
    	/**
    	 *
    	 */
    	IMPORT_C ~CCalenExporter();

    public: // New functions

        /*
         * Exports CCalEntry to RWriteStream. 
         *
         * @param aEntry Calendar entry to export.
         * @param aWriteStream Write stream where calendar entry is exported.     
         * @return None. 
         */    
        IMPORT_C void ExportVCalL( const CCalEntry& aEntry, RWriteStream& aWriteStream );
        IMPORT_C void ExportICalL( const CCalEntry& aEntry, RWriteStream& aWriteStream );


        /*
         * Exports CCalEntries to RWriteStream. Current implementation does not 
         * support exporting multiple CCalEntries into write stream. Method will 
         * leave with error code KErrNotSupported if array contains more than one 
         * entry.
         *
         * @param aArray Array of calendar entries to export.
         * @param aWriteStream Write stream where calendar entry is exported.     
         * @return None. 
         */    
        IMPORT_C void ExportVCalL( const RPointerArray<CCalEntry>& aArray, RWriteStream& aWriteStream );
        IMPORT_C void ExportICalL( const RPointerArray<CCalEntry>& aArray, RWriteStream& aWriteStream );


    protected: // private functions

        CCalenExporter( CCalSession& aSession );
        
        void ConstructL();

        TInt GetExportFlagsL( const CCalEntry& aEntry );
            
    protected: // from MAgnExportObserver

        TExpResponse AgnExportErrorL( TExpError aType, const TDesC8& aUid, 
            const TDesC& aContext);    
        
    private: // Debug
        
        //writes the exported vCal/iCal to file
        #ifdef TRACE_TO_FILE_EXPORT
            
            enum TDebugExportType
                {
                EVCal = 0,
                EICal
                };
        
            void DebugPrintFunction( const CCalEntry& aEntry, const TDebugExportType& aType );
            void WriteStreamToFileL( const CCalEntry& aEntry, const TDebugExportType& aType );
        
            //Used for incrementing the file number for exported iCals/vCals
            TUint iFileNumber;
            TBool iExported;
        #endif   
            
    private:    // Data
        CCalSession& iSession;
        
        // iCalendar specific
    	CAgnExternalInterface*  iICalInterface;    

        /// vCalendar specific 
        CCalDataExchange*       iVCalDataExchange;
        
        
    };

#endif      // CALENEXPORTER_H

// End of File
