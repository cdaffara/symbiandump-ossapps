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
* Description: Declaration for CMMCScBkupStateArchiveOpPassiveData
*     
*
*/

#ifndef __CMMCSCBKUPSTATEARCHIVEOPPASSIVEDATA_H__
#define __CMMCSCBKUPSTATEARCHIVEOPPASSIVEDATA_H__

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
NONSHARABLE_CLASS(CMMCScBkupStateArchiveOpPassiveData) : public CMMCScBkupStateOpAware, public MMMCScBkupIndexHandler
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateArchiveOpPassiveData* NewL( MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupStateArchiveOpPassiveData( );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateArchiveOpPassiveData( MMMCScBkupDriver& aDriver );

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

    private: // Member data

        //
        TInt iIndexValueCurrent;
        //
        CMMCScBkupWriteDataTransferRequest< TTransferDataType >* iBackupTransferObject;
        //
        CMMCScBkupReadDataTransferRequest< TTransferDataType >* iRestoreTransferObject;
    };




#endif // __CMMCSCBKUPSTATEARCHIVEOPPASSIVEDATA_H__

//End of File
