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
* Description: Declaration for MMCScBkupPhoneModelUtils
*     
*
*/

#ifndef __MMCSCBKUPPHONEMODELUTILS_H__
#define __MMCSCBKUPPHONEMODELUTILS_H__

// System includes
#include <f32file.h>
#include <babitflags.h>

/**
*
*
* @since 3.0
*/
class MMCScBkupPhoneModelUtils
    {
    public:

        /**
        *
        */
        static HBufC8* CurrentPhoneModelLC();

        /**
        *
        */
        static TInt MaximumPhoneModelIdentifierLength();

        /**
        *
        */
        static TBool ArchiveRestorePermissableL( const TDesC8& aPhoneModelData, TBitFlags aArchiveFlags, const TVersion& aArchiveVersion );
    };




#endif // __MMCScBkupPhoneModelUtils_H__

//End of File
