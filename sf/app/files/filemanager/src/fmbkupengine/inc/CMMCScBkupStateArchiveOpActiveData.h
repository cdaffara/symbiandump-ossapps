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
* Description: Declaration for CMMCScBkupStateArchiveOpActiveData
*     
*
*/

#ifndef __CMMCSCBKUPSTATEARCHIVEOPACTIVEDATA_H__
#define __CMMCSCBKUPSTATEARCHIVEOPACTIVEDATA_H__

// System includes
#include <connect/sbtypes.h>

// User includes
#include "CMMCScBkupStateOpAware.h"
#include "TMMCScBkupArchiveVector.h"
#include "CMMCScBkupTransferWriteRequest.h"
#include "CMMCScBkupTransferReadRequest.h"

// Namespaces
using namespace conn;

// Classes referenced
class CMMCScBkupDataOwnerInfo;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateArchiveOpActiveData) : public CMMCScBkupStateOpAware, public MMMCScBkupIndexHandler
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateArchiveOpActiveData* NewL( MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupStateArchiveOpActiveData( );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateArchiveOpActiveData( MMMCScBkupDriver& aDriver );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;

    public: // From CMMCScBkupStateOpAware
        TMMCScBkupStateId NextStateBackupId( TBool aPartial ) const;
        TMMCScBkupStateId NextStateRestoreId( TBool aPartial ) const;

    private: // From CMMCScBkupStateOpAware
        void PerformStateInitBackupL( TBool aPartial );
        void PerformStateInitRestoreL( TBool aPartial );
        void PerformAsynchronousStateStepBackupL( TBool aPartial );
        void PerformAsynchronousStateStepRestoreL( TBool aPartial );
        void PerformLastRightsBackupL( TBool aPartial );
        TBool PerformAsynchronousErrorCleanupBackup( TBool aPartial, TInt aError );
        TBool PerformAsynchronousErrorCleanupRestore( TBool aPartial, TInt aError );

    private: // From CMMCScBkupState
        void PerformAsynchronousCancellation();

    private: // From MMMCScBkupIndexHandler
        void AddIndexRecordL( CMMCScBkupArchiveFooter& aFooter, CMMCScBkupDataOwnerInfo& aDataOwner, const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive );

    private: // Internal members

        /**
        *
        */
        inline TBool AllDataOwnersHandled() const { return !iAtLeastOneDataOwnerIsNotYetReady; }

        /**
        *
        */
        void CheckNeedToStartRetryTimerL();

    private: // Member data

        //
        TInt iIndexValueCurrent;
        //
        TBool iAtLeastOneDataOwnerIsNotYetReady;
        //
        RTimer iTimer;
        //
        CMMCScBkupWriteDataTransferRequest< TTransferDataType >* iBackupTransferObject;
        //
        CMMCScBkupReadDataTransferRequest< TTransferDataType >* iRestoreTransferObject;
    };








#endif // __CMMCSCBKUPSTATEARCHIVEOPACTIVEDATA_H__

//End of File
