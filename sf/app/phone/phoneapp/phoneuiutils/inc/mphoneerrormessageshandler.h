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
* Description:  Show error notes.
*
*/


#ifndef __MPHONEERRORMESSAGESHANDLER_H
#define __MPHONEERRORMESSAGESHANDLER_H

// INCLUDES
#include <pevirtualengine.h>

// CLASS DECLARATION
class TPEErrorInfo;

class MPhoneErrorMessagesHandler
    {
    public:

        /**
        * Destructor.
        */
        virtual ~MPhoneErrorMessagesHandler() {};

        virtual void ShowErrorSpecificNoteL( const TPEErrorInfo& aErrorInfo ) = 0;
    };

#endif // __MPHONEERRORMESSAGESHANDLER_H

// End of File