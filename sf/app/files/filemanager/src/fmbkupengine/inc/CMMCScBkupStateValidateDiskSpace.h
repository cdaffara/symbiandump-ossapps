/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration for CMMCScBkupStateValidateDiskSpace
*     
*
*/

#ifndef __CMMCSCBKUPSTATEVALIDATEDISKSPACE_H__
#define __CMMCSCBKUPSTATEVALIDATEDISKSPACE_H__

// User includes
#include "CMMCScBkupState.h"


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateValidateDiskSpace) : public CMMCScBkupState
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateValidateDiskSpace* NewL( MMMCScBkupDriver& aDriver );

        /**
        * Destructor
        */
        ~CMMCScBkupStateValidateDiskSpace();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateValidateDiskSpace( MMMCScBkupDriver& aDriver );

        //   Second phase
        void ConstructL( );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;
        TMMCScBkupStateId NextStateId() const;
        TStateExecution CategorySpecific() const { return EStatePerCategoryCommon; }

    private: // From CMMCScBkupState
        void PerformStateInitL();
        void PerformAsynchronousStateStepL();
        
    private: // Internal methods
        
        /**
        *
        */
        void ValidateFreeSpaceBeforeBackupL();
        void ValidateFreeSpaceBeforeRestoreL();
        
    private: // Member data

        //
        TInt64 iCumulatedSize;
        //
        RArray<TMMCScBkupDriveAndSize> iDriveSizes;
        //
        RArray<TMMCScBkupDriveAndSize> iDriveMaxFileSizes;
    };


#endif // __CMMCSCBKUPSTATEVALIDATEDISKSPACE_H__

//End of File
