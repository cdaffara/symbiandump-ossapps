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
* Description: Declaration for CMMCScBkupStateArchiveOpJavaData
*     
*
*/

#ifndef __CMMCSCBKUPSTATEARCHIVEOPJAVADATA_H__
#define __CMMCSCBKUPSTATEARCHIVEOPJAVADATA_H__

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
NONSHARABLE_CLASS(CMMCScBkupStateArchiveOpJavaData) : public CMMCScBkupStateOpAware, public MMMCScBkupIndexHandler
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateArchiveOpJavaData* NewL( MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupStateArchiveOpJavaData( );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateArchiveOpJavaData( MMMCScBkupDriver& aDriver );

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
        void PerformLastRightsRestoreL( TBool aPartial );
        TBool PerformAsynchronousErrorCleanupBackup( TBool aPartial, TInt aError );
        TBool PerformAsynchronousErrorCleanupRestore( TBool aPartial, TInt aError );

    private: // From CMMCScBkupState
        void PerformAsynchronousCancellation();

    private: // From MMMCScBkupIndexHandler
        void AddIndexRecordL( CMMCScBkupArchiveFooter& aFooter, CMMCScBkupDataOwnerInfo& aDataOwner, const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive );

    private: // Internal enumerations
        enum TJavaState
            {
            EJavaStateIdle = 0,
            EJavaStateMidlet,
            EJavaStateMidletData
            };

    private: // Internal methods
        void BackupMidletL();
        void BackupMidletDataL();
        void RestoreMidletL();
        void RestoreMidletDataL();

    private: // Member data

        //
        TInt iIndexValueCurrent;
        //
        TJavaState iCurrentJavaState;
        //
        RArray<TMMCScBkupArchiveDriveAndVector> iRestoreEntries;
        //
        CMMCScBkupWriteDataTransferRequest< TJavaTransferType >* iBackupTransferObjectMidlet;
        CMMCScBkupWriteDataTransferRequest< TJavaTransferType >* iBackupTransferObjectMidletData;
        CMMCScBkupReadDataTransferRequest< TJavaTransferType >* iRestoreTransferObjectMidlet;
        CMMCScBkupReadDataTransferRequest< TJavaTransferType >* iRestoreTransferObjectMidletData;
    };





#endif // __CMMCSCBKUPSTATEARCHIVEOPJAVADATA_H__

//End of File
