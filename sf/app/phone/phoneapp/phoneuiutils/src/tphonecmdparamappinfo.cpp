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
* Description: Implementation of TPhoneCmdParamAppInfo class.
*
*/


// INCLUDE FILES

#include "tphonecmdparamappinfo.h"   

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C TPhoneCmdParamAppInfo::TPhoneCmdParamAppInfo() :
    TPhoneCommandParam(),
    iAppUid( KNullUid ),
    iViewUid( KNullUid ),
    iParam( KNullDesC8 ),
    iCustomMessageId( KNullUid ), // NULL for no message
    iCustomMessage( KNullDesC8 ),
    iMultipleInstances( EFalse )
    {
    iParamId = EPhoneParamIdAppInfo;
    }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::SetAppUid
// Sets applicaiton uid.
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAppInfo::SetAppUid( const TUid& aAppUid )
   {
   iAppUid = aAppUid;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::SetViewUid
// Sets View uid.
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAppInfo::SetViewUid( const TUid& aViewUid )
   {
   iViewUid = aViewUid;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::SetParam
// Sets application parameter
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAppInfo::SetParam( const TDesC8& aParam )
   {
   iParam.Set( aParam );
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::SetCustomMessageId
// Sets Custom Message Id.
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAppInfo::SetCustomMessageId( 
   const TUid& aCustomMessageId )
   {
   iCustomMessageId = aCustomMessageId;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::SetCustomMessage
// Sets Custom message
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAppInfo::SetCustomMessage( 
   const TDesC8& aCustomMessage )
   {
   iCustomMessage.Set( aCustomMessage );
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::SetMultipleInstances
// Sets the application's multiple instance status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAppInfo::SetMultipleInstances( 
   TBool aMultipleInstances )
   {
   iMultipleInstances = aMultipleInstances;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::AppUid
// Returns application uid
// ---------------------------------------------------------
//
EXPORT_C TUid TPhoneCmdParamAppInfo::AppUid() const
   {
   return iAppUid;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::ViewUid
// Returns View uid
// ---------------------------------------------------------
//
EXPORT_C TUid TPhoneCmdParamAppInfo::ViewUid() const
   {
   return iViewUid;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::Param
// Returns App parameter
// ---------------------------------------------------------
//
EXPORT_C const TDesC8& TPhoneCmdParamAppInfo::Param() const
   {
   return iParam;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::CustomMessageId
// Returns Custom Message Id
// ---------------------------------------------------------
//
EXPORT_C TUid TPhoneCmdParamAppInfo::CustomMessageId() const
   {
   return iCustomMessageId;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::CustomMessage
// Returns Message
// ---------------------------------------------------------
//
EXPORT_C const TDesC8& TPhoneCmdParamAppInfo::CustomMessage() const
   {
   return iCustomMessage;
   }

// ---------------------------------------------------------
// TPhoneCmdParamAppInfo::MultipleInstances
// Returns the application's multiple instance status
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamAppInfo::MultipleInstances() const
   {
   return iMultipleInstances;
   }

//  End of File  
