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
* Description: Declaration for CMMCScBkupWriteDataTransferRequestBase
*     
*
*/

#ifndef __CMMCSCBKUPTRANSFERWRITEREQUEST_H__
#define __CMMCSCBKUPTRANSFERWRITEREQUEST_H__

// User includes
#include "MMCScBkupConfig.h"
#include "MMCScBkupSBEUtils.h"
#include "TMMCScBkupOwnerDataType.h"
#include "CMMCScBkupTransferRequest.h"
#include "CMMCScBkupDataOwnerInfo.h"

// Classes referenced
class CMMCScBkupArchiveFooter;
class CMMCScBkupDriveAndOperationTypeManager;

/**
*
*
* @since 3.0
*/
class MMMCScBkupIndexHandler
    {
    public:

        /**
        *
        */
        virtual void AddIndexRecordL( CMMCScBkupArchiveFooter& aFooter, CMMCScBkupDataOwnerInfo& aDataOwner, const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive ) = 0;
    };






/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupWriteDataTransferRequestBase) : public CMMCScBkupTransferRequestBase
    {
    public:

        /**
        * C++ destructor
        */
        ~CMMCScBkupWriteDataTransferRequestBase();

    public: // API

        /**
        *
        */
        void RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver, const CMMCScBkupDriveAndOperationTypeManager& aDriveAndOperations );

    protected:

        /**
        * C++ default constructor
        */
        CMMCScBkupWriteDataTransferRequestBase( MMMCScBkupDriver& aDriver, 
                                                MMMCScBkupIndexHandler& aIndexHandler,
                                                TMMCScBkupOwnerDataType aElementType,
                                                TSBDerivedType aExpectedTransferType,
                                                TInt aPriority = CActive::EPriorityIdle );

    protected: // New Framework API

        /**
        *
        */
        virtual CSBGenericTransferType* PrepareTransferTypeL( const CSBGenericDataType& aGeneric, 
            TDriveNumber aDrive, TInt aVersion ) const = 0;

    protected: // From CMMCScBkupTransferRequestBase

        /**
        *
        */
        TBool ProcessChunkOfDataL();

        /**
        *
        */
        void PrepareDataTransferL();

        /**
        *
        */
        TDriveNumber CurrentDrive() const;

    private: // Internal methods
    
        /**
        *
        */
        inline void SetCurrentDrive( TDriveNumber aDrive ) { iCurrentDrive = aDrive; }

        /**
        *
        */
        void ValidateTransferInfoL( CSBGenericTransferType* aInfo );

        /**
        *
        */
        void RequestDataTransferL();

    private: // From CActive
        TInt RunError( TInt aError );

    private:

        /**
        *
        */
#ifdef DEBUGGING_DATA_TRANSFER
        void DumpTransferDataL( RFs& aFsSession, const TDesC8& aData ) const;
#endif


    private: // Member data

        // Owned by this class
        //
        TMMCScBkupDriveFilter iDriveFilter;
        //
        MMMCScBkupIndexHandler& iIndexHandler;
        //
        const TSBDerivedType iExpectedTransferType;
        //
        const CMMCScBkupDriveAndOperationTypeManager* iDriveAndOperations;
        //
        RWriteStream iStream;
        //
        TBool iStreamIsOpen;
        //
        CSBGenericTransferType* iTransferType;
        //
        TBool iAdditionalTransferRequired;
        //
        TDriveNumber iCurrentDrive;
    };











/**
*
*
* @since 3.0
*/
template <class Type>
class CMMCScBkupWriteDataTransferRequest : public CMMCScBkupWriteDataTransferRequestBase
    {
    public: // Construction

        /**
        * Static constructor
        */
        inline static CMMCScBkupWriteDataTransferRequest* NewL( MMMCScBkupDriver& aDriver, 
                                                         MMMCScBkupIndexHandler& aIndexHandler,
                                                         TMMCScBkupOwnerDataType aElementType,
                                                         TSBDerivedType aExpectedTransferType,
                                                         Type aTransferEnumType,
                                                         TInt aPriority = CActive::EPriorityIdle )
            {
            CMMCScBkupWriteDataTransferRequest<Type>* self = new(ELeave) CMMCScBkupWriteDataTransferRequest<Type>( aDriver, aIndexHandler, aElementType, aExpectedTransferType, aTransferEnumType, aPriority );
            return self;
            }

    private: // Internal construction

        /**
        * C++ default constructor
        */
        inline CMMCScBkupWriteDataTransferRequest( MMMCScBkupDriver& aDriver, 
                                                   MMMCScBkupIndexHandler& aIndexHandler,
                                                   TMMCScBkupOwnerDataType aElementType,
                                                   TSBDerivedType aExpectedTransferType,
                                                   Type aTransferEnumType,
                                                   TInt aPriority )
        :   CMMCScBkupWriteDataTransferRequestBase( aDriver, aIndexHandler, aElementType, aExpectedTransferType, aPriority ), 
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





#endif // __CMMCSCBKUPTRANSFERWRITEREQUEST_H__

// End of File
