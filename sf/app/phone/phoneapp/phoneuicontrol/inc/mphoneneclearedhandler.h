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
* Description:  Interface to response for Number Entry clearing.
*
*/


#ifndef MPHONENECLEAREDHANDLER_H
#define MPHONENECLEAREDHANDLER_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Handle NE cleared indication
*/
class MPhoneNEClearedHandler
    {
    public:
        
        /**
        * To response to Number Entry cleared event.
        */
        virtual void HandleNumberEntryClearedL() = 0;            
    };

#endif      // MPHONENECLEAREDHANDLER_H   
            
// End of File
