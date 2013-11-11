/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPELogHandlingProxy
*              : class.
*
*/


#ifndef    CPELOGHANDLINGPROXY_H
#define    CPELOGHANDLINGPROXY_H

//INCLUDES
#include "mpeloghandling.h"
#include <e32base.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION

/**
*  This proxy class provides for the access to call logging services.
*  The construction of log handling component is two-fold:
*  1) CreateFirstPhaseL() creates the proxy object along with log handling dummy
*  2) CreateSecondPhaseL() creates the real subject with capability to log calls
*
*  @lib LogHandling.lib
*  @since Series60_4.0
*/
class CPELogHandlingProxy : public CBase,
                        public MPELogHandling
    {
    public:
        /**
        * Create log handling proxy and dummy implementation
        * @return pointer to log handling proxy object, i.e. the handle to
        * access log handling services
        */
        IMPORT_C static MPELogHandling* CreateFirstPhaseL();

        /**
        * Create log handling sub system
        * This method creates the actual log handling implementation which
        * replaces the dummy component in use. The phone model should call this
        * function as all the critical sub systems already enable emergency call
        * capabilities.
        * @param aModel, handle to the owner of this object and phone engine
        * @param aFsSession, handle to the shared file server session
        */
        IMPORT_C void CreateSecondPhaseL( MPEPhoneModelInternal& aModel, RFs& aFsSession );

    public: // From MPELogHandling
        /**
         * Destructor.
         */          
        IMPORT_C ~CPELogHandlingProxy();

        /**
        * Redirects the function call to either log handling implementation or
        * log handling dummy implementation.
        * @param aCallId, call id
        * @return possible error code.
        */
        IMPORT_C TInt SaveCallEntry( const TInt aCallId );

    protected:
        /**
        * C++ default constructor.
        */
        CPELogHandlingProxy();

        /**
        * Constructor, second phase. 
        */
        void ConstructL();

    private: // data
        // Handle to log handling implementation
        MPELogHandling* iLogHandling; 
        // Handle to log handling dummy implementation
        MPELogHandling* iLogHandlingDummy;  
    };

#endif // CPELOGHANDLINGPROXY_H

// End of File

