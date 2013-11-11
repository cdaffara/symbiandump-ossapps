/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MPHONENUMBERENTRYEVENTS_H
#define MPHONENUMBERENTRYEVENTS_H


//  INCLUDES
#include <e32def.h>
#include <e32keys.h>
#include <w32std.h>

// CLASS DECLARATION

/**
*  MPhoneKeyEvents:
*  An abstract class whose purpose is to define an interface from the key
*  event handler to the UI controller.
*/

class MPhoneNumberEntryEvents
{
    public:

        /**
        * Destructor
        */
        virtual ~MPhoneNumberEntryEvents(){};

    public: // Message handling functions

        virtual void HandleNumberEntryEdited() = 0;
};

#endif // MPHONENUMBERENTRYEVENTS_H
