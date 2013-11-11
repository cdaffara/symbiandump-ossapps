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

#ifndef __CMMCSCBKUPENGINEIMPL_H__
#define __CMMCSCBKUPENGINEIMPL_H__

// System includes
#include <connect/sbeclient.h>

// User includes
#include "MMCScBkupConfig.h"
#include "MMMCScBkupDriver.h"
#include "MMCScBkupStateIds.h"
#include "MMCScBkupOperations.h"
#include "MMMCScBkupEngineObserver.h"
#include "MMMCScBkupProgressObserver.h"

// Namespaces
using namespace conn;

// Classes referenced
class RFs;
class CMMCScBkupState;
class CMMCScBkupArchive;
class CMMCScBkupStateFactory;
class CMMCScBkupOpParamsBase;
class MMMCScBkupEngineObserver;
class CMMCScBkupFileListCollection;
class CMMCScBkupDataOwnerCollection;
class MMMCScBkupArchiveDataInterface;
class CMMCScBkupArchiveInfo;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupEngineImpl) : public CActive, public MMMCScBkupDriver, public MMMCScBkupProgressObserver
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CMMCScBkupEngineImpl* NewL( RFs& aFsSession );

        /**
        * Destructor
        */
        ~CMMCScBkupEngineImpl();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupEngineImpl( RFs& aFsSession );

        /**
        * Second phase constructor
        */
        void ConstructL( );


    public: // API

        /**
        *
        */
        void StartOperationL(TMMCScBkupOperationType aOperation, MMMCScBkupEngineObserver& aObserver, CMMCScBkupOpParamsBase* aParams );

        /**
        *
        */
        void CleanupL( TInt aError = KErrNone );
        
        /**
        *
        */
        TBool ValidArchiveForRestoreL( const TDesC& aFileName );
        
        /**
        *
        */
        TInt64 TotalOperationSizeL() const;
 
        /**
        *
        */
        TBool RebootRequired() const;
        
        /**
        *
        */
        TBool DeleteArchivesL( RPointerArray< CMMCScBkupArchiveInfo >& aArchives ) const;
        
        /**
        *
        */
        void ListArchivesL(
            RPointerArray< CMMCScBkupArchiveInfo >& aArchives,
            CMMCScBkupOpParamsBase* aParams,
            const TUint32 aDriveAttMatch,
            const TInt aDriveMatch );
        
    private: // From MMMCScBkupDriver
        void DrvHandleStateExecutionCompleteL( const CMMCScBkupState& aState );
        void DrvHandleStateExecutionCompleteL( const CMMCScBkupState& aState, TMMCScBkupStateId aNextState );
        void DrvHandleStateExecutionErrorL( const CMMCScBkupState& aState, TInt aError );
    
    private: // From MMMCScBkupDriver
        MMMCScBkupArchiveDataInterface& DrvADI() const;
        CMMCScBkupArchive& DrvArchive() const;
        CSBEClient& DrvSecureBackupClient() const;
        TMMCScBkupOperationType DrvOperation() const;
        CMMCScBkupOpParamsBase& DrvParamsBase() const;
        CMMCScBkupDataOwnerCollection& DrvDataOwners() const;
        RPointerArray<CMMCScBkupDataOwnerCollection>& DrvDataOwnersAll();
        CMMCScBkupFileListCollection& DrvFileList() const;
        TBool DrvLastCategory() const;
        void DrvStoreTotalProgress(TInt64 aProgress);
        TInt64 DrvTotalProgress() const;
        MMMCScBkupProgressObserver& DrvProgressHandler() const;
    
    private: // From MMMCScBkupProgressObserver
        void MMCScBkupHandleProgress( TInt aAmountCompleted );
        void MMCScBkupHandleProgressDomainUnderstood( TInt aTotalProgressAmount );
        TInt MMCScBkupHandleFreeSpace( TInt aPercentualFree );
        void MMCScBkupStartBackuping( TBool aProceed );

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private: // Internal

        /**
        *
        */
        void CompleteOwnRequest(TInt aCompletionCode = KErrNone, TBool aSetActive = ETrue);

        /**
        *
        */
        void NotifyObserver(MMMCScBkupEngineObserver::TEvent aEvent, TInt aAssociatedData = KErrNone);

        /**
        *
        */
        void PrepareForBackupL(TBool aPartial);

        /**
        *
        */
        void PrepareForRestoreL(TBool aPartial);

        /**
        *
        */
        void SetParameters(CMMCScBkupOpParamsBase* aParameters);

        /**
        *
        */
        void CreateFactoryL(TMMCScBkupOperationType aOperation);

        /**
        *
        */
        TBool CurrentStateAvailable() const;

        /**
        *
        */
        TMMCScBkupStateId CurrentStateId() const;

        /**
        *
        */
        CMMCScBkupState& CurrentState();

        /**
        *
        */
        const CMMCScBkupState& CurrentState() const;

        /**
        *
        */
        void PrepareNextStateL( TMMCScBkupStateId aCurrentStateId );

        /**
        *
        */
        void DestroyCurrentState();

        /**
        *
        */
        void ExecuteStateL();

        /**
        *
        */
        void PrepareObjectsL();

#ifdef DEBUGGING_DATA_TRANSFER

        /**
        *
        */
        void CleanBackupFilesL();
        
        /**
        *
        */
        void ClearRestoreFilesL();
#endif

    private: // Member data

        // Referenced objects
        // file server session
        RFs& iFsSession;
        // backup engine observer
        MMMCScBkupEngineObserver* iObserver;

        // Owned objects
        //
        TInt64 iCumulativeProgress;
        // Secure backup engine client
        CSBEClient* iSBEClient;
        // Archive array
        RMMCScBkupPointerArray<CMMCScBkupArchive> iArchives;
        // Current operation
        TMMCScBkupOperationType iOperationType;
        // Operational parameters
        CMMCScBkupOpParamsBase* iParameters;
        //
        CMMCScBkupState* iCurrentState;
        // Current factory
        CMMCScBkupStateFactory* iFactory;
        //
        RPointerArray<CMMCScBkupDataOwnerCollection> iDataOwners;
        //
        RPointerArray<CMMCScBkupFileListCollection> iFileLists;
        //
        TInt iCurrentArchive;
        //
        TInt64 iTotalProgress;
        //
        TBool iActiveDataProcessingOngoing;
    };




#endif // __CMMCSCBKUPENGINEIMPL_H__

//End of File
