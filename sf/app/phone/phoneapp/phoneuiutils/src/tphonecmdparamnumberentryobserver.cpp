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
* Description: Implementation of TPhoneCmdParamNumberEntryObserver class.
*
*/

// INCLUDE FILES

#include "tphonecmdparamnumberentryobserver.h"   

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C TPhoneCmdParamNumberEntryObserver::TPhoneCmdParamNumberEntryObserver() :
    TPhoneCommandParam()
    {
    iParamId = EPhoneNumberEntryObserver;
    }

// ---------------------------------------------------------
// TPhoneCmdParamNumberEntryObserver::SetObserver
// Sets the key event
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamNumberEntryObserver::SetObserver( TCallBack aCallBack )
   {
   iCallBack = aCallBack;
   }

// ---------------------------------------------------------
// TPhoneCmdParamNumberEntryObserver::Observer
// Returns the observer
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TCallBack TPhoneCmdParamNumberEntryObserver::Observer()
   {
   return iCallBack;
   }


//  End of File  
