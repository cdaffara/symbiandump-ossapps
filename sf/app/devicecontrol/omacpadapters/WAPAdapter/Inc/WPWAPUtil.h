/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Utility class for reading resource strings.
*
*/


#ifndef WPWAPUTIL_H
#define WPWAPUTIL_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RResourceFile;

// CLASS DECLARATION

/**
 * WPWAPUtil is a utility class for reading resource strings.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class WPWAPUtil
    {
    public: // New functions
        /**
        * Loads a specified resource string from resource file.
        * @param aResourceId is of a resource string.
        * @return pointer to loaded descriptor.
        */
        static HBufC* ReadHBufCL( TInt aResourceId );

        /**
        * Searches for the resource file with the correct language extension 
        * for the language of the current locale, or failing this, the best 
        * matching file.
        * @param aFs reference to file server.
        * @param aResFileName resource file name.
        * @param aResFile reference to resource file.
        */
        static void FindAndOpenResourceFileLC(RFs& aFs, 
            const TDesC& aResFileName, 
            RResourceFile& aResFile);

        /**
        * Checks if a URL is valid.
        * @param aURL the URL to check
        * @return ETrue if the URL is valid
        */
        static TBool CheckURI( const TDesC& aUrl );

    };

#endif  // WPWAPUTIL_H
            
// End of File
