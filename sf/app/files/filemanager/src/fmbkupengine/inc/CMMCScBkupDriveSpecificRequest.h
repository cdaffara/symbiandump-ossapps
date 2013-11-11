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
* Description: Declaration of CMMCScBkupDriveSpecificRequest
*     
*
*/

#ifndef __CMMCSCBKUPDRIVESPECIFICREQUEST_H__
#define __CMMCSCBKUPDRIVESPECIFICREQUEST_H__

// System includes
#include <e32base.h>
#include <f32file.h>

// User includes
#include "TMMCScBkupOwnerDataType.h"

// Classes referenced
class CMMCScBkupDriveAndOperationTypeManager;



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDriveSpecificRequest) : public CActive
    {
    protected:

        /**
        * C++ default constructor
        */
        CMMCScBkupDriveSpecificRequest( const CMMCScBkupDriveAndOperationTypeManager& aDriveAndOperations, TMMCScBkupOwnerDataType aDataType, CActive::TPriority aPriority = CActive::EPriorityIdle );

        /**
        * Second phase constructor
        */
        void ConstructL( );

    public:

        /**
        * C++ destructor
        */
        ~CMMCScBkupDriveSpecificRequest();

    protected: // API

        /**
        *
        */
        virtual void RequestL( TRequestStatus& aObserver );

    protected: // Internal methods

        /**
        *
        */
        TBool NextValidDrive(TDriveNumber& aDrive);

        /**
        *
        */
        TBool NextValidDrive(TDriveNumber& aDrive, const TDriveList& aCrossCheckList);

        /**
        * Return the current drive
        */
        TDriveNumber CurrentDrive() const;

        /**
        *
        */
        void CompleteObserverRequest(TInt aCompletionCode);

        /**
        *
        */
        void CompleteSelf(TInt aCompletionCode = KErrNone);

        /**
        *
        */
        inline const CMMCScBkupDriveAndOperationTypeManager& DriveAndOperations() const { return iDriveAndOperations; }

    private: // From CActive

        /**
        * Default behaviour is to do nothing
        */
        void DoCancel();

        /**
        * Complete's observer with the error code
        */
        TInt RunError(TInt aError);

    private: // Member data

        //
        const CMMCScBkupDriveAndOperationTypeManager& iDriveAndOperations;
        //
        TMMCScBkupOwnerDataType iDataType;
        //
        TInt iCurrentDrive;
        //
        TRequestStatus* iObserver;
    };








#endif // __CMMCSCBKUPARCHIVE_H__

//End of File
