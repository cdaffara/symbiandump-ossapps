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
* Description: 
*    API for register error messages handler.
*
*/


#ifndef __MPHONEERRORMESSAGESHANDLERREGISTER_H
#define __MPHONEERRORMESSAGESHANDLERREGISTER_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  API for register resource resolvers.
*/
class MPhoneErrorMessagesHandlerRegister
    {
    public:     
        
        /**
        * Destructor
        */
        virtual ~MPhoneErrorMessagesHandlerRegister(){};

        /**
        * R 
        * @return None.
        */
        virtual void RegisterErrorMessagesHandler( MPhoneErrorMessagesHandler* aResolver ) = 0;

    };
#endif      // __MPHONEERRORMESSAGESHANDLERREGISTER_H
            
// End of File
