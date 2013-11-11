/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration of CMMCScBkupArchiveDataManager
*     
*
*/

#ifndef __CMMCSCBKUPARCHIVEDATAMANAGER_H__
#define __CMMCSCBKUPARCHIVEDATAMANAGER_H__

// System includes
#include <f32file.h>
#include <s32strm.h>

// User includes
#include "CMMCScBkupBufferManagers.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "TMMCScBkupArchiveVector.h"
#include "RMMCScBkupArchiveStreams.h"
#include "MMCScBkupOperations.h"
#include "MMCScBkupConfig.h"

// Classes referenced
class CEZCompressor;
class CEZDecompressor;
class RMMCScBkupArchiveBuf;
class MMMCScBkupProgressObserver;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupArchiveDataManager) : public CActive, public MMMCScBkupArchiveDataInterface
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupArchiveDataManager* NewL( RFs& aFsSession, RFile64& aFile, MMMCScBkupProgressObserver& aProgressManager );

        /**
        * C++ destructor
        */
        ~CMMCScBkupArchiveDataManager();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupArchiveDataManager( RFs& aFsSession, RFile64& aFile, MMMCScBkupProgressObserver& aProgressManager );

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        * Leaves on error, returns offset
        */
        TInt CurrentOffsetL() const;

        /**
        * Returns error or offset
        */
        TInt CurrentOffset() const;

    private: // For RMMCScBkupArchiveBuf only
   
        /**
        *
        */
        void SetCurrentVector(const TMMCScBkupArchiveVector& aInfo);


    private: // From MMMCScBkupArchiveDataInterface
        RFs& ADIFsSession() const;
        RFile64& ADIRawArchiveFile() const;
        //
        const TMMCScBkupArchiveVector& ADICurrentArchiveVectorInfo() const;
        const TMMCScBkupArchiveVector& ADIOverallArchiveVectorInfo() const;
        //
        RWriteStream& ADIWriteStreamUncompressedLC( TInt aPos );
        RReadStream& ADIReadStreamUncompressedLC( TInt aPos );
        //
        const TMMCScBkupArchiveVector& ADIWriteL( const TDesC8& aData );
        const TMMCScBkupArchiveVector& ADIReadL( TDes8& aSink, const TMMCScBkupArchiveVector& aInfo );
        //
        void ADIWriteFileL( const TDesC& aSourceFileName, TRequestStatus& aObserver );
        void ADIReadFileL( const TDesC& aDestinationFileName, const TMMCScBkupArchiveVector& aInfo, TRequestStatus& aStatus );
        //
        void ADIWriteCompressedL( const TDesC8& aData, TRequestStatus& aObserver );
        void ADIReadDecompressedL( TDes8& aSink, const TMMCScBkupArchiveVector& aInfo, TRequestStatus& aStatus ) ;
        //
        void ADIAsynchronousCancel();
        void ADIResetResources(TMMCScBkupOperationType aType);
#ifdef RD_FILE_MANAGER_BACKUP
        void ADIActivateCrcCalculation(TBool aType) { iCrcCalcActivated = aType; }
        TUint32 ADIArchiveCrc() { return iCrc; }
#endif


    private: // Internal enumerations

        /**
        *
        */
        enum TOperation
            {
            EOperationIdle = 0,
            EOperationCompressing,
            EOperationDecompressing
            };
    
    private: // Internal methods

        /**
        *
        */
        void EnsureCompressorExistsL(MEZBufferManager& aBufferManager);

        /**
        *
        */
        void EnsureDecompressorExistsL(MEZBufferManager& aBufferManager);

        /**
        *
        */
        void SetObserver(TRequestStatus& aStatus);

        /**
        *
        */
        void CompleteSelf(TInt aCompletionCode = KErrNone);

        /**
        *
        */
        void CompleteObserver(TInt aCompletionCode = KErrNone);

        /**
        *
        */
        void SetOperation(TOperation aOperation);
        
#ifdef RD_FILE_MANAGER_BACKUP
        /**
        *
        */
        void CalculateCrc( const TAny* aPtr,TInt aLength );
#endif


    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private: // Member data

        // Owned externally:
        // 
        RFs& iFsSession;
        //
        RFile64& iFile;
        //
        MMMCScBkupProgressObserver& iProgressManager;
        //
        TRequestStatus* iObserverStatus;

        // Owned by this object
        //
        TOperation iOperation;
        //
        CMMCScBkupBufferManagerBase* iBufferManager;
        //
        CEZCompressor* iCompressor;
        //
        CEZDecompressor* iDecompressor;
        //
        TMMCScBkupArchiveVector iCurrentVectorInfo;
        //
        TMMCScBkupArchiveVector iOverallArchiveVectorInfo;
        //
        RMMCScBkupArchiveWriteStream iWriteStream;
        //
        RMMCScBkupArchiveReadStream iReadStream;
#ifdef RD_FILE_MANAGER_BACKUP
        //
        TBool iCrcCalcActivated;
        //
        TUint32 iCrc;
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
        TUint32 iTotalTickCount;
#endif
#endif // RD_FILE_MANAGER_BACKUP

        friend class RMMCScBkupArchiveBuf;
    };




#endif // __CMMCSCBKUPARCHIVEDATAMANAGER_H__

//End of File
