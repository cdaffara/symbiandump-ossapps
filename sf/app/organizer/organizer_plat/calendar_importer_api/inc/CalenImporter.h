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
* Description:   The class converts from a stream to one or more agenda
*                entries.
*
*/


#ifndef CALENIMPORTER_H
#define CALENIMPORTER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <AgnImportObserver.h>



// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;
class CCalDataExchange;
class CAgnExternalInterface;




// CLASS DECLARATION

enum TCalenImportMode
    {
    ECalenImportModeNormal=0,
    ECalenImportModeExtended,        
    };




/**
* A class that imports a vCalendar into a Series60 Calendar compatible
* CCalEntry.
* 
*  @lib CalenImp.dll
*/
NONSHARABLE_CLASS (CCalenImporter) : 
    public CBase, 
    public MAgnImportObserver
    {
    public:  // Constructors and destructor

        /**
         * Two phased constructor.
         * @param A file server session
         */
        IMPORT_C static CCalenImporter* NewL( CCalSession& aSession );

        /**
         * Destructor.
         */
        ~CCalenImporter();

    public: // New functions

        /*
         * Imports a vCalendar.
         *
         * A: If the importing succeeds, CCalEntry array is filled with 
         *    appropriate entries. 
         *
         * B: Method will leave in any error situation (no memory, corrupted 
         *    data etc.). It is possible that importing succeeds partly, in 
         *    which case CCalEntry array can contain entries that were 
         *    imported before leave occured. 
         * @param aReadStream A read stream which is already open.
         * @param aArray An array where imported entries are stored. 
         * @return None. 
         */
        IMPORT_C void 
            ImportVCalendarL( RReadStream& aReadStream, RPointerArray<CCalEntry>& aArray );

        IMPORT_C void 
            ImportICalendarL( RReadStream& aReadStream, RPointerArray<CCalEntry>& aArray );
            
        IMPORT_C void SetImportMode(TCalenImportMode aImportMode);
            
	
	private: // From MAgnImportObserver
	
	    TImpResponse AgnImportErrorL( TImpError aType, 
	        const TDesC8& aUid, const TDesC& aContext);
	    
    private:

        /**
        * C++ default constructor.
        */
        CCalenImporter( CCalSession& aSession );
        
        void ConstructL();
        
        //writes the imported vCal/iCal to file
        #ifdef TRACE_TO_FILE_IMPORT
            void DebugPrintFunction( RReadStream& aReadStream );
            void WriteReadStreamToFileL( RReadStream& aReadStream );
            void ResetReadStreamL( RReadStream& aReadStream );  
        #endif
    private:    // Data
    
        CCalSession&            iSession;
                
        /// iCalendar specific                 
	    CAgnExternalInterface*  iICalInterface;        
                
        /// vCalendar specific                
        CCalDataExchange*       iVCalDataExchange;        
        
        TCalenImportMode        iImportMode;
        
        //Used for incrementing the file number for imported iCals/vCals
        #ifdef TRACE_TO_FILE_IMPORT
            TUint iFileNumber;
        #endif

    };

#endif      // CALENIMPORTER_H

// End of File
