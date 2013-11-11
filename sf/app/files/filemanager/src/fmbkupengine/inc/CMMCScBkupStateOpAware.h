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
* Description: Declaration for CMMCScBkupStateOpAware
*     
*
*/

#ifndef __CMMCSCBKUPSTATEOPAWARE_H__
#define __CMMCSCBKUPSTATEOPAWARE_H__

// User includes
#include "CMMCScBkupState.h"
#include "MMCScBkupOperations.h"

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateOpAware) : public CMMCScBkupState
    {
    protected:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateOpAware( MMMCScBkupDriver& aDriver, TInt aPriority = CActive::EPriorityIdle );

    public: // From CMMCScBkupState

        /**
        *
        */
        TMMCScBkupStateId NextStateId() const;

        /**
        *
        */
        TStateExecution CategorySpecific() const { return EStatePerCategory; }

    protected: // New Framework

        /**
        *
        */
        virtual TMMCScBkupStateId NextStateBackupId( TBool aPartial ) const = 0;

        /**
        *
        */
        virtual TMMCScBkupStateId NextStateRestoreId( TBool aPartial ) const = 0;

    protected: // New Framework

        /**
        *
        */
        virtual void PerformStateInitBackupL( TBool aPartial ) = 0;

        /**
        *
        */
        virtual void PerformStateInitRestoreL( TBool aPartial ) = 0;

        /**
        *
        */
        virtual void PerformAsynchronousStateStepBackupL( TBool aPartial );

        /**
        *
        */
        virtual void PerformAsynchronousStateStepRestoreL( TBool aPartial );

        /**
        *
        */
        virtual void PerformAsynchronousCancellationBackup( TBool aPartial );

        /**
        *
        */
        virtual void PerformAsynchronousCancellationRestore( TBool aPartial );

        /**
        *
        */
        virtual void PerformLastRightsBackupL( TBool aPartial );

        /**
        *
        */
        virtual void PerformLastRightsRestoreL( TBool aPartial );

        /**
        *
        */
        virtual TBool PerformAsynchronousErrorCleanupBackup( TBool aPartial, TInt aError );

        /**
        *
        */
        virtual TBool PerformAsynchronousErrorCleanupRestore( TBool aPartial, TInt aError );

    protected: // From CMMCScBkupState
        void PerformStateInitL();
        void PerformAsynchronousStateStepL();
        void PerformAsynchronousCancellation();
        void PerformLastRightsL();
        TBool PerformAsynchronousErrorCleanup( TInt aError );
    };





#endif // __CMMCSCBKUPSTATEOPAWARE_H__

//End of File
