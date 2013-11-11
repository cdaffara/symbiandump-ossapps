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
* Description: Declaration for CMMCScBkupIndexSystemData
*     
*
*/

#ifndef __CMMCSCBKUPINDEXSYSTEMDATA_H__
#define __CMMCSCBKUPINDEXSYSTEMDATA_H__

// System includes
#include <f32file.h>

// User includes
#include "CMMCScBkupIndexWithIdentifier.h"

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupIndexSystemData) : public CMMCScBkupIndexWithIdentifier< TInt32 >
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupIndexSystemData* NewLC();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupIndexSystemData();

    };





#endif // __CMMCSCBKUPINDEXSYSTEMDATA_H__

//End of File
