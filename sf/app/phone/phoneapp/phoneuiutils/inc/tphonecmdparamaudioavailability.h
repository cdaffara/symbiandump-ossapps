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
* Description: 
*   See class description.
*
*/


#ifndef __TPHONECMDPARAMAUDIOAVAILABILITY_H
#define __TPHONECMDPARAMAUDIOAVAILABILITY_H

//  INCLUDES

#include <w32std.h>
#include <pevirtualengine.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for call header information.
*/
class TPhoneCmdParamAudioAvailability : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamAudioAvailability();

    public: 
 
        /**
        * BT Acc available mode
        * @param aAudioOutput is the accessory mode
        */        
        IMPORT_C void SetBTAccAvailable( TBool aState );
        
        /**
        * Returns the BT Acc available mode
        * @return Returns the audio output
        */
        IMPORT_C TBool IsBTAccAvailable() const;

        /**
        * Wired Acc available mode
        * @param aAudioOutput is the accessory mode
        */        
        IMPORT_C void SetWiredAccAvailable( TBool aState );
        
        /**
        * Returns the Wired Acc available mode
        * @return Returns the audio output
        */
        IMPORT_C TBool IsWiredAccAvailable() const;
 
    private:    

        /**
        * BT Acc available mode 
        */
        TBool iBTAccAvailable;

        /**
        * Wired Acc available mode 
        */
        TBool iWiredAccAvailable;
    };

#endif // _TPHONECMDPARAMAUDIOOUTPUT_H  
            
// End of File
