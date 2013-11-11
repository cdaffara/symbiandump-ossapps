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
*   See class description.
*
*/


#ifndef __TPHONECMDPARAMAUDIOOUTPUT_H
#define __TPHONECMDPARAMAUDIOOUTPUT_H

//  INCLUDES

#include <w32std.h>
#include <pevirtualengine.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for call header information.
*/
class TPhoneCmdParamAudioOutput : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamAudioOutput();

    public: 
        
        /**
        * Sets the audio output
        * @param aAudioOutput is the accessory mode
        */
        IMPORT_C void SetAudioOutput( 
            TPEAudioOutput aAudioOutput );

        /**
        * Returns the audio output
        * @return Returns the audio output
        */
        IMPORT_C TPEAudioOutput AudioOutput() const;
 
    private:    

        /**
        * Audio path
        */
        TPEAudioOutput iAudioOutput;
           
    };

#endif // _TPHONECMDPARAMAUDIOOUTPUT_H  
            
// End of File
