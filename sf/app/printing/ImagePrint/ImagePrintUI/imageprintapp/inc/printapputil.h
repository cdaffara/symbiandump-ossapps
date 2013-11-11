/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef PRINTAPPUTIL_H
#define PRINTAPPUTIL_H

#include <e32std.h>

/**
 *
 *  Utility class that contains small static helper methods to group
 *  the common functionality
 *
 */
class PrintAppUtil
    {
    public:

        /**
         *  Displays error note using given resource id as the text
         *
         *  @param aResourceId  Resource ID for the given error text
         */
        static void ShowErrorNoteL( TInt aResourceId );            

        /**
         *  Displays information note using given resource id as the text
         *
         *  @param aResourceId  Resource ID for the given error text
         */
        static void ShowInfoNoteL( TInt aResourceId ); 
        
        /**
         *  Displays error note using given error code as the source
         *  for the text
         *
         *  @param aErrCode Error code
         */
        static void ShowErrorMsgL( TInt aErrCode );

        /**
         *  Loads error message specifically created for printer application
         *
         *  @param aErrCode Error code
         *
         *  @return the error message string for given error code
         */
        static HBufC* PrintErrorMsgLC( TInt aErrCode );     
        
        /**
         *  Returns filename with the application path
         *
         *  @param aFileName  The filename
         *
         *  @return filename added to the application path
         */
        static TFileName AddApplicationPath( const TDesC& aFileName );
    };


#endif  //  PRINTAPPUTIL_H

//  End of File
