/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     Scans for names of the files according to array.
*
*/

#ifndef __CMMCSCBKUPENGINE_H__
#define __CMMCSCBKUPENGINE_H__

// User includes
#include "MMCScBkupOperations.h"

// Classes referenced
class RFs;
class CMMCScBkupEngineImpl;
class CMMCScBkupOpParamsBase;
class MMMCScBkupEngineObserver;
class CMMCScBkupArchiveInfo;

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupEngine) : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMMCScBkupEngine* NewL( RFs& aFsSession );

        /**
        * Destructor
        */
        IMPORT_C ~CMMCScBkupEngine();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupEngine();

        /**
        * Second phase constructor
        */
        void ConstructL( RFs& aFsSession );


    public: // API

        /**
        * NB. aParams are IMMEDIATELY owned by the engine. Do not pass them in
        * left on the cleanup stack.
        */
        IMPORT_C void StartOperationL(TMMCScBkupOperationType aOperation, MMMCScBkupEngineObserver& aObserver, CMMCScBkupOpParamsBase* aParams );

        /**
        *
        */
        IMPORT_C void CancelOperation();
        
        /**
        *
        */
        IMPORT_C TBool ValidArchiveForRestore( const TDesC& aFileName );

        /**
        *
        */
        IMPORT_C TInt64 TotalOperationSizeL() const;

        /**
        *
        */
        IMPORT_C TBool RebootRequired() const;

        /**
        *
        */
        IMPORT_C TBool DeleteArchivesL( RPointerArray< CMMCScBkupArchiveInfo >& aArchives ) const;
        
        /**
        *
        */
        IMPORT_C void ListArchivesL(
            RPointerArray< CMMCScBkupArchiveInfo >& aArchives,
            CMMCScBkupOpParamsBase* aParams,
            const TUint32 aDriveAttMatch,
            const TInt aDriveMatch = KErrNotFound ) const;

    private: // Member data

        // Real engine
        CMMCScBkupEngineImpl* iEngine;
    };




#endif // __CMMCSCBKUPENGINE_H__

//End of File
