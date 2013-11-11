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
* Description: Declaration for CMMCScBkupStateArchiveOpDataOwners
*     
*
*/

#ifndef __CMMCSCBKUPSTATEARCHIVEOPDATAOWNERS_H__
#define __CMMCSCBKUPSTATEARCHIVEOPDATAOWNERS_H__

// User includes
#include "CMMCScBkupStateOpAware.h"


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateArchiveOpDataOwners) : public CMMCScBkupStateOpAware
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateArchiveOpDataOwners* NewL( MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupStateArchiveOpDataOwners();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateArchiveOpDataOwners( MMMCScBkupDriver& aDriver );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;

    public: // From CMMCScBkupStateOpAware
        TMMCScBkupStateId NextStateBackupId( TBool aPartial ) const;
        TMMCScBkupStateId NextStateRestoreId( TBool aPartial ) const;

    private: // From CMMCScBkupStateOpAware

    private: // From CMMCScBkupState
        void PerformStateInitBackupL( TBool aPartial );
        void PerformStateInitRestoreL( TBool aPartial );
        //
        void PerformAsynchronousStateStepBackupL( TBool aPartial );
        void PerformAsynchronousStateStepRestoreL( TBool aPartial );
        //
        void PerformLastRightsBackupL( TBool aPartial );
        void PerformLastRightsRestoreL( TBool aPartial );
        //
        TBool PerformAsynchronousErrorCleanupRestore( TBool aPartial, TInt aError );

    private: // Member data

        //
        TInt iIndexValueCurrent;
    };


#endif // __CMMCSCBKUPSTATEARCHIVEOPDATAOWNERS_H__

//End of File
