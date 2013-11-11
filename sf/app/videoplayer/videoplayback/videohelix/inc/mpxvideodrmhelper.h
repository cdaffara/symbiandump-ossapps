/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This class handles the DRM functions for the playback plugin
*
*/


// Version : %version: 6 %


#ifndef __MPXVIDEODRMHELPER_H__
#define __MPXVIDEODRMHELPER_H__


// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <drmutility.h>
#include <drmutilitytypes.h>

// CLASS DECLARATION

/*
 *  CMpxVideoDrmHelper
 *
 */
class CMpxVideoDrmHelper : public CBase
{
    public:
        // Constructors and destructor

        /*
         *  Destructor.
         */
        ~CMpxVideoDrmHelper();

        /*
         *  Two-phased constructor.
         */
        static CMpxVideoDrmHelper* NewL();

        TBool IsProtectedL( RFile& aFile );

        TInt GetDrmRightsStatus( RFile& aFile );

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        TBool IsProtected64L( RFile64& aFile );
        TInt GetDrmRightsStatus64( RFile64& aFile );

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    private:

        CMpxVideoDrmHelper();

        void ConstructL();

    private:

        DRM::CDrmUtility* iDrmUtility;
};

#endif // __MPXVIDEODRMHELPER_H__
