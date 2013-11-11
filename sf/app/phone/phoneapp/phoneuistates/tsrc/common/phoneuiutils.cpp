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
* Description:  PhoneUIUtils.lib stubs
*
*/


#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamkeyevent.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamaudiooutput.h"
#include "tphonecmdparamemergencycallheaderdata.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamappinfo.h"
#include "cphonepubsubproxy.h"
#include "cphonelogger.h"
#include "cphoneresourceresolverbase.h"
#include "cphonemainerrormessageshandler.h"
#include "cphonekeys.h"
#include <eikenv.h>

// Note: Commented out functions are not in use in this stub (yet).
//       They are left in place in case they are needed later.


// ================= MEMBER FUNCTIONS =======================
CPhoneMainErrorMessagesHandler * CPhoneMainErrorMessagesHandler::Instance()
    {
    
    }

CPhoneResourceResolverBase::CPhoneResourceResolverBase() :
    iEnv( *CEikonEnv::Static() )
    {
    }

CPhoneResourceResolverBase::~CPhoneResourceResolverBase()
    {
    }

TInt CPhoneResourceResolverBase::ResolveResourceID( const TInt& /*aParam*/ ) const
    {
    }

TInt CPhoneResourceResolverBase::IsTelephonyFeatureSupported( TInt /*aParam*/ ) const
    {
    }

TPhoneCmdParamEmergencyCallHeaderData::TPhoneCmdParamEmergencyCallHeaderData()
    {
    }

void TPhoneCmdParamEmergencyCallHeaderData::SetHeaderText( const TDesC16& /*aParam*/ )
    {
    }

void TPhoneCmdParamEmergencyCallHeaderData::SetCiphering( TBool /*aParam*/ )
    {
    }

void TPhoneCmdParamEmergencyCallHeaderData::SetCipheringIndicatorAllowed( TBool /*aParam*/ )
    {
    }

int CPhoneKeys::IsNumericKey(const TKeyEvent& /*aEvent*/, TEventCode /*aKey*/ )
    {
    }

TPhoneCmdParamAudioOutput::TPhoneCmdParamAudioOutput()
    {
    }

void TPhoneCmdParamAudioOutput::SetAudioOutput( TPEAudioOutput /*aParam*/ )
    {
    }

TPhoneCmdParamString::TPhoneCmdParamString()
    {
    }

TPhoneCmdParamKeyEvent::TPhoneCmdParamKeyEvent()
    {
    }

void TPhoneCmdParamString::SetString( TPtr16* /*aString*/ )
    {
    }

void TPhoneCmdParamKeyEvent::SetKeyEvent( const TKeyEvent& /*aEvent*/ )
    {
    }

void TPhoneCmdParamKeyEvent::SetEventCode( TEventCode /*aEvent*/ )
    {
    }

TPhoneCmdParamInteger::TPhoneCmdParamInteger():
    TPhoneCommandParam(),
    iInteger( 0 )
    {
    iParamId = EPhoneParamIdInteger;
    }

void TPhoneCmdParamInteger::SetInteger( TInt aInteger )
   {
   iInteger = aInteger;   
   }

TInt TPhoneCmdParamInteger::Integer() const
   {
   return iInteger;
   }

TPhoneCmdParamBoolean::TPhoneCmdParamBoolean() :
    TPhoneCommandParam(),
    iBoolean( EFalse )
    {
    iParamId = EPhoneParamIdBoolean;
    }

void TPhoneCmdParamBoolean::SetBoolean( TBool aBoolean )
   {
   iBoolean = aBoolean;   
   }

TBool TPhoneCmdParamBoolean::Boolean() const
   {
   return iBoolean;
   }
    
TPhoneCmdParamCallStateData::TPhoneCmdParamCallStateData()
    {
    }
    
void TPhoneCmdParamCallStateData::SetCallId( TInt /*aParam*/ )
    {
    }

TInt TPhoneCmdParamCallStateData::CallId() const
    {
    }
    
void TPhoneCmdParamCallStateData::SetCallState( TInt /*aState*/ )
    {
    }

TInt TPhoneCmdParamCallStateData::CallState() const
    {
    }
    
TPhoneCmdParamGlobalNote::TPhoneCmdParamGlobalNote()
    {
    }
    
void TPhoneCmdParamGlobalNote::SetTextResourceId( TInt /*aResource*/ )
    {
    }

void TPhoneCmdParamGlobalNote::SetText( const TDesC16& /*aParam*/ )
    {
    }
    
TPhoneCmdParamAppInfo::TPhoneCmdParamAppInfo()
    {
    }
    
void TPhoneCmdParamAppInfo::SetAppUid( const TUid& /*aUid*/ )
    {
    }
    
void TPhoneCmdParamAppInfo::SetViewUid( const TUid& /*aUid*/ )
    {
    }
    
void TPhoneCmdParamAppInfo::SetCustomMessageId( const TUid& /*aUid*/ )
    {
    }
    
TInt CPhonePubSubProxy::Value( const TUid& /*aUid*/, TUint /*aValue*/ )
    {
    }
    
    
void CPhoneLogger::LogIt( TPhoneUILoggingLevel /*aLevel*/, 
            TPhoneUILoggingComponent /*aLoggingComponent*/, 
            TRefByValue<const TDesC> /*aFmt*/, ...)
    {
    }
    
void CPhoneLogger::LogStateChange( 
    const TInt /*aCurrentState*/, 
    const TInt /*aNewState*/ )
    {
    }

// -----------------------------------------------------------------------------
// TPhoneCommandParam::TPhoneCommandParam
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPhoneCommandParam::TPhoneCommandParam()
    {
    }

// -----------------------------------------------------------------------------
// TPhoneCommandParam::ParamId
// -----------------------------------------------------------------------------
//
TPhoneCommandParam::TPhoneParamId TPhoneCommandParam::ParamId() const
    {
    return iParamId;
    }
//
// -----------------------------------------------------------------------------
// TPhoneCmdParamCallHeaderData::TPhoneCmdParamCallHeaderData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPhoneCmdParamCallHeaderData::TPhoneCmdParamCallHeaderData() :
        TPhoneCommandParam(),
        iLabelText( NULL ),
        iShortLabelText( NULL ),
        iCallState( 0 ),
        iNumberType( 0 ),
        iCiphering( EFalse ),
        iCLIText( NULL ),
        iCNAPText( NULL ),
        iPicture( NULL ),
        iLine2 ( EFalse ),
        iThumbnail( NULL ),
        iCallerText( KNullDesC ),
        iParticipantListCLI( EPhoneParticipantCLIText ),
        iCipheringIndicatorAllowed( ETrue ),
        iContactLink( KNullDesC8 ),
        iRemotePhoneNumber( KNullDesC ),
        iDiverted( EFalse )
        {
        iParamId = EPhoneParamIdCallHeaderData;
        }

//// ---------------------------------------------------------
//// TPhoneCmdParamCallHeaderData::SetLabelText
//// Sets the call header label text
//// (other items were commented in a header).
//// ---------------------------------------------------------
////
void TPhoneCmdParamCallHeaderData::SetLabelText(
   const TDesC& aLabelText )
    {
    delete iLabelText;
    iLabelText = aLabelText.Alloc();
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetShortLabelText
// Sets the call header short label text
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetShortLabelText(
   const TDesC& aShortLabelText )
    {
    delete iShortLabelText;
    iShortLabelText = aShortLabelText.Alloc();
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetCallState
// Sets the call header call state
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetCallState( TInt aCallState )
   {
   iCallState = aCallState;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetCallType
// Sets the call header call type
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetCallType( TInt aCallType )
   {
   iCallType = aCallType;
   }

//// ---------------------------------------------------------
//// TPhoneCmdParamCallHeaderData::SetNumberType
//// Sets the call header number type
//// (other items were commented in a header).
//// ---------------------------------------------------------
////
void TPhoneCmdParamCallHeaderData::SetNumberType( TInt aNumberType )
   {
   iNumberType = aNumberType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetCiphering
// Sets the call header ciphering status
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetCiphering(
   TBool aCiphering )
   {
   iCiphering = aCiphering;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetCLIText
// Sets the call header CLI text
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetCLIText( 
   const TDesC& aCLIText,
   TPhoneTextClippingDirection aDirection )
   {
   delete iCLIText;
   iCLIText = aCLIText.Alloc();
   iCLITextClippingDirection = aDirection; 
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::CallState
// Returns the call header call state
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPhoneCmdParamCallHeaderData::CallState() const
   {
   return iCallState;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::CallType
// Returns the call header call type
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPhoneCmdParamCallHeaderData::CallType() const
   {
   return iCallType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::Line2
// Returns the call header line2
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool TPhoneCmdParamCallHeaderData::Line2() const
   {
   return iLine2;
   }

//// ---------------------------------------------------------
//// TPhoneCmdParamCallHeaderData::SetCipheringIndicatorAllowed
//// Sets the ciphering indicator allowed status
//// (other items were commented in a header).
//// ---------------------------------------------------------
////
void TPhoneCmdParamCallHeaderData::SetCipheringIndicatorAllowed(
    TBool aAllowed )
    {
    iCipheringIndicatorAllowed = aAllowed;
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetServiceId
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetServiceId( TUint32 aServiceId )
    {
    iServiceId = aServiceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::ServiceId
// ---------------------------------------------------------
//
TUint32 TPhoneCmdParamCallHeaderData::ServiceId() const
    {
    return iServiceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetContactLink
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetContactLink(
    const TDesC8& aContactLink )
    {
    iContactLink.Set( aContactLink );
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::ContactLink
// ---------------------------------------------------------
//
const TDesC8& TPhoneCmdParamCallHeaderData::ContactLink() const
    {
    return iContactLink;
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::SetRemotePhoneNumber
// ---------------------------------------------------------
//
void TPhoneCmdParamCallHeaderData::SetRemotePhoneNumber(
    const TDesC& aPhoneNumber )
    {
    iRemotePhoneNumber.Set( aPhoneNumber );
    }

// ---------------------------------------------------------
// TPhoneCmdParamCallHeaderData::RemotePhoneNumber
// ---------------------------------------------------------
//
const TDesC& TPhoneCmdParamCallHeaderData::RemotePhoneNumber() const
    {
    return iRemotePhoneNumber;
    }

void TPhoneCmdParamCallHeaderData::SetCNAPText( 
  const TDesC& aCNAPText,
  TPhoneTextClippingDirection aDirection )
  {
  delete iCNAPText;
  iCNAPText = aCNAPText.Alloc();
  iCNAPTextClippingDirection = aDirection; 
  }

void TPhoneCmdParamCallHeaderData::SetPicture( 
   const TDesC& aPicture )
   {
   iPicture.Set( aPicture );
   }

void TPhoneCmdParamCallHeaderData::SetDiverted(
    TBool aDiverted )
    {
    iDiverted = aDiverted;        
    }

void TPhoneCmdParamCallHeaderData::SetLine2(
    TBool aLine2 )
   {
   iLine2 = aLine2;
   }
    
const TDesC& TPhoneCmdParamCallHeaderData::LabelText() const
   {
   return iLabelText ? *iLabelText : KNullDesC();
   }

TBool TPhoneCmdParamCallHeaderData::Ciphering() const
   {
   return iCiphering;
   }

const TDesC& TPhoneCmdParamCallHeaderData::CLIText() const
   {
   return iCLIText ? *iCLIText : KNullDesC();
   }

TPhoneCmdParamCallHeaderData::TPhoneTextClippingDirection 
TPhoneCmdParamCallHeaderData::CLITextClippingDirection() const
    {
    return iCLITextClippingDirection;
    }

TPhoneCmdParamCallHeaderData::TPhoneTextClippingDirection 
TPhoneCmdParamCallHeaderData::CNAPTextClippingDirection() const
    {
    return iCNAPTextClippingDirection;
    }

const TDesC& TPhoneCmdParamCallHeaderData::CNAPText() const
   {
   return iCNAPText ? *iCNAPText : KNullDesC();
   }
const TDesC& TPhoneCmdParamCallHeaderData::Picture() const
  {
  return iPicture;
  }
TBool TPhoneCmdParamCallHeaderData::Diverted() const
    {
    return iDiverted;        
    }
CFbsBitmap* TPhoneCmdParamCallHeaderData::Thumbnail() const
   {
   return iThumbnail;
   }
void TPhoneCmdParamCallHeaderData::SetThumbnail( 
   CFbsBitmap* aThumbnail )
   {
   iThumbnail = aThumbnail;
   }
void TPhoneCmdParamCallHeaderData::SetCallerText( 
   const TDesC& aCallerText )
   {
   iCallerText.Set( aCallerText );
   }
const TDesC& TPhoneCmdParamCallHeaderData::CallerText() const
  {
  return iCallerText;
  }

void TPhoneCmdParamCallHeaderData::SetParticipantListCLI(
    TPhoneParticipantListCLI aCLIType )
    {
    iParticipantListCLI = aCLIType;
    }

TPhoneCmdParamCallHeaderData::TPhoneParticipantListCLI 
    TPhoneCmdParamCallHeaderData::ParticipantCLI() const
    {
    return iParticipantListCLI;    
    }       

TBool TPhoneCmdParamCallHeaderData::HasThumbnail() const
    {
    return iHasThumbnail;
    }    

void TPhoneCmdParamCallHeaderData::SetHasThumbnail( TBool aStatus )
    {
    iHasThumbnail = aStatus;
    }

TBool TPhoneCmdParamCallHeaderData::CipheringIndicatorAllowed() const    
   {
   return iCipheringIndicatorAllowed;
   }

TPhoneCmdParamCallHeaderData::~TPhoneCmdParamCallHeaderData()
   {
   delete iLabelText;
   delete iShortLabelText;
   delete iCLIText;
   delete iCNAPText;
   }

// end of file
