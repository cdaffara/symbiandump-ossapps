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


#ifndef IMGPPRINTAPPUTIL_H
#define IMGPPRINTAPPUTIL_H

#include <e32std.h>

/**
 *
 *  Utility class that contains small static helper methods to group
 *  the common functionality
 *
 */
class IMGPPrintAppUtil
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
        static void ShowErrorMsgL( TInt aErrCode, TInt aErrorStringCode = 0 );

        /**
         *  Loads error message specifically created for printer application
         *
         *  @param aErrCode Error code
         */
        static HBufC* PrintErrorMsgLC( TInt aErrCode, TInt aErrorStringCode = 0 );     
          
        /**
         *  Returns filename with the application path
         *
         *  @param aFileName  The filename
         *  @return filename added to the application path
         */
        static TFileName AddApplicationPath( const TDesC& aFileName );
        
    private:
    	static void HandleByStringCodeL( TInt aErrCode, TInt aErrorStringCode, TInt& aResourceId );
    	static void StringCodeInkL( TInt aErrorStringCode, TInt& aResourceId, TBool& aMatch  );
    	static void StringCodeHwL( TInt aErrorStringCode, TInt& aResourceId, TBool& aMatch  );    	
    	static void StringCodePaperL( TInt aErrorStringCode, TInt& aResourceId, TBool& aMatch  );
    	
    	static void HandleByErrorCodeL( TInt& aResourceId, TInt& aFinalErrCode, TInt aErrCode );    	
    	static void ErrorCodeInkL( TInt& aResourceId, TInt aErrCode, TBool& aMatch );
    	static void ErrorCodeHwL( TInt& aResourceId, TInt aErrCode, TBool& aMatch );
    	static void ErrorCodePaperL( TInt& aResourceId, TInt aErrCode, TBool& aMatch );
    	
    };

#endif  //  IMGPPRINTAPPUTIL_H

//  End of File
