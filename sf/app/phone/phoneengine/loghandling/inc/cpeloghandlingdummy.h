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
* Description:  This file contains the header file of the CPELogHandlingDummy
*              : class.
*
*/


#ifndef    CPELOGHANDLINGDUMMY_H
#define    CPELOGHANDLINGDUMMY_H

//INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <logwrap.h>
#include "mpeloghandling.h"

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPELogHandlingCommand;
class CPELogExternalData;
class CPELogEvent;
class CLogClient;
class MPEPhoneModelInternal;
class MPEDataStore;

// CLASS DECLARATION

class CPELogHandlingDummy : public CBase,
                       public MPELogHandling
    {
    public:
        /**
        * Two-phased constructor
        */
        static MPELogHandling* NewL();
             
        /**
         * Destructor.
         */               
        virtual IMPORT_C ~CPELogHandlingDummy();
        
    protected:
        /**
        * C++ default constructor.
        */
        CPELogHandlingDummy();

    public: // From MPELogHandling
        /**
        * Save call entry
        * @since N/A
        * @param: TInt, call id
        * @return: possible error code.
        */
        IMPORT_C TInt SaveCallEntry( const TInt aCallId );
    };

#endif // CPELOGHANDLINGDUMMY_H

// End of File

