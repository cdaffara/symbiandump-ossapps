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


#ifndef __TPHONECMDPARAMKEYCAPTURE_H
#define __TPHONECMDPARAMKEYCAPTURE_H

//  INCLUDES

#include <e32keys.h>
#include "tphonecommandparam.h"

// DATA TYPES
enum TPhoneCaptureType
{
    EPhoneKeyEvent,
    EPhoneKeyUpAndDownEvents,
    EPhoneKeyAllEvents
};

// CLASS DECLARATION

/**
*  A parameter class for key capture information.
*/
class TPhoneCmdParamKeyCapture : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamKeyCapture();

    public: 
        
        /**
        * Sets the key scan code.
        * @param aKey is the key scan code
        */
        IMPORT_C void SetKey( TStdScanCode aKey );

        
        /**
        * Sets the key code.
        * @param aKeyCode is the key code
        */
        IMPORT_C void SetKeyCode( TKeyCode aKeyCode );

        /**
        * Sets the key capture type.
        * @param aCaptureType is the key capture type
        */
        IMPORT_C void SetCaptureType( TPhoneCaptureType aCaptureType );

        /**
        * Returns the key scan code.
        * @return Returns the key scan code
        */
        IMPORT_C TStdScanCode Key() const;

        /**
        * Returns the key code.
        * @return Returns the key code
        */
        IMPORT_C TKeyCode KeyCode() const;

        /**
        * Returns the key capture type
        * @return Returns the key capture type
        */
        IMPORT_C TPhoneCaptureType CaptureType() const;

    private:    
        
        /**
        * Key scan code
        */
        TStdScanCode iKey;

        /**
        * Key code
        */
        TKeyCode iKeyCode;

        /**
        * Key capture type
        */
        TPhoneCaptureType iCaptureType;
    };

#endif // __TPHONECMDPARAMKEYCAPTURE_H   
            
// End of File
