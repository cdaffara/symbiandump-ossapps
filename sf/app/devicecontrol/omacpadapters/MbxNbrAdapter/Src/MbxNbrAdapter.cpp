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
* Description:  Handles mailbox number settings in OMA provisioning.
*
*/


// INCLUDE FILES
#include    <f32file.h>
#include    <CWPCharacteristic.h>
#include    <CWPParameter.h>
#include    <wpmbxnbradapterresource.rsg>
#include    <cvoicemailbox.h>
#include 		<cvoicemailboxentry.h>
#include    <commonphoneparser.h>
#include 	<voicemailboxdomaincrkeys.h>
#include 	<centralrepository.h>

#include    "WPAdapterUtil.h"
#include    "MbxNbrAdapter.h"
#include    "ProvisioningDebug.h"

// CONSTANTS
#if ( defined (__WINS__) || defined (__WINSCW) ) // this different on hw
    _LIT( KAdapterName, "WPMbxNbrAdapterResource" );
#else
    _LIT( KAdapterName, "WPMbxNbrAdapter" );
#endif
_LIT( KMailboxNumberStr, "MBNMBR" );
_LIT( KMailboxTypeStr, "MBTYPE" );
_LIT( KCSVideoMailboxType, "VIDEO" );
_LIT( KVideoSupportType, "VIDEO_MBOX_SUPPORT" );
_LIT( KVideoSupportTypeTrue, "TRUE" );
_LIT( KVideoSupportTypeFalse, "FALSE" );
_LIT( KMailboxAppId, "w9027" );        // From OMA provisioning registration document
const TInt KSettingsGranularity = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::CMbxNbrAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMbxNbrAdapter::CMbxNbrAdapter() : 
    iSettings( KSettingsGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::ConstructL()
    {
    FLOG( _L( "[Provisioning] CMbxNbrAdapter::ConstructL:" ) );
    
    TFileName fileName;
    Dll::FileName( fileName );
    iTitle = WPAdapterUtil::ReadHBufCL( fileName,
                                        KAdapterName,
                                        R_MAILBOX_NUMBER_ADAPTER_TITLE );
                                        
    FLOG( _L( "[Provisioning] CMbxNbrAdapter::ConstructL: Done" ) );
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMbxNbrAdapter* CMbxNbrAdapter::NewL()
    {
    CMbxNbrAdapter* self = new( ELeave ) CMbxNbrAdapter; 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::~CMbxNbrAdapter
// Destructor
// -----------------------------------------------------------------------------
//
CMbxNbrAdapter::~CMbxNbrAdapter()
    {
    delete iTitle;
    iSettings.Close();
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::ItemCount
// -----------------------------------------------------------------------------
//
TInt CMbxNbrAdapter::ItemCount() const
    {
	// SummaryTitle(), SummaryText(), SaveL(), SetAsDefaultL() and 
	// CanSetAsDefault() are called ItemCount() times
	return iSettings.Count();
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CMbxNbrAdapter::SummaryTitle( TInt /*aIndex*/ ) const
    {
    return *iTitle;
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CMbxNbrAdapter::SummaryText( TInt aIndex ) const
    {
    // SummaryText should be called ItemCount() times
    __ASSERT_DEBUG( iSettings.Count() > aIndex, 
        User::Panic( _L("CMbxNbrAdapter::SummaryText"),  KErrCorrupt ) );
    __ASSERT_DEBUG( aIndex >= 0, 
        User::Panic( _L("CMbxNbrAdapter::SummaryText"),  KErrCorrupt ) );              
    return iSettings[ aIndex ].iName;
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CMbxNbrAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/ )
    {
    // Detail view is a feature for later release.
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::SaveL( TInt aItem )
    {
    FLOG( _L( "[Provisioning] CMbxNbrAdapter::SaveL:" ) );
    __ASSERT_DEBUG( iSettings.Count() > aItem, User::Leave( KErrCorrupt ) ); 
    __ASSERT_DEBUG( aItem >= 0, User::Leave( KErrCorrupt ) ); 
   
    //RVmbxNumber model;
    //User::LeaveIfError( model.Open() );
    //CleanupClosePushL( model );  
       
    TInt err( KErrNone );    
    switch( iSettings[aItem].iMailboxType )
        {
        case ECSVoiceMailbox:
        		
        		CVoiceMailbox* voiceModel = CVoiceMailbox::NewLC();
    				CVoiceMailboxEntry* voiceEntry = CVoiceMailboxEntry::NewLC();
        		voiceEntry->SetVoiceMailboxType(EVmbxVoice);
        		voiceEntry->SetVmbxNumber(iSettings[aItem].iMailboxNumber);
        		voiceModel->SaveProvisionedEntry(*voiceEntry);
        		CleanupStack::PopAndDestroy(voiceEntry);
        		CleanupStack::PopAndDestroy(voiceModel);	            
            break;
        case ECSVideoMailbox:
        
        		CVoiceMailbox* videoModel = CVoiceMailbox::NewLC();
    				CVoiceMailboxEntry* videoEntry = CVoiceMailboxEntry::NewLC();        		
            videoEntry->SetVoiceMailboxType(EVmbxVideo);
            videoEntry->SetVmbxNumber(iSettings[aItem].iMailboxNumber);
            videoModel->SaveProvisionedEntry(*videoEntry);
            CleanupStack::PopAndDestroy(videoEntry);
        		CleanupStack::PopAndDestroy(videoModel);
        			
            if ( iSettings[aItem].iIsVideosupport )
            {
            CRepository* cenrep = CRepository::NewL( KCRUidVideoMailbox );
	        FLOG( _L( "[Provisioning] CMbxNbrAdapter::SaveL:case Saving it to Cenrep" ) );
	        TBuf<100> buf1;
	        buf1.AppendNum( iSettings[aItem].iVidiosupport );
	        
	        FLOG( buf1 );
            CleanupStack::PushL(cenrep);
			User::LeaveIfError(
			cenrep->Set( KVideoMbxSupport, iSettings[aItem].iVidiosupport )); 
			FLOG( _L( "[Provisioning] CMbxNbrAdapter::SaveL:case Saved" ) );
			CleanupStack::PopAndDestroy(cenrep);
            }
            break;
        default:
            User::Leave( KErrCorrupt );
            break;  
        }

    User::LeaveIfError( err );        
    //CleanupStack::PopAndDestroy();  // Close model
    
    FLOG( _L( "[Provisioning] CStreamingAdapter::SaveL: Done" ) );
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CMbxNbrAdapter::CanSetAsDefault( TInt /*aItem*/ ) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::SetAsDefault
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::SetAsDefaultL( TInt /*aItem*/ )
    {
    // This shouldn't be called because CanSetAsDefault
    // always returns EFalse.
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::VisitL( CWPCharacteristic& aCharacteristic )
    {
    switch( aCharacteristic.Type() )
        {
        case KWPApplication:
            // Initialize iTempData's members
            iTempData.iName.Set( KNullDesC );
            iTempData.iMailboxNumber.Set( KNullDesC );
            iTempData.iVidiosupport = EFalse;
            iTempData.iIsVideosupport = EFalse;
            iTempData.iMailboxType = ECSVoiceMailbox;

            // Accept characteristic
            aCharacteristic.AcceptL( *this );

            // Check iAppId and validate data
            if( iAppId == KMailboxAppId && IsValid() )
                {
                // iAppId is correct and data is valid
                // -> Lets append it to array                
                iSettings.Append( iTempData );
                }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::VisitL( CWPParameter& aParameter )
    {   
    switch( aParameter.ID() )
        {
        case EWPParameterAppID:
            iAppId.Set( aParameter.Value() );
            break;
        case EWPParameterName:
            iTempData.iName.Set( aParameter.Value() );
            break;
		case EWPNamedParameter:
			{
			if( aParameter.Name().Compare( KMailboxNumberStr ) == 0 )
			    {
			    iTempData.iMailboxNumber.Set( aParameter.Value() );
			    }
			else if( aParameter.Name().Compare( KMailboxTypeStr ) == 0 )
			    {
			    ParseMbxTypeL( aParameter.Value(), iTempData.iMailboxType );
			    }
			else if( aParameter.Name().Compare( KVideoSupportType ) == 0)
				{
			FLOG( _L( "[Provisioning] CMbxNbrAdapter::VisitL:Visiting to ParseVideoTypeL" ) );
				ParseVideoTypeL( aParameter.Value(), iTempData.iVidiosupport, iTempData.iIsVideosupport );	
			    }
			break;
			}
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMbxNbrAdapter::VisitLinkL
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::VisitLinkL( CWPCharacteristic& /*aCharacteristic*/ )
    { 
    }


// -----------------------------------------------------------------------------
// CMbxNbrAdapter::IsValid
// -----------------------------------------------------------------------------
//
TBool CMbxNbrAdapter::IsValid()
    {
    TBool validity( EFalse );
    
    // Validity check:
    //  iMailboxType is either ECSVideoMailbox or ECSVoiceMailbox and
    //  iMailboxNumber is valid number in e.164 format  
    if( ( iTempData.iMailboxType == ECSVideoMailbox || 
        iTempData.iMailboxType == ECSVoiceMailbox ) &&
        IsValidPhoneNumber( iTempData.iMailboxNumber ) )
        {
        validity = ETrue;
        }    
        
    return validity;
    }
    
// -----------------------------------------------------------------------------
// CMbxNbrAdapter::IsValidPhoneNumber
// -----------------------------------------------------------------------------
//
TBool CMbxNbrAdapter::IsValidPhoneNumber( const TDesC& aPtr ) 
    {
    return CommonPhoneParser::IsValidPhoneNumber( aPtr, 
                                CommonPhoneParser::EPhoneClientNumber );
    }       
    
// -----------------------------------------------------------------------------
// CMbxNbrAdapter::ParseMbxTypeL
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::ParseMbxTypeL( const TDesC& aPtr, TMailboxType& aType ) 
    {
    if( aPtr.Length() )
        {
        if( aPtr.Compare( KCSVideoMailboxType ) == 0 )
            {
            aType = ECSVideoMailbox;   
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }
    else
        {
        aType = ECSVoiceMailbox;
        }
    }
// -----------------------------------------------------------------------------
// CMbxNbrAdapter::ParseVideoTypeL
// -----------------------------------------------------------------------------
//
void CMbxNbrAdapter::ParseVideoTypeL( const TDesC& aPtr, TBool& aType, TBool& aType1 ) 
    {
    FLOG( _L( "[Provisioning] CMbxNbrAdapter::ParseVideoTypeL:in" ) );
    if( aPtr.Length() )
        {
        FLOG( _L( "[Provisioning] CMbxNbrAdapter::ParseVideoTypeL:If there is ON/OFF Value" ) );
        if( aPtr.Compare( KVideoSupportTypeTrue ) == 0 )
            {
            FLOG( _L( "[Provisioning] CMbxNbrAdapter::ParseVideoTypeL:if ON set TRUE" ) );
            aType = ETrue;
            aType1 = ETrue;
            }
        else if( aPtr.Compare( KVideoSupportTypeFalse ) == 0 )
            {
            FLOG( _L( "[Provisioning] CMbxNbrAdapter::ParseVideoTypeL:if OFF set False" ) );
            aType = EFalse;
            aType1 = ETrue;
            }
        else
        {
        	FLOG( _L( "[Provisioning] CMbxNbrAdapter::ParseVideoTypeL:if some thing else set False" ) );
        	aType1 = EFalse;
        }
        }
    else
        {
        FLOG( _L( "[Provisioning] CMbxNbrAdapter::ParseVideoTypeL:no ON/OFF" ) );
        aType1 = EFalse;
        }
    }
	
//  End of File  
