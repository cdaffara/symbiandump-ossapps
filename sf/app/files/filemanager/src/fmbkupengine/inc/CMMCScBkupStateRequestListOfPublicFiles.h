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
* Description: Declaration for CMMCScBkupStateRequestListOfPublicFiles
*     
*
*/

#ifndef __CMMCSCBKUPSTATEREQUESTLISTOFPUBLICFILES_H__
#define __CMMCSCBKUPSTATEREQUESTLISTOFPUBLICFILES_H__

// User includes
#include "CMMCScBkupState.h"
#include "CMMCScBkupDriveSpecificRequest.h"

// Namespaces
using namespace conn;

// Classes referenced
class CMMCScBkupDataOwnerInfo;
class CMMCScBkupStateRequestSpecificPublicFileInfo;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateRequestListOfPublicFiles) : public CMMCScBkupState
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateRequestListOfPublicFiles* NewL( MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupStateRequestListOfPublicFiles( );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateRequestListOfPublicFiles( MMMCScBkupDriver& aDriver );

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
        TBool PerformAsynchronousErrorCleanup(TInt aError);
        void PerformAsynchronousCancellation();
        void PerformLastRightsL();

    private: // Data members

        //
        TInt iCurrentDataOwnerIndex;
        //
        CMMCScBkupStateRequestSpecificPublicFileInfo* iRequestObject;
    };







/**
*
*
* @since 3.0
*/
class CMMCScBkupStateRequestSpecificPublicFileInfo : public CMMCScBkupDriveSpecificRequest
    {
    public:

        /**
        *
        */
        static CMMCScBkupStateRequestSpecificPublicFileInfo* NewL( MMMCScBkupDriver& aDriver );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateRequestSpecificPublicFileInfo( MMMCScBkupDriver& aDriver );

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

    private: // Member data

        //
        MMMCScBkupDriver& iDriver;
        //
        CMMCScBkupDataOwnerInfo* iOwner;
    };







#endif // __CMMCSCBKUPSTATEREQUESTLISTOFPUBLICFILES_H__

//End of File
