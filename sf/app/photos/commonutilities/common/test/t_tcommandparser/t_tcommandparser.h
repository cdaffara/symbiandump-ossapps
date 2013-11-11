/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/



#ifndef __T_TCOMMANDPARSER_H__
#define __T_TCOMMANDPARSER_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include <glxmediageneraldefs.h>
//  INTERNAL INCLUDES
#include "glxcommandparser.h"

//  FORWARD DECLARATIONS

class CMPXCollectionPath;
//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_tcommandparser )
	: public CEUnitTestSuiteClass, public MGlxCommandParserCallback
    {
    public:
        enum TTest
    	{
    	ENoTest,
    	ETestRemove1,
    	ETestRemove2,
    	ETestAdd1,
    	ETestAdd2,
    	ETestAdd3,
    	ETestAdd4,
    	ETestSet1,
    	ETestSet2,
    	ETestSet3,
        ETestSet4,
        ETestSet5
    	};
    public:     // Constructors and destructors

    	/**
         * Two phase construction
         */
        static t_tcommandparser* NewL();
        static t_tcommandparser* NewLC();
        /**
         * Destructor
         */
        ~t_tcommandparser();

    public: // from MGlxCommandParserCallback
    	
  
    	    void AddToContainerL(const RArray<TGlxMediaId>& aSourceIds, const RArray<TGlxMediaId>& aTargetContainers);
    	
    	    void AddContainerL(const TDesC& aContainerName);
    	      
    	    void AddToContainerL(const TDesC& aSourceUri, const RArray<TGlxMediaId>& aTargetContainers);    
    
    	    void CopyL(const RArray<TGlxMediaId>& aSourceIds, const TDesC& aDrive);
    
    	    void MoveL(const RArray<TGlxMediaId>& aSourceIds, const TDesC& aDrive);
  
    	    void RemoveFromContainerL(const RArray<TGlxMediaId>& aItemIds, const TGlxMediaId& aContainerId);
    
    	    void DeleteL(const RArray<TGlxMediaId>& aItemIds);
    	    
    	    void RenameL(const TGlxMediaId& aSourceItemId, const TDesC& aTitle);
   
    	    void SetDescriptionL(const RArray<TGlxMediaId>& aItemIds, const TDesC& aDescription);    
 	    
    	    void SetCaptureLocationL(const RArray<TGlxMediaId>& aItemIds, const TCoordinate& aCoordinate);
    	    
    	    void ThumbnailCleanupL() {} ;
	private:    // Constructors and destructors
        t_tcommandparser();
        void ConstructL();

    
   private:    // New methods

	     void TestRemove1L();
	     void TestRemove2L();
	     void TestAdd1L();
	     void TestAdd2L();
	     void TestAdd3L();
	     void TestAdd4L();
	     void TestSet1L();
	     void TestSet2L();
	     void TestSet3L();
         void TestSet4L();
         void TestSet5L();
         
	     void SetupL();
         void Teardown();
        
         void T_TCommandParser_ParseLL();
          
         CMPXCommand* CommandFactoryLC();
         
         CMPXCollectionPath* PathFactoryLC(const RArray<TGlxMediaId>& aArray);
         
         void PopulateArrayL(RArray<TGlxMediaId>& aArray);
         void PopulateArrayAltL(RArray<TGlxMediaId>& aArray);
         
         TBool CompareArray(const RArray<TGlxMediaId>& aArray1, const RArray<TGlxMediaId>& aArray2);
    private:    // Data
		
    	// Test data
    	RArray<TGlxMediaId> iArray1;
    	RArray<TGlxMediaId> iArray2;
    	TGlxMediaId iId1;
    	TUid iUid1;
    	TUint iUint1;
    	HBufC* iDesc1;
        TCoordinate iCoordinate;        
    	TBool iCallbackCalled;    	
    	TTest iCurrentTest;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_TCOMMANDPARSER_H__

// End of file
