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
* Description:  Base class for voip command parameter classes.
*
*/

#ifndef TPHONECOMMANDPARAMVOIP_H
#define TPHONECOMMANDPARAMVOIP_H

//  INCLUDES
#include <e32std.h>
#include "tphonecommandparam.h"


// CLASS DECLARATION

/**
*  A base class for voip command parameter.
*  
*/
class TPhoneCommandParamVoip : public TPhoneCommandParam
    {
    public: // enum

    /** 
     * TPhoneParamIdVoip is list of possible voip 
     * phone ui command parameters 
     */
    enum TPhoneParamIdVoip
        {
        EPhoneParamIdSelector = EPhoneParamIdLast
        };

    };

#endif // TPHONECOMMANDPARAMVOIP_H   
            
// End of File
