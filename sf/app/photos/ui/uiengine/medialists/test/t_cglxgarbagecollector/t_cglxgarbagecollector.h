/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CGlxGarbageCollector unit tests
*
*/



#ifndef __T_CGLXGARBAGECOLLECTOR_H__
#define __T_CGLXGARBAGECOLLECTOR_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include <glxmediaid.h>

//  INTERNAL INCLUDES
#include "mglxmediauser.h"

//  FORWARD DECLARATIONS
class CGlxCache;
class CGlxGarbageCollector;

//  CLASS DEFINITION
/**
 * T_CGlxGarbageCollector Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_CGlxGarbageCollector )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxGarbageCollector* NewL();
        static T_CGlxGarbageCollector* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxGarbageCollector();

    public:

        void IdleCallbackCalledL();

    private:    // Constructors and destructors

        T_CGlxGarbageCollector();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CGlxGarbageCollector_Cleanup_SubTest1L();
         void T_CGlxGarbageCollector_Cleanup_SubTest2L();
         void T_CGlxGarbageCollector_Cleanup_SubTest3L();
         void T_CGlxGarbageCollector_Cleanup_SubTest4L();
        
         void T_CGlxGarbageCollector_CleanupCachesL();

         void T_CGlxGarbageCollector_CleanupCache_SubTest1L();
         void T_CGlxGarbageCollector_CleanupCache_SubTest2L();
         void T_CGlxGarbageCollector_CleanupCache_SubTest3L();
         void T_CGlxGarbageCollector_CleanupCache_SubTest4L();
         void T_CGlxGarbageCollector_CleanupCache_SubTest5L();

         void T_CGlxGarbageCollector_CleanupMediaL_SubTest1L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest2L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest3L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest4L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest5L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest6L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest7L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest8L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest9L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest10L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest11L();
         void T_CGlxGarbageCollector_CleanupMediaL_SubTest12L();

         void T_CGlxGarbageCollector_GetAttributesInUseLL();

         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest1L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest2L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest3L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest4L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest5L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest6L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest7L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest8L();
         void T_CGlxGarbageCollector_DeleteOtherAttributes_SubTest9L();

    private:    // Internal methods

        void TimerCallback();

        void UpdateMediaL();

        void SetupCachesL();

        void AddMediaUserToCacheMediasL(CGlxCache& aCache, MGlxMediaUser& aMediaUser);
        void RemoveMediaUserFromCacheMedias(CGlxCache& aCache, MGlxMediaUser& aMediaUser);

        void AddMediaUserToMediaL(CGlxCache& aCache, TInt aIndex, MGlxMediaUser& aMediaUser);
        void RemoveMediaUserFromMedia(CGlxCache& aCache, TInt aIndex, MGlxMediaUser& aMediaUser);

        void CheckCache1MediaAttributes(const CGlxMedia& aMedia, TInt aAttributeCount, 
            TBool aTitleSupported, TBool aSubtitleSupported, TBool aSystemItemSupported, 
            TBool aErrorSupported = EFalse);
        void CheckCache2MediaAttributes(const CGlxMedia& aMedia, TInt aAttributeCount, 
            TBool aTitleSupported, TBool aSubtitleSupported, TBool aSystemItemSupported, 
            TBool aErrorSupported = EFalse);
        void CheckCache3MediaAttributes(const CGlxMedia& aMedia, TInt aAttributeCount, 
            TBool aThumbnailSupported, TBool aTitleSupported, TBool aDateSupported, 
            TBool aSizeSupported, TBool aErrorSupported = EFalse);

        void CheckCache1MediaErrors(const CGlxMedia& aMedia, TBool aError, 
            TBool aTitleError = EFalse, TBool aSubtitleError = EFalse, 
            TBool aSystemItemError = EFalse);
        void CheckCache2MediaErrors(const CGlxMedia& aMedia, TBool aError, 
            TBool aTitleError = EFalse, TBool aSubtitleError = EFalse, 
            TBool aSystemItemError = EFalse);
        void CheckCache3MediaErrors(const CGlxMedia& aMedia, TBool aError, 
            TBool aThumbnailError = EFalse, TBool aTitleError = EFalse, 
            TBool aDateError = EFalse, TBool aSizeError = EFalse);

    private:    // Test classes

        // Test media user
        class CGlxMediaUserTest : public CBase, public MGlxMediaUser
            {
            public:
                static CGlxMediaUserTest* NewL(const RArray<TMPXAttribute>& aAttributes);
                virtual ~CGlxMediaUserTest();

            private:
                CGlxMediaUserTest();
                void ConstructL(const RArray<TMPXAttribute>& aAttributes);

            private: // From MGlxMediaUser
                void GetRequiredAttributesL(TInt aIndex, RArray<TMPXAttribute>& aAttributes);
                void HandleAttributesAvailableL(TInt /*aIndex*/, 
                                                const RArray<TMPXAttribute>& /*aAttributes*/) {};
                void RemoveReference(TInt aIndex);
                void HandleError(TInt /*aError*/) {};

            private: // Data
                RArray<TMPXAttribute> iAttributes;
            };

        // Test timer
        class CTimerTest : public CTimer
            {
            public:
                static CTimerTest* NewL(T_CGlxGarbageCollector* aTestGarbageCollector);
                virtual ~CTimerTest();

            public:
                void StartTimer(TInt aSeconds);

            private:
                CTimerTest(T_CGlxGarbageCollector* aTestGarbageCollector);
                void ConstructL();

            private: // From CTimer/CActive
                void RunL();

            private:
                T_CGlxGarbageCollector* iTestGarbageCollector;
            };

    public:    // Data
		
        static T_CGlxGarbageCollector* iGarbageCollectorTest;

    private:    // Data
		
        CGlxGarbageCollector* iCGlxGarbageCollector;

        RPointerArray<CGlxCache> iCaches;

        CActiveSchedulerWait iWait;

        TInt iIdleCallbackCount;

        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif      //  __T_CGLXGARBAGECOLLECTOR_H__

// End of file
