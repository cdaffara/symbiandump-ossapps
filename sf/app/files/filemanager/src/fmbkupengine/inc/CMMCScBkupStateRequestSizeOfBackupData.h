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
* Description: Declaration for CMMCScBkupStateRequestSizeOfBackupData
*     
*
*/

#ifndef __CMMCSCBKUPSTATEREQUESTSIZEOFBACKUPDATA_H__
#define __CMMCSCBKUPSTATEREQUESTSIZEOFBACKUPDATA_H__

// User includes
#include "MMCScBkupOperations.h"
#include "CMMCScBkupState.h"
#include "CMMCScBkupDriveSpecificRequest.h"

// Classes referenced
class CMMCScBkupDataOwnerInfo;
class CMMCScBkupStateRequestSizeOfDataOwner;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateRequestSizeOfBackupData) : public CMMCScBkupState
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateRequestSizeOfBackupData* NewL( MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupStateRequestSizeOfBackupData( );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateRequestSizeOfBackupData( MMMCScBkupDriver& aDriver );

        /**
        * Second phase constructor
        */
        void ConstructL( );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;
        TMMCScBkupStateId NextStateId() const;
        TStateExecution CategorySpecific() const { return EStatePerCategory; }

    private: // From CMMCScBkupState
        void PerformStateInitL();
        void PerformAsynchronousStateStepL();
        TBool PerformAsynchronousErrorCleanup( TInt aError );
        void PerformAsynchronousCancellation();

    private: // Data members

        //
        TInt iCurrentDataOwnerIndex;
        //
        CMMCScBkupStateRequestSizeOfDataOwner* iRequestObject;
    };







/**
* NB. This class only sizes active, passive, system and java data. Public
* data is sized elsewhere.
*
* @since 3.0
*/
class CMMCScBkupStateRequestSizeOfDataOwner : public CMMCScBkupDriveSpecificRequest
    {
    public:

        /**
        *
        */
        static CMMCScBkupStateRequestSizeOfDataOwner* NewL( MMMCScBkupDriver& aDriver );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateRequestSizeOfDataOwner( MMMCScBkupDriver& aDriver );

    public: // API

        /**
        *
        */
        void RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver );

    private: // From CActive
        void RunL();
        void DoCancel();

    private: // Internal methods
        inline MMMCScBkupDriver& Driver() { return iDriver; }
        //
        TUint GetActiveAndPassiveDataSizesL( const CDataOwnerInfo& aOwner, CSBGenericDataType& aIdentifier, TDriveNumber aDrive );
        TUint GetSIDSpecificSizeL( TSecureId aSecureID, TDriveNumber aDrive, TTransferDataType aDataType );
        TUint GetSystemDataSizesL( const CDataOwnerInfo& aOwner, CSBGenericDataType& aIdentifier, TDriveNumber aDrive );
        TUint GetJavaDataSizesL( const CDataOwnerInfo& aOwner, CSBGenericDataType& aIdentifier, TDriveNumber aDrive );
        TUint GetJavaSpecificSizeL( const TDesC& aHash, TDriveNumber aDrive, TJavaTransferType aDataType );
    
    private: // Member data

        //
        MMMCScBkupDriver& iDriver;
        //
        CMMCScBkupDataOwnerInfo* iOwner;
    };





#endif // __CMMCSCBKUPSTATEREQUESTSIZEOFBACKUPDATA_H__

//End of File
