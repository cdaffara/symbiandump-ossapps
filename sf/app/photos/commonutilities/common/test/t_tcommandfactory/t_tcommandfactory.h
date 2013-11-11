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



#ifndef __T_TCOMMANDFACTORY_H__
#define __T_TCOMMANDFACTORY_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxcommandparser.h"

//  FORWARD DECLARATIONS

class CMPXCollectionPath;
//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_tcommandfactory )
	: public CEUnitTestSuiteClass, public MGlxCommandParserCallback
    {
    public:
        enum TTest
    	{
        ENoTest,
    	ETestAdd1,
    	ETestAdd2,
    	ETestAdd3,
    	ETestAdd4,
        ETestRemove,
        ETestSet4,
        ETestSet5,
        ETestThumbnailCleanup
        };
    public:     // Constructors and destructors

    	/**
         * Two phase construction
         */
        static t_tcommandfactory* NewL();
        static t_tcommandfactory* NewLC();
        /**
         * Destructor
         */
        ~t_tcommandfactory();

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
    
            void ThumbnailCleanupL();
	private:    // Constructors and destructors
        t_tcommandfactory();
        void ConstructL();

    
   private:    // New methods
   		void TestRemoveL();
         
	     void TestSet4L();
         
         void TestSet5L();
	     
         void TestAdd1L();
	     
	     void TestAdd2L();
	     
	     void TestAdd3L();
	     
	     void TestAdd4L();
	     
	     void TestThumbnailCleanupL();
	     
	     void SetupL();
         
	     void Teardown();
        
         void T_TCommandParser_ParseLL();
          
         CMPXCollectionPath* PathFactoryLC(const RArray<TGlxMediaId>& aArray);
         
         void PopulateArrayL(RArray<TGlxMediaId>& aArray);
         
         void PopulateArrayAltL(RArray<TGlxMediaId>& aArray);
         
         TBool CompareArray(const RArray<TGlxMediaId>& aArray1, const RArray<TGlxMediaId>& aArray2);
         
         /**
          * @param aDest is pushed onto the cleanup stack before population. This is done as 
          * a convenience to the method user.
          */
         void ConvertArrayLC(RArray<TMPXItemId>& aDest, const RArray<TGlxMediaId>& aSource);
    private:    // Data
		
    	// Test data
    	RArray<TGlxMediaId> iArray1;
    	RArray<TGlxMediaId> iArray2;
    	TGlxMediaId iId1;
    	TUid iUid1;
    	TUint iUint1;
    	HBufC* iDesc1;
    	TBool iCallbackCalled;   	
    	TTest iCurrentTest;
      EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_TCOMMANDFACTORY_H__

// End of file
