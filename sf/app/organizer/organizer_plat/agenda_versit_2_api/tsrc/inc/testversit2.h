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
* Description:   test agnversit2 api
*
*/



#ifndef __TESTVERSIT2_H__
#define __TESTVERSIT2_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES
#include <s32file.h>				// RFileReadStream

//  FORWARD DECLARATIONS
class CICalParser;
class CCalEntry;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 * EUnitWizard generated test class. 
 */
NONSHARABLE_CLASS( CCVersit2Test )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CCVersit2Test* NewL();
        static CCVersit2Test* NewLC();
        /**
         * Destructor
         */
        ~CCVersit2Test();

    private:    // Constructors and destructors

        CCVersit2Test();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
         
         void doTestStepL();
         
         void PropertyValidationL();
         
         void EmbeddingValidationL();
         
         void EscapeSequenceL();
         
         void VJournalL();
         
         void VFreeBusyL();
         
         void Corrupt1L();
         
         void Corrupt2L();
         
         void Corrupt3L();
         
         void NotCorruptL();
         
    protected:
    
    	TBool ImportL();
    
    	TBool ExportL();        
    	
    	// Helper methods.
	    	
    	TBool CompareFilesL();
    	
    	void GetFilesL(const TDesC8& aDes8);
    
    	void GetLineL(RFileReadStream& aStream, TPtr8& aPtr);

    	void GetStringFromFile(const TDesC8& aSectName, const TDesC8& aKeyName, TPtrC& aResult);
    	
    	HBufC* Convert8To16BitL(const TDesC8& aDes8);
    
    private:    // Data
    
    	RFs iFs;
	
    	HBufC* iInputDir;	// Directory to import from
    	HBufC* iOutputDir;	// Directory to export to
    	HBufC* iImportFile;	// File to import from
    	HBufC* iExportFile;	// File to export to
    	HBufC* iCheckFile;	// File to verify export against
    	
    	TBool iOOMTesting;	// Perform out of memory testing
    	TInt iAllocCells;	// Number of allocated heap cells
    	
    	HBufC* iString;
    	
    protected:	
	
	    CICalParser* iCalParser;
    
    private:
    
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __TESTVERSIT2_H__

// End of file
