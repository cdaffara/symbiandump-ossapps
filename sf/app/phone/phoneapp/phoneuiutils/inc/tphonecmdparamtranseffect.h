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
* Description: 
*   See class description.
*
*/


#ifndef __TPHONECMDPARAMTRANSEFFECT_H
#define __TPHONECMDPARAMTRANSEFFECT_H

//  INCLUDES

#include <w32std.h>
#include <aknnotedialog.h> 
#include "tphonecommandparam.h"

// DATA TYPES

/**
* Enumerates effect types
*
* EPhoneTransitionEffectStart - TBD
* EPhoneTransitionEffectStop - TBD 
*/
enum TPhoneTransEffectType
    {
    EPhoneTransEffectNone,
    EPhoneTransEffectDialerOpen,
    EPhoneTransEffectDialerClose,
    EPhoneTransEffectDialerCreate,
    EPhoneTransEffectStop
    };

// CLASS DECLARATION

/**
*  A parameter class for note dialog information.
*/
class TPhoneCmdParamTransEffect : public TPhoneUICommandParam
    {   
   
    
    public: 
           
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamTransEffect();

    public: 
        
        /**
        * Sets the effect type
        * @param aType is the effect type
        */
        IMPORT_C void SetType( TPhoneTransEffectType aType );

 
        /**
        * Returns the effect type
        * @return Returns the type
        */
        IMPORT_C TPhoneTransEffectType Type() const;
   
    private:    
        
        /**
        * Note type
        */
        TPhoneTransEffectType iType;

    };

#endif // __TPHONECMDPARAMTRANSEFFECT_H   
            
// End of File
