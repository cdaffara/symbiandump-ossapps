/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     All possible menu and CBA events.
*
*/


#ifndef MPHONEMENUANDCBAEVENTS_H
#define MPHONEMENUANDCBAEVENTS_H

//  INCLUDES
#include <aknappui.h>

// CLASS DECLARATION

/**
*  All possible menu and CBA events
*/
class MPhoneMenuAndCbaEvents
    {

    public:

        /**
        * Destructor
        */
        virtual ~MPhoneMenuAndCbaEvents(){};

        /**
        * From CAknAppUi, handles commands.
        * @param aCommand It is the code of the command to be handled.
        * @returns boolean value was the command handled by the state 
        * (ETrue) or not (EFalse)
        */
        virtual TBool HandleCommandL( TInt aCommand ) = 0;
        
        /**
        * Process command.
        * @param aCommand It is the code of the command to be handled.
        * @returns boolean value was the command handled by the state 
        * (ETrue) or not (EFalse)
        */
        virtual TBool ProcessCommandL( TInt aCommand ) = 0;
        
    };

#endif      // MPHONEMENUANDCBAEVENTS_H   
            
// End of File
