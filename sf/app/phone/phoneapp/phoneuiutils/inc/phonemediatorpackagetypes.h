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
*     Package types which may be sent via Mediator.
*
*/


#ifndef PHONEMEDIATORPACKAGETYPES_H
#define PHONEMEDIATORPACKAGETYPES_H

// INCLUDES
#include <e32base.h>
#include "tphonecommandparam.h"

// CONSTANTS
const TInt KPhoneDescriptorSize = 200;

// DATA TYPES
class TCallIdAndCommandParam
    {
    public:
        TInt iCallId;
        TPhoneCommandParam iCommandParam;   
    };
    
class TCallIdAndTDesC
    {
    public:
        TInt iCallId;
        TBuf<KPhoneDescriptorSize> iMessage;        
    };

typedef TPckgBuf<TInt> TCallIdPackage;
typedef TPckgBuf<TPhoneCommandParam> TCommandParamPackage;
typedef TPckgBuf<TCallIdAndCommandParam> TCallIdAndCommandParamPackage;
typedef TPckgBuf<TCallIdAndTDesC> TCallIdAndTDesCPackage;
    
#endif // PHONEMEDIATORPACKAGETYPES_H

// End of File
