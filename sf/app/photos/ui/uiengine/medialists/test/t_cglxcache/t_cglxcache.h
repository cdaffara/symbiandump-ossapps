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
* Description:   CGlxCache unit tests
*
*/



#ifndef __T_CGLXCACHE_H__
#define __T_CGLXCACHE_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include <e32base.h>

#include <glxmediaid.h>

//  INTERNAL INCLUDES
#include "mglxcacheobserver.h"
#include "mglxmediauser.h"

//  FORWARD DECLARATIONS
class CGlxCache;
class CGlxMedia;

//  CLASS DEFINITION
/**
 * T_CGlxCache Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_CGlxCache )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxCache* NewL();
        static T_CGlxCache* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxCache();

    private:    // Constructors and destructors

        T_CGlxCache();
        void ConstructL();

    private:    // New methods

        void SetupL();
        
        void Teardown();
        
        void T_CGlxCache_IdSpaceIdL();
        
        void T_CGlxCache_MediaUpdatedLL();
        
        void T_CGlxCache_UpdateMediaLL();
        
        void T_CGlxCache_HandleItemModifiedL();
        
        void T_CGlxCache_MediaL();
        
        void T_CGlxCache_FindItemForceCreateLL();
        
        void T_CGlxCache_CountL();
        
        void T_CGlxCache_Media2L();
        
        void T_CGlxCache_Delete_SubTest1L();
        
        void T_CGlxCache_Delete_SubTest2L();
        
        void T_CGlxCache_Delete_SubTest3L();
        
        void T_CGlxCache_Delete_SubTest4L();
        
        void T_CGlxCache_Delete_SubTest5L();
        
        void T_CGlxCache_ReserveUsersLL();
        
    private:    // Internal methods

        void SetupCacheL();


    private:    // Data
		
        CGlxCache* iCGlxCache;
        EUNIT_DECLARE_TEST_TABLE; 


    private:    // Test classes

        // Test cache observer
        class CGlxCacheObserverTest : public CBase, public MGlxCacheObserver
            {
            public:
                static CGlxCacheObserverTest* NewL();
                virtual ~CGlxCacheObserverTest();

            public:
                TBool AttributesAvailableNotified() const;
                void ResetCalls();

                const TGlxIdSpaceId& IdSpaceId() const;
                const TGlxMediaId& MediaId() const;
                const RArray<TMPXAttribute>& Attributes() const;
                const CGlxMedia* Media() const;

            private:
                CGlxCacheObserverTest();
                void ConstructL();

            private: // From MGlxCacheObserver
                void HandleAttributesAvailableL(const TGlxIdSpaceId& aIdSpaceId, 
                                                const TGlxMediaId& aMediaId, 
                                                const RArray<TMPXAttribute>& aAttributes, 
                                                const CGlxMedia* aMedia);

            private: // Data
                TBool iAttributesAvailableNotified;

                TGlxIdSpaceId iIdSpaceId;
                TGlxMediaId iMediaId;
                RArray<TMPXAttribute> iAttributes;
                CGlxMedia* iMedia;
            };

        // Test media user
        class CGlxMediaUserTest : public CBase, public MGlxMediaUser
            {
            public:
                static CGlxMediaUserTest* NewL();
                virtual ~CGlxMediaUserTest();

            public:
                TBool AttributesAvailableNotified() const;
                TBool RemoveReferenceCalled() const;
                void ResetCalls();

                const TGlxIdSpaceId& IdSpaceId() const;
                const TGlxMediaId& MediaId() const;
                const RArray<TMPXAttribute>& Attributes() const;

            private:
                CGlxMediaUserTest();
                void ConstructL();

            private: // From MGlxMediaUser
                void GetRequiredAttributesL(TInt /*aIndex*/, 
                                            RArray<TMPXAttribute>& /*aAttributes*/) {};
                void HandleAttributesAvailableL(TInt aIndex, 
                                                const RArray<TMPXAttribute>& aAttributes);
				void HandleAttributesAvailableL(const TGlxIdSpaceId& aIdSpaceId, 
        								const TGlxMediaId& aItemId, 
        								const RArray<TMPXAttribute>& aAttributes);
				                                                
                void RemoveReference(TInt aIndex);
                void RemoveReference(CGlxMedia& aMedia);
                void HandleError(TInt /*aError*/) {};

            private: // Data
                TBool iAttributesAvailableNotified;
                TBool iRemoveReferenceCalled;

                TGlxIdSpaceId iIdSpaceId;
                TGlxMediaId iMediaId;
                RArray<TMPXAttribute> iAttributes;
            };
    };

#endif      //  __T_CGLXCACHE_H__

// End of file
