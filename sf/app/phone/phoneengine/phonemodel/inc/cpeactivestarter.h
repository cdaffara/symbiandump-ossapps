/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPEActiveStarter class.
*
*/


#ifndef CPEACTIVESTARTER_H
#define CPEACTIVESTARTER_H

//INCLUDES
#include <e32base.h>
#include "mpeactivestarter.h"

//CONSTANTS
//None.

// MACROS
//None.

// DATA TYPES
//None.

// FUNCTION PROTOTYPES
//None.

// FORWARD DECLARATIONS
//None.

// CLASS DECLARATION

/**
*  Starts Phone Engine's modules asynchronically. 
*  
*
*  @lib PhoneEngineBase.lib
*  @since Series60_2.7
*/
NONSHARABLE_CLASS( CPEActiveStarter ) : public CActive
    {
    public:    //Constructors and descructor

        /**
        * Two-phased constructor.
        */
        static CPEActiveStarter* NewL( MPEActiveStarter* aPEActiveStarter );
    
        /**
        * Destructor.
        */
        virtual ~CPEActiveStarter();

    public:    //New functions

        /**
        * Starts Phone engine constuction.
        * @since Series60_2.7
        */        
        void StartUp();

    protected: // From CActive

        /**
        * Cancels asyncronous request(s).
        * @since Series60_2.7
        */
        void DoCancel();

        /**
        * Standard RunError() method
        * @since Series60_2.7
        * @param  aError The leave code.
        * @return KErrNone.
        */
        TInt RunError( TInt aError );

        /**
        * Standard RunL() method
        * @since Series60_2.7
        */
        void RunL();
        
    private:
    
        /**
        * C++ default constructor.
        */
        CPEActiveStarter( MPEActiveStarter* aPEActiveStarter );
        
    private:    //Data

        // protocol specific active starter pointer
        MPEActiveStarter* iActiveStarter;
    };

#endif // CPEACTIVESTARTER_H

// End of File
