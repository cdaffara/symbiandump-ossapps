/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: Nokia
*
* Description: An abstract class for accessing generic state properties
*
*/


#ifndef MPHONESTATEPROPERTIES_H
#define MPHONESTATEPROPERTIES_H

// INCLUDES
#include <e32cmn.h>
#include <eikenv.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  An abstract class for accessing generic state properties
*
*/
class MPhoneStateProperties
    {
    public:

        /**
        * Sets CEikonEnv handle to minimize static system calls
        * @param CEikonEnv handle
        */
        virtual void SetEikonEnv( CEikonEnv* aEnv ) = 0;
        
    };

#endif // MPHONESTATEPROPERTIES_H
            
// End of File
