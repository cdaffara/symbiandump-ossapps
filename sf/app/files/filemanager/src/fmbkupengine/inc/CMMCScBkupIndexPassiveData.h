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
* Description: Declaration for CMMCScBkupIndexPassiveData
*     
*
*/

#ifndef __CMMCSCBKUPINDEXPASSIVEDATA_H__
#define __CMMCSCBKUPINDEXPASSIVEDATA_H__

// System includes
#include <f32file.h>

// User includes
#include "CMMCScBkupIndexWithIdentifier.h"

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupIndexPassiveData) : public CMMCScBkupIndexWithIdentifier< TSecureId >
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupIndexPassiveData* NewLC();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupIndexPassiveData();

    };




#endif // __CMMCScBkupIndexPassiveData_H__

//End of File
