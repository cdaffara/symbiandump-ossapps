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
* Description: Declaration for CMMCScBkupStateNotifyAllSnapshotsSupplied
*     
*
*/

#ifndef __CMMCSCBKUPSTATENOTIFYALLSNAPSHOTSSUPPLIED_H__
#define __CMMCSCBKUPSTATENOTIFYALLSNAPSHOTSSUPPLIED_H__

// User includes
#include "MMCScBkupOperations.h"
#include "CMMCScBkupState.h"

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateNotifyAllSnapshotsSupplied) : public CMMCScBkupState
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateNotifyAllSnapshotsSupplied* NewL( MMMCScBkupDriver& aDriver );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateNotifyAllSnapshotsSupplied( MMMCScBkupDriver& aDriver );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;
        TMMCScBkupStateId NextStateId() const;

    private: // From CMMCScBkupState
        void PerformStateInitL();
    };




#endif // __CMMCSCBKUPSTATENOTIFYALLSNAPSHOTSSUPPLIED_H__

//End of File
