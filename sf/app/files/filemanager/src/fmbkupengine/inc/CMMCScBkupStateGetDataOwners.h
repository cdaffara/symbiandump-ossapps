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
* Description: Declaration for CMMCScBkupStateGetDataOwners
*     
*
*/

#ifndef __CMMCSCBKUPSTATEGETDATAOWNERS_H__
#define __CMMCSCBKUPSTATEGETDATAOWNERS_H__

// User includes
#include "CMMCScBkupState.h"



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateGetDataOwners) : public CMMCScBkupState
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateGetDataOwners* NewL( MMMCScBkupDriver& aDriver );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateGetDataOwners( MMMCScBkupDriver& aDriver );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;
        TMMCScBkupStateId NextStateId() const;

    private: // From CMMCScBkupState
        void PerformStateInitL();
        void PerformAsynchronousStateStepL();
    };




#endif // __CMMCSCBKUPSTATEGETDATAOWNERS_H__

//End of File
