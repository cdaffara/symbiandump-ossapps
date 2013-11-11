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
* Description: Declaration for CMMCScBkupStateArchiveOpPublicDataFiles
*     
*
*/

#ifndef __CMMCSCBKUPSTATEARCHIVEOPPUBLICDATAFILES_H__
#define __CMMCSCBKUPSTATEARCHIVEOPPUBLICDATAFILES_H__

// System includes
#include <connect/sbtypes.h>

// User includes
#include "CMMCScBkupStateOpAware.h"

// Namespaces
using namespace conn;

// Classes referenced
class CMMCScBkupDataOwnerInfo;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateArchiveOpPublicDataFiles) : public CMMCScBkupStateOpAware
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateArchiveOpPublicDataFiles* NewL( MMMCScBkupDriver& aDriver );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateArchiveOpPublicDataFiles( MMMCScBkupDriver& aDriver );

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
        void PerformAsynchronousCancellation();

    private: // Internal enumerations
        
        /**
        *
        */
        enum TState
            {
            EStateCommonIdle = 0,
            EStateCommonKickOff,
            EStateCommonProcessingFile,
            EStateBackupAddIndexRecord,
            EStateRestoreResetAttributes
            };

    private: // Internal methods
        
        /**
        *
        */
        void CompressingAndWritingFileL();
        
        /**
        *
        */
        void DecompressingAndWritingFileL();
       
        /**
        *
        */
        void AddIndexRecordL();
        
        /**
        *
        */
        void RestoreFileAttributesL();

        /**
        *
        */
        inline void SetState(TState aState) { iState = aState; }

        /**
        *
        */
        inline TState State() const { return iState; }

    private: // Member data

        //
        TInt iIndexValueCurrent;
        //
        TState iState;
    };


#endif // __CMMCSCBKUPSTATEARCHIVEOPPUBLICDATAFILES_H__

//End of File
