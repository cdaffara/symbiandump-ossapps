/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCommandParam class.
*
*/


// INCLUDE FILES
#include "tphonecommandparam.h"
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCommandParam::TPhoneCommandParam
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCommandParam::TPhoneCommandParam()
    {
    }

// -----------------------------------------------------------------------------
// TPhoneCommandParam::ParamId
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCommandParam::TPhoneParamId TPhoneCommandParam::ParamId() const
    {
    __ASSERT_DEBUG( iParamId, Panic( EPhoneUtilsInvariant ) );
    return iParamId;
    }

//  End of File  
