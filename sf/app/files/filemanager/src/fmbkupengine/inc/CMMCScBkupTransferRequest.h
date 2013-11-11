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
* Description: Declaration for CMMCScBkupTransferRequestBase 
*     
*
*/

#ifndef __CMMCSCBKUPTRANSFERREQUEST_H__
#define __CMMCSCBKUPTRANSFERREQUEST_H__

// System includes
#include <e32base.h>
#include <connect/sbeclient.h>

// User includes
#include "TMMCScBkupDriveFilter.h"
#include "TMMCScBkupOwnerDataType.h"
#include "TMMCScBkupArchiveVector.h"

// Namespaces
using namespace conn;

// Classes referenced
class CMMCScBkupDataOwnerInfo;
class MMMCScBkupDriver;
class TMMCScBkupArchiveVector;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupTransferRequestBase) : public CActive
    {
    public:

        /**
        * C++ destructor
        */
        ~CMMCScBkupTransferRequestBase();
    
    protected:

        /**
        * C++ default constructor
        */
        CMMCScBkupTransferRequestBase( MMMCScBkupDriver& aDriver, TMMCScBkupOwnerDataType aElementType, TBool aUpdateOperationalSizes = ETrue, TInt aPriority = CActive::EPriorityIdle );

    public: // API

        /**
        *
        */
        virtual void RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver );

    protected: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    protected: // State relation

        /**
        *
        */
        enum TState
            {
            EIdle = 0,
            EFinished,
            //
            ETransferData,
            EProcessData
            };

        /**
        *
        */
        inline TState State() const { return iState; }

        /**
        *
        */
        inline void SetState( TState aState ) { iState = aState; }

    protected: // Internal framework API

        /**
        *
        */
        virtual TBool ProcessChunkOfDataL() = 0;

        /**
        *
        */
        virtual void PrepareDataTransferL() = 0;

        /**
        *
        */
        virtual TDriveNumber CurrentDrive() const = 0;
        
        /**
        *
        */
        virtual void SingleDriveTransferCompleteL();

        /**
        *
        */
        virtual void TransferCompleteL();

    protected: // Internal methods

        /**
        *
        */
        void CompleteSelf( TInt aCompletionCode = KErrNone );

        /**
        *
        */
        inline MMMCScBkupDriver& Driver() { return iDriver; }

        /**
        *
        */
        inline CMMCScBkupDataOwnerInfo& DataOwner() { return *iDataOwner; }

        /**
        *
        */
        inline const CMMCScBkupDataOwnerInfo& DataOwner() const { return *iDataOwner; }

        /**
        *
        */
        inline TMMCScBkupArchiveVector& Info() { return iCurrentInfo; }

        /**
        *
        */
        inline const TMMCScBkupArchiveVector& Info() const { return iCurrentInfo; }
        
        /**
        *
        */
        inline TMMCScBkupOwnerDataType ElementType() const { return iElementType; }

        /**
        *
        */
        inline void DataTransferred( TInt aAmount ) { iDataTransferred += aAmount; }

        /**
        *
        */
        inline TInt AmountOfDataTransferred() const { return iDataTransferred; }
        
        /**
        *
        */
        inline void ResetDataTransferCounter() { iDataTransferred = 0; }

    private: // Internal methods

        /**
        *
        */
        void SetObserver( TRequestStatus& aObserver );

        /**
        *
        */
        void CompleteObserverRequest( TInt aCompletionCode );

    private: // Internal state methods

        /**
        *
        */
        void ProcessDataChunkL();

    private: // Member data

        // Owned externally
        //
        MMMCScBkupDriver& iDriver;
        //
        CMMCScBkupDataOwnerInfo* iDataOwner;
        //
        TRequestStatus* iObserver;

        // Owned by this class
        //
        TInt iDataTransferred;
        //
        TState iState;
        //
        TMMCScBkupArchiveVector iCurrentInfo;
        //
        const TMMCScBkupOwnerDataType iElementType;
        //
        const TBool iUpdateOperationalSizes;
    };






#endif // __CMMCSCBKUPTRANSFERREQUEST_H__

//End of File
