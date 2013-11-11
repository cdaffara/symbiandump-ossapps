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
* Description: Implementation of TPhoneCmdParamCustomDialer class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamcustomdialer.h"    

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamDynMenu::TPhoneCmdParamDynMenu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamCustomDialer::TPhoneCmdParamCustomDialer() :
    TPhoneCommandParam(),
    iCustomDialer( NULL )
    {
    iParamId = EPhoneParamIdCustomDialer;
    }

// ---------------------------------------------------------
// TPhoneCmdParamCustomDialer::SetDialerController
// Sets the dialer controller
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamCustomDialer::SetDialerController( MPhoneDialerController* aDialerController )
    {  
    iCustomDialer = aDialerController;
    }

// ---------------------------------------------------------
// TPhoneCmdParamCustomDialer::DialerController
// Return pointer to dialer controller
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C MPhoneDialerController* TPhoneCmdParamCustomDialer::DialerController()
    {
    return iCustomDialer;
    }

// End of file
