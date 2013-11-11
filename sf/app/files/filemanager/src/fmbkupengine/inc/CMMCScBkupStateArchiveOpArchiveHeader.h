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
* Description: Declaration for CMMCScBkupStateArchiveOpArchiveHeader
*     
*
*/

#ifndef __CMMCSCBKUPSTATEARCHIVEOPARCHIVEHEADER_H__
#define __CMMCSCBKUPSTATEARCHIVEOPARCHIVEHEADER_H__

// User includes
#include "CMMCScBkupState.h"


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateArchiveOpArchiveHeader) : public CMMCScBkupState
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupStateArchiveOpArchiveHeader* NewL( MMMCScBkupDriver& aDriver );

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupStateArchiveOpArchiveHeader( MMMCScBkupDriver& aDriver );

    public: // From CMMCScBkupState
        TMMCScBkupStateId StateId() const;
        TMMCScBkupStateId NextStateId() const;
        TStateExecution CategorySpecific() const { return EStatePerCategory; }

    private: // From CMMCScBkupState
        void PerformStateInitL();
        void PerformAsynchronousStateStepL();
    };


#endif // __CMMCSCBKUPSTATEARCHIVEOPARCHIVEHEADER_H__

//End of File
