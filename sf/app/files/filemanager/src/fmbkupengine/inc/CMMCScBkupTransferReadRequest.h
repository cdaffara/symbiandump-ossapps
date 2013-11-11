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
* Description: Declaration for CMMCScBkupReadDataTransferRequestBase
*     
*
*/

#ifndef __CMMCSCBKUPTRANSFERREADREQUEST_H__
#define __CMMCSCBKUPTRANSFERREADREQUEST_H__

// User includes
#include "MMCScBkupConfig.h"
#include "MMCScBkupSBEUtils.h"
#include "TMMCScBkupOwnerDataType.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "CMMCScBkupTransferRequest.h"
#include "CMMCScBkupIndexWithIdentifier.h"

// Classes referenced
class MMMCScBkupDriver;
class CMMCScBkupArchiveFooter;

// Constants
const TInt KMMCScBkupDefaultChunkReadSize = 4096; // 4k read chunks

// CMMCScBkupIndexEntry

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupReadDataTransferRequestBase) : public CMMCScBkupTransferRequestBase
    {
    public:

        /**
        * C++ destructor
        */
        ~CMMCScBkupReadDataTransferRequestBase();

    protected:

        /**
        * C++ default constructor
        */
        CMMCScBkupReadDataTransferRequestBase( MMMCScBkupDriver& aDriver, 
                                               TMMCScBkupOwnerDataType aElementType,
                                               TInt aChunkSize = KMMCScBkupDefaultChunkReadSize,
                                               TInt aPriority = CActive::EPriorityIdle );

    public: // API

        /**
        *
        */
        void RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver, const RArray<TMMCScBkupArchiveDriveAndVector>& aEntries );

    protected: // New Framework API

        /**
        *
        */
        virtual CSBGenericTransferType* PrepareTransferTypeL( const CSBGenericDataType& aGeneric, 
            TDriveNumber aDrive, TInt aVersion ) const = 0;

    private: // From CMMCScBkupTransferRequest
        TBool ProcessChunkOfDataL();
        void PrepareDataTransferL();

    private: // Internal methods

        /**
        *
        */
        void ReadChunkL( TDes8& aSink, TInt aLength );

        /**
        *
        */
        TBool DoProcessChunkOfDataL();

        /**
        *
        */
        TBool PrepareForNextEntry();

        /**
        *
        */
        TDriveNumber CurrentDrive() const;

        /**
        *
        */
        const TMMCScBkupArchiveVector& CurrentReadInfo() const;

        /**
        *
        */
        const TMMCScBkupArchiveDriveAndVector& CurrentEntry() const;
        
        /**
        *
        */
        inline TInt CurrentOffset() const { return iCurrentOffset; }

    private: // From CActive
        TInt RunError( TInt aError );

    private: // Debugging support

        /**
        *
        */
#ifdef DEBUGGING_DATA_TRANSFER
        void DumpTransferDataL( RFs& aFsSession, const TDesC8& aData ) const;
#endif

    private: // Member data

        // Owned externally
        //
        const RArray<TMMCScBkupArchiveDriveAndVector>* iEntries;

        // Owned by this object

        // Pointer to the SBE's shared chunk. It *must* be a reference
        // because otherwise the SBE loses track of what we actually write
        // to the chunk!
        TPtr8* iTransferChunkPointer;
        //
        const TInt iReadChunkSize;
        //
        TInt iCurrentIndex;
        //
        TBool iFinishedSupplyingData;
        //
        CSBGenericTransferType* iTransferType;
        //
        TInt iCurrentOffset;
        //
        HBufC8* iTemporaryTransferSink;
    };









/**
*
*
* @since 3.0
*/
template <class Type>
class CMMCScBkupReadDataTransferRequest : public CMMCScBkupReadDataTransferRequestBase
    {
    public: // Construction

        /**
        *
        */
        inline static CMMCScBkupReadDataTransferRequest* NewL( MMMCScBkupDriver& aDriver, 
                                                        TMMCScBkupOwnerDataType aElementType,
                                                        Type aTransferEnumType,
                                                        TInt aChunkSize = KMMCScBkupDefaultChunkReadSize,
                                                        TInt aPriority = CActive::EPriorityIdle )
            {
            CMMCScBkupReadDataTransferRequest<Type>* self = new(ELeave) CMMCScBkupReadDataTransferRequest<Type>( aDriver, aElementType, aTransferEnumType, aChunkSize, aPriority );
            return self;
            }

    private: // Internal construction

        /**
        * C++ default constructor
        */
        inline CMMCScBkupReadDataTransferRequest( MMMCScBkupDriver& aDriver, 
                                           TMMCScBkupOwnerDataType aElementType,
                                           Type aTransferEnumType,
                                           TInt aChunkSize = KMMCScBkupDefaultChunkReadSize,
                                           TInt aPriority = CActive::EPriorityIdle )
        :   CMMCScBkupReadDataTransferRequestBase( aDriver, aElementType, aChunkSize, aPriority), 
            iTransferEnumType( aTransferEnumType )
            {
            }

    private: // From CMMCScBkupTransferRequest
        inline CSBGenericTransferType* PrepareTransferTypeL( const CSBGenericDataType& aGeneric, 
            TDriveNumber aDrive, TInt aVersion ) const
            {
            CSBGenericTransferType* transferType = MMCScBkupSBEUtils::TransferTypeLC( aGeneric, aDrive, 
                iTransferEnumType, aVersion );
            CleanupStack::Pop( transferType );
            return transferType;
            }

    private: // Member data

        //
        Type iTransferEnumType;
    };



#endif // __CMMCSCBKUPTRANSFERREADREQUEST_H__

// End of File
