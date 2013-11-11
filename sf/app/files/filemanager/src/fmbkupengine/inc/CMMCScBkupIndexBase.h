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
* Description: Declaration for CMMCScBkupIndexBase
*     
*
*/

#ifndef __CMMCSCBKUPINDEXBASE_H__
#define __CMMCSCBKUPINDEXBASE_H__

// System includes
#include <e32base.h>

// User includes
#include "MMMCScBkupDriver.h"
#include "TMMCScBkupOwnerDataType.h"
#include "RMMCScBkupPointerArray.h"
#include "TMMCScBkupArchiveVector.h"


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupIndexBase) : public CBase
    {
    public:

        /**
        * C++ destructor
        */
        ~CMMCScBkupIndexBase();

    protected:

        /**
        * C++ default constructor
        */
        CMMCScBkupIndexBase(TMMCScBkupOwnerDataType aType);

        /**
        *
        */
        virtual void ConstructL();

    public: // API

        /**
        *
        */
        inline TMMCScBkupOwnerDataType Type() const { return iType; }

        /**
        *
        */
        inline const TMMCScBkupArchiveVector& Vector() const { return iVector; }

        /**
        *
        */
        inline void SetVector( const TMMCScBkupArchiveVector& aVector ) { iVector = aVector; }

    public: // Store/Restore

        /**
        *
        */
        virtual void StoreL(MMMCScBkupDriver& aDriver) = 0;

        /**
        *
        */
        virtual void RestoreL(MMMCScBkupDriver& aDriver) = 0;

    private: // Member data

        //
        TMMCScBkupOwnerDataType iType;
        //
        TMMCScBkupArchiveVector iVector;
    };




#endif // __CMMCSCBKUPINDEXBASE_H__

//End of File
