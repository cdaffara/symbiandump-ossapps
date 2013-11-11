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
* Description: Declaration for CMMCScBkupStateGetDataOwnerStatuses
*     
*
*/

#ifndef __CMMCSCBKUPSTATEGETDATAOWNERSTATUSES_H__
#define __CMMCSCBKUPSTATEGETDATAOWNERSTATUSES_H__

// User includes
#include "CMMCScBkupState.h"



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateGetDataOwnerStatuses) : public CMMCScBkupState
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateGetDataOwnerStatuses* NewL( MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupStateGetDataOwnerStatuses( );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateGetDataOwnerStatuses( MMMCScBkupDriver& aDriver );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;
        TMMCScBkupStateId NextStateId() const;
        TStateExecution CategorySpecific() const { return EStatePerCategory; }

    private: // From CMMCScBkupState
        void PerformStateInitL();
        void PerformAsynchronousStateStepL();

    private: // Internal state enumeration

        /**
        *
        */
        enum TState
            {
            EPrearingQuery = 0,
            EGettingStatus,
            EProcessingResults
            };

    private: // Internal methods
        inline TState State() const { return iState; }
        inline void SetState(TState aState) { iState = aState; }
        //
        void PrepareQueryL();
        void ProcessStatusResultsL();

    private: // Member data

        //
        RSIDStatusArray iStatusArray;
        //
        TState iState;
    };




#endif // __CMMCSCBKUPSTATEGETDATAOWNERSTATUSES_H__

//End of File
