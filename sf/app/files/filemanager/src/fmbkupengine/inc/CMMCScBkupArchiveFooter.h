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
* Description: Declaration of CMMCScBkupArchiveFooter
*     
*
*/

#ifndef __CMMCSCBKUPARCHIVEFOOTER_H__
#define __CMMCSCBKUPARCHIVEFOOTER_H__

// System includes
#include <e32base.h>

// User includes
#include "CMMCScBkupIndexBase.h"
#include "RMMCScBkupPointerArray.h"
#include "TMMCScBkupDriveAndSize.h"

// Classes referenced
class MMMCScBkupDriver;
class MMMCScBkupArchiveDataInterface;
class CMMCScBkupIndexRegistrationData;



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupArchiveFooter) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupArchiveFooter* NewL( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupArchiveFooter();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupArchiveFooter( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver );

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        CMMCScBkupIndexBase& IndexByType( TMMCScBkupOwnerDataType aType );

        /**
        *
        */
        CMMCScBkupIndexBase* IndexByTypeOrNull( TMMCScBkupOwnerDataType aType );

    public: // Store/Restore

        /**
        *
        */
        void StoreL();

        /**
        *
        */
        void RestoreL( TInt aCalculatedFooterOffsetWithinArchive );

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Member data

        //
        MMMCScBkupArchiveDataInterface& iDataInterface;
        //
        MMMCScBkupDriver& iDriver;
        //
        RMMCScBkupPointerArray< CMMCScBkupIndexBase > iIndicies;
    };




#endif // __CMMCSCBKUPARCHIVEFOOTER_H__

//End of File
