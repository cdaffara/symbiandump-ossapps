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
* Description:  Declaration for RMMCScBkupProgressSizer
*     
*
*/

#ifndef __RMMCSCBKUPPROGRESSSIZER_H__
#define __RMMCSCBKUPPROGRESSSIZER_H__

// System includes
#include <f32file.h>

// User includes
#include "TMMCScBkupDriveFilter.h"
#include "TMMCScBkupOwnerDataType.h"

// Classes referenced
class CMMCScBkupDataOwnerInfo;
class MMMCScBkupProgressObserver;
class CMMCScBkupDriveAndOperationTypeManager;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(RMMCScBkupProgressSizer)
    {
    public:

        /**
        * C++ default constructor
        */
        RMMCScBkupProgressSizer( const CMMCScBkupDriveAndOperationTypeManager& aDriveAndOperationTypes );

    public: // BACKUP SIZING API

        /**
        *
        */
        TInt64 BackupTotalProgressValueL( const CMMCScBkupDataOwnerInfo& aDataOwner );

        /**
        *
        */
        void BackupReportFixedProgressForOpL( MMMCScBkupProgressObserver& aProgressManager, TMMCScBkupOwnerDataType aType );

    public: // RESTORE SIZING API

        /**
        *
        */
        TInt64 RestoreCombinedDataSizeL( const CMMCScBkupDataOwnerInfo& aOwner );

    private: // Internal methods

        /**
        *
        */
        TInt NumberOfDriveOpsRequiredL( const CMMCScBkupDataOwnerInfo& aOwner, TMMCScBkupOwnerDataType aType );

        /**
        *
        */
        TInt64 AmountOfPublicDataToBeRestoredL( const CMMCScBkupDataOwnerInfo& aOwner );

    private: // Member data

        //
        const CMMCScBkupDriveAndOperationTypeManager& iDriveAndOperationTypes;
        //
        TMMCScBkupDriveFilter iDriveFilter;

    };




#endif // __RMMCSCBKUPPROGRESSSIZER_H__

//End of File
