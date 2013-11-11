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
* Description:  Handles streaming app settings in OMA provisioning.
*
*/


// INCLUDE FILES
#include    <f32file.h>
#include    <CWPCharacteristic.h>
#include    <CWPParameter.h>
#include    <wpstreamingadapterresource.rsg>
#include    <mpsettingsmodel.h>
#include    <cmconnectionmethoddef.h>
#include    <cmmanagerext.h>
#include    "WPAdapterUtil.h"
#include    "StreamingAdapter.h"
#include    "ProvisioningDebug.h"

// CONSTANTS
#if ( defined (__WINS__) || defined (__WINSCW) ) // this different on hw
    _LIT( KAdapterName, "WPStreamingAdapterResource" );
#else
    _LIT( KAdapterName, "WPStreamingAdapter" );
#endif
_LIT(KMaxBandwidthStr,"MAX-BANDWIDTH");
_LIT(KMaxUdpPortStr,"MAX-UDP-PORT");
_LIT(KMinUdpPortStr,"MIN-UDP-PORT");
_LIT(KStreamingAppId, "554");

const TInt KInvalidValue = -1;
const TInt KProxyEnabled = 1;

const TInt KMinUDPPort = 1024; //6970;
const TInt KMaxUDPPort = 65535; //32000;
const TInt KDefaultMinUDPPort = 6970;
const TInt KDefaultMaxUDPPort = 32000;

const TInt KBandwidthValue1 = 9050;
const TInt KBandwidthValue2 = 13400;
const TInt KBandwidthValue3 = 18100;
const TInt KBandwidthValue4 = 26800;
const TInt KBandwidthValue5 = 27150;
const TInt KBandwidthValue6 = 40200;

// MODULE DATA STRUCTURES
struct TStreamingAdapterData
    {
    TPtrC iName;
    TInt iMaxBw;
    TInt iMinUdpPort;
    TInt iMaxUdpPort;
    CWPCharacteristic* iNapDef;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStreamingAdapter::CStreamingAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CStreamingAdapter::CStreamingAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::ConstructL()
    {
    FLOG( _L( "[Provisioning] CStreamingAdapter::ConstructL:" ) );
    
    TFileName fileName;
    Dll::FileName( fileName );
    iTitle = WPAdapterUtil::ReadHBufCL( fileName,
                                        KAdapterName,
                                        R_STREAMING_ADAPTER_TITLE );
                                        
    FLOG( _L( "[Provisioning] CStreamingAdapter::ConstructL: Done" ) );
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CStreamingAdapter* CStreamingAdapter::NewL()
    {
    CStreamingAdapter* self = new(ELeave) CStreamingAdapter; 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::~CStreamingAdapter
// Destructor
// -----------------------------------------------------------------------------
//
CStreamingAdapter::~CStreamingAdapter()
    {
    delete iTitle;
    delete iCurrentData;
    delete iTempData;
	delete iModel;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::ItemCount
// -----------------------------------------------------------------------------
//
TInt CStreamingAdapter::ItemCount() const
    {
    // If ItemCount() returns zero, the adapter is unloaded at startup. 
	// SummaryTitle(), SummaryText(), SaveL(), SetAsDefaultL() and 
	// CanSetAsDefault() are called ItemCount() times
    return iCurrentData ? 1 : 0;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CStreamingAdapter::SummaryTitle(TInt /*aIndex*/) const
    {
    return *iTitle;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CStreamingAdapter::SummaryText(TInt /*aIndex*/) const
    {
    return iCurrentData->iName;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CStreamingAdapter::DetailsL(TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/)
    {
    // Detail view is a feature for later release.
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::SaveL( TInt /*aItem*/ )
    {
    FLOG( _L( "[Provisioning] CStreamingAdapter::SaveL:" ) );
    
    __ASSERT_DEBUG( iCurrentData->iNapDef, User::Leave( KErrCorrupt ) );

    InitSettingsModelL();
        
    // Load default values
    iModel->LoadSettingsL(EConfigDefault);

    // Set max bandwidth value if valid
    if (iCurrentData->iMaxBw != KInvalidValue)
        {
        iModel->SetMaxBandwidth(iCurrentData->iMaxBw, EBearerGPRS);
        }

    // Set udp port range if valid
    if (iCurrentData->iMinUdpPort != KInvalidValue && 
        iCurrentData->iMaxUdpPort != KInvalidValue)
        {
        iModel->SetMinUDPPort(iCurrentData->iMinUdpPort);
        iModel->SetMaxUDPPort(iCurrentData->iMaxUdpPort);
        }

    // Get the IAP id
    
    RCmManagerExt  cmmanagerExt;
    cmmanagerExt.OpenL();
    CleanupClosePushL(cmmanagerExt);

    TPckgBuf<TUint32> uid;
    TBool proxyDefined(EFalse);
    TBool apDefined(EFalse);
    TBool apWithoutProxyDefined(EFalse);

    for (TInt i(0); iCurrentData->iNapDef->Data(i).Length() == uid.MaxLength()
        && !(proxyDefined && apWithoutProxyDefined); i++)
        {
        uid.Copy(iCurrentData->iNapDef->Data(i));
        RCmConnectionMethodExt cm;
        cm = cmmanagerExt.ConnectionMethodL( uid() );
        CleanupClosePushL( cm );

        TBool useProxy = cm.GetBoolAttributeL( CMManager::ECmProxyUsageEnabled );
        
        if (!apDefined || (!useProxy && !apWithoutProxyDefined))
            {
            iModel->SetDefaultAp(uid());
            apDefined = ETrue;

            iSavedID.Copy(uid);

            if (!useProxy)
                {
                apWithoutProxyDefined = ETrue;
                }
            }
        
        if (!proxyDefined && useProxy)
            {
           // Get proxy port
            TUint32 proxyPort = cm.GetIntAttributeL( CMManager::ECmProxyPortNumber );
            

            // Get proxy host name
            const HBufC* proxyHost = cm.GetStringAttributeL( CMManager::ECmProxyServerName );

            if(*proxyHost != KNullDesC && proxyPort <= 65535)
                {
                iModel->SetProxyHostNameL(*proxyHost);
                iModel->SetProxyPort(static_cast<TInt>(proxyPort));
                iModel->SetProxyMode(KProxyEnabled);
                
                proxyDefined = ETrue;
                }
            delete proxyHost;
            }
        CleanupStack::PopAndDestroy();//cm
        }

    CleanupStack::PopAndDestroy(); // cmmanagerext

    iModel->StoreSettingsL();
    
    FLOG( _L( "[Provisioning] CStreamingAdapter::SaveL: Done" ) );
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CStreamingAdapter::CanSetAsDefault(TInt /*aItem*/) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::SetAsDefault
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::SetAsDefaultL(TInt /*aItem*/)
    {
    // This shouldn't be called because CanSetAsDefault
    // always returns EFalse.
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::VisitL(CWPCharacteristic& aCharacteristic)
    {
    switch(aCharacteristic.Type())
        {
        case KWPApplication:
            // Create new iTempData
            if (iTempData)
                {
                delete iTempData;
                iTempData = NULL;
                }

            iTempData = new (ELeave) TStreamingAdapterData;

            // Initialize iTempData's members
            iTempData->iName.Set(KNullDesC);
            iTempData->iMaxBw = KInvalidValue;
            iTempData->iMinUdpPort = KInvalidValue;
            iTempData->iMaxUdpPort = KInvalidValue;
            iTempData->iNapDef = NULL;

            // Accept characteristic
            aCharacteristic.AcceptL(*this);

            // Check iAppId and validate data
            if (iAppId == KStreamingAppId && IsValid())
                {
                // iAppId is correct and data is valid
                delete iCurrentData;
                iCurrentData = iTempData;
                iTempData = NULL;
                }
            else
                {
                // iAppId is incorrect or data is invalid
                delete iTempData;
                iTempData = NULL;
                }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::VisitL(CWPParameter& aParameter)
    { 
    // Check the contents of whole characteristic before overwriting iMaxBw, 
    // iMinUdpPort and iMaxUdpPort. Might be looking at the wrong application
	// characteristic
    
    __ASSERT_DEBUG(iTempData, User::Leave(KErrCorrupt));
    
    TInt value;
    switch(aParameter.ID())
        {
        case EWPParameterAppID:
            iAppId.Set(aParameter.Value());
            break;
        case EWPParameterName:
            if (iTempData->iName == KNullDesC)
                {
                iTempData->iName.Set(aParameter.Value());
                }
            break;
		case EWPNamedParameter:
			{
			if( aParameter.Name().Compare( KMaxBandwidthStr ) == 0
				&& !ParseIntegerL( aParameter.Value(), value ) )
				{							
				iTempData->iMaxBw = value;
				}
			else if( aParameter.Name().Compare( KMaxUdpPortStr ) == 0
				&& !ParseIntegerL( aParameter.Value(), value ) )
				{							
				iTempData->iMaxUdpPort = value;
				}
			else if( aParameter.Name().Compare( KMinUdpPortStr ) == 0
				&& !ParseIntegerL( aParameter.Value(), value ) )
				{							
                iTempData->iMinUdpPort = value;
				}
			break;
			}
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::VisitLinkL
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::VisitLinkL(CWPCharacteristic& aCharacteristic)
    { 
    // Here again, do not overwrite iPXLogical before you're sure
    // you're looking at the correct APPLICATION

    __ASSERT_DEBUG(iTempData, User::Leave(KErrCorrupt));

    switch(aCharacteristic.Type())
        {
        //intentional fall-through
        case KWPNapDef:        
        case KWPPxLogical:    
            iTempData->iNapDef = &aCharacteristic;          
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::ContextExtension
// -----------------------------------------------------------------------------
//
TInt CStreamingAdapter::ContextExtension(MWPContextExtension*& aExtension)
    {
    aExtension = this;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::SaveDataL
// -----------------------------------------------------------------------------
//
const TDesC8& CStreamingAdapter::SaveDataL(TInt /*aIndex*/) const
    {
    return iSavedID;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::DeleteL
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::DeleteL(const TDesC8& aSaveData)
    {
    TUint32 id;
    
    InitSettingsModelL();
    
    iModel->GetDefaultAp(id);

    TPckgBuf<TUint32> pckg;
    pckg.Copy(aSaveData);

    if (id == pckg())
        {
        iModel->LoadSettingsL(EConfigDefault);
        iModel->StoreSettingsL();
        }
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::Uid
// -----------------------------------------------------------------------------
//
TUint32 CStreamingAdapter::Uid() const
    {
    return iDtor_ID_Key.iUid;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::IsValid
// -----------------------------------------------------------------------------
//
TBool CStreamingAdapter::IsValid()
    {
    TBool validity(EFalse);
    
    if (iTempData->iNapDef)
        {
        // Validity checks for min and max UDP port values
        if (iTempData->iMinUdpPort < KMinUDPPort)
            {
            iTempData->iMinUdpPort = KDefaultMinUDPPort; 
            }

        if (iTempData->iMaxUdpPort > KMaxUDPPort)
            {
            iTempData->iMaxUdpPort = KDefaultMaxUDPPort; 
            }

        if (iTempData->iMinUdpPort > iTempData->iMaxUdpPort)
            {
            iTempData->iMinUdpPort = KInvalidValue;
            iTempData->iMaxUdpPort = KInvalidValue;
            }
        
        // Validity check for max bandwidth value
        switch (iTempData->iMaxBw)
            {
            case KBandwidthValue1:
            case KBandwidthValue2:
            case KBandwidthValue3:
            case KBandwidthValue4:
            case KBandwidthValue5:
            case KBandwidthValue6:
                // iMaxBw is valid
                break;
            default:
                // iMaxBw is invalid
                iTempData->iMaxBw = KInvalidValue;
                break;
            }

        validity = ETrue;
        }
            
    return validity;
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::ParseIntegerL
// -----------------------------------------------------------------------------
//
TInt CStreamingAdapter::ParseIntegerL(const TDesC& aPtr, TInt& aInt) 
    {
    TLex lex(aPtr);
    return lex.Val(aInt);
    }

// -----------------------------------------------------------------------------
// CStreamingAdapter::Cleanup
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::Cleanup( TAny* aAny )
	{
	RImplInfoPtrArray* implArray = 
		reinterpret_cast< RImplInfoPtrArray*> ( aAny );
	implArray->ResetAndDestroy();
	implArray->Close();
	}

// -----------------------------------------------------------------------------
// CStreamingAdapter::InitSettingsModelL
// -----------------------------------------------------------------------------
//
void CStreamingAdapter::InitSettingsModelL()
	{
	FLOG( _L( "[Provisioning] CStreamingAdapter::InitSettingsModelL:" ) );
	
    if ( !iModel )
        {
        RImplInfoPtrArray impl; 
    	CleanupStack::PushL( TCleanupItem( Cleanup, &impl ) );
    	CMPSettingsModel::ListImplementationsL( impl );
    	if( impl.Count() > 0 )
            {
            FLOG( _L( "[Provisioning] CStreamingAdapter::InitSettingsModelL: Creating CMPSettingsModel" ) );
    	    // using the first available implementation
            iModel= CMPSettingsModel::NewL( impl[0]->ImplementationUid() );
            FLOG( _L( "[Provisioning] CStreamingAdapter::InitSettingsModelL: Creating CMPSettingsModel Done" ) );
        	}
    	CleanupStack::PopAndDestroy(); // implArray
        }
	}
	
//  End of File  
