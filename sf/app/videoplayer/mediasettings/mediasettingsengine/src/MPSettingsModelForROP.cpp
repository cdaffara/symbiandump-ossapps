/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   MMF ROP plugin specific settings model.*
*/


// Version : %version: 10 %




// INCLUDE FILES
#include <mmf/common/mmfbase.h>
#include <MMFROPCustomCommandConstants.h>
#include <f32file.h>
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include <f32file.h>
#include <commdb.h>             // CMDBSession
#include <commsdattypesv1_1.h>  // CCDWAPIPBearerRecord
#include <mpsettingsropmodel.rsg>

#include    <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR

#include    "MPSettingsModelForROP.h"
#include    "MPSettingsRopConfigParser.h"
#include    "MPSettingsROPSettings.hrh"
#include    "MediaPlayerPrivateCRKeys.h"
#include	"MediaPlayerVariant.hrh" 
#include	"mpxlog.h"

// CONSTANTS
// ROP controller UID
const TUid KRopControllerUid = {0x101F8514};
// This should be used only as the granularity for iItems array
const TInt KMPRopItemsArrayGranularity = 8;
// Initial config string length
const TInt KMPRopConfigStringLength = 2048;
// Seconds to milli seconds multiplier
const TInt KMPRopSecondsMultiplier = 1000;
// Unlocalized resource file path
_LIT( KMPSettROPResource, "MPSettingsROPModel.rsc" );


// CLASS DECLARATION

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::CMPSettingsModelForROP
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsModelForROP::CMPSettingsModelForROP()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::CMPSettingsModelForROP()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::ConstructL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::ConstructL()"));
    InitializeCentralRepositoryL();
    iItems = new(ELeave) CArrayPtrSeg<CMPRopSettingItem>(KMPRopItemsArrayGranularity);

    // Connect RFs
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Locate resource file
    TFileName fileName;
    LocateResourceFileL( fileName, fs );
            
    // Open resource file
    RResourceFile resourceFile;
    resourceFile.OpenL(fs, fileName);
    CleanupClosePushL(resourceFile);
    
    // Read array from resource file to a buffer
    resourceFile.ConfirmSignatureL(0); // Magic: dummy value

    HBufC8* buf = resourceFile.AllocReadLC(R_MPSETT_ROP_SETTINGS_ARRAY);

    CMPRopSettingItem* item = NULL;
    TInt id = 0;
    HBufC* key = NULL;

    // Set buffer to resource reader
    TResourceReader reader;
    reader.SetBuffer(buf);
    // Read number of items from the resource structure
    TInt count = reader.ReadInt16();
 
    for (TInt index = 0; index < count; ++index)
        {
        // Read id and key
        id = reader.ReadInt16();
        key = reader.ReadHBufCL();
        CleanupStack::PushL(key);

        // key's ownership is transferred to item
        item = CMPRopSettingItem::NewLC(id, *key);
        iItems->AppendL(item); // Ownership transferred
        CleanupStack::Pop(2); // item & key
        delete key;
        key = NULL;
        }

    CleanupStack::PopAndDestroy(3); // fs, resourceFile & buf

    // Create config parser
    iParser = CMPSettingsRopConfigParser::NewL();

    // Open ROP controller
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = EMdaPriorityNormal;
    prioritySettings.iPref = EMdaPriorityPreferenceNone;
    prioritySettings.iState = EMMFStateIdle;
    User::LeaveIfError(iMMFController.Open(KRopControllerUid,
                                           prioritySettings));
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsModelForROP* CMPSettingsModelForROP::NewL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::NewL()"));
    CMPSettingsModelForROP* self = new(ELeave) CMPSettingsModelForROP;    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
   
// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::~CMPSettingsModelForROP
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsModelForROP::~CMPSettingsModelForROP()
    {  
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::~CMPSettingsModelForROP()"));
	UninitializeCentralRepository();

    if (iItems)
        {
        iItems->ResetAndDestroy();
        delete iItems;
        }

    if (iParser) 
        {
        delete iParser;
        }

    if (iROPSettings) 
        {
        delete iROPSettings;
        }

    if (iROPHeader) 
        {
        delete iROPHeader;
        }

    iMMFController.Close();
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::LoadSettingsL
// -----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::LoadSettingsL(TInt aConfigVersion)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::LoadSettingsL(%d)"),aConfigVersion);
    // Initialize variables
    TInt configLength(KMPRopConfigStringLength);
    TInt error(KErrOverflow);
    const TMMFMessageDestinationPckg destinationPckg(KUidInterfaceMMFROPController);

    TInt ropConfigVersion;
    switch (aConfigVersion)
        {
        case EConfigDefault:
            ropConfigVersion = KMMFAudioVideoConfigDefault;
            break;
        case EConfigUser:
        default:
            ropConfigVersion = KMMFAudioVideoConfigUser;
            break;
        }

    const TPckgBuf<TInt> verPckg(ropConfigVersion);


    // If iROPSettings is not large enough, increase the length and try again 
    for (TInt i = 0; i < 2 && error == KErrOverflow; ++i)
        {
        delete iROPSettings;
        iROPSettings = NULL;
        iROPSettings = HBufC8::NewL(configLength);
        TPtr8 ptr = iROPSettings->Des();

        error = iMMFController.CustomCommandSync(destinationPckg, EMMFROPControllerGetApplicationConfig,
            verPckg, KNullDesC8, ptr);

        if (error == KErrOverflow)
            {
            // iROPSettings is not large enough => increase length:
            // extract correct length from iROPSettings
            TPckgBuf<TInt> newLength;
            newLength.Copy(ptr);
            configLength = newLength();
            }
        else 
            {
            // Leave if unexpected error occurred
            User::LeaveIfError(error);
            }               
        }

    // Leave if unexpected error occurred
    User::LeaveIfError(error);

    delete iROPHeader;
    iROPHeader = NULL;
    iROPHeader = iParser->ParseConfigStringL(*iROPSettings, iItems);

    delete iROPSettings;
    iROPSettings = NULL;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::StoreSettingsL
// -----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::StoreSettingsL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::StoreSettingsL()"));
    delete iROPSettings;
    iROPSettings = NULL;
    iROPSettings = iParser->CreateConfigStringL(iItems, *iROPHeader);

    if (iROPSettings)
        {
        const TMMFMessageDestinationPckg destinationPckg(KUidInterfaceMMFROPController);
        const TPckgBuf<TBool> savePckg(ETrue);

        User::LeaveIfError(iMMFController.CustomCommandSync(destinationPckg, EMMFROPControllerSetApplicationConfig,
            *iROPSettings, savePckg));
        }

    // Set iValueChanged status to EFalse for all items
    TInt count = iItems->Count();
    for (TInt i = 0; i < count; ++i)
        {
        iItems->At(i)->iValueChanged = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetVideoContrast
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetVideoContrast(TInt aContrast)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetVideoContrast(%d)"),aContrast);
    return SetIntegerValue(EMPRopContrast, aContrast);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetVideoContrast
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetVideoContrast(TInt& aContrast)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetVideoContrast()"));
    return GetIntegerValue(EMPRopContrast, aContrast);
    }



// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetProxyMode
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetProxyMode(TInt aMode)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetProxyMode(%d)"),aMode);
    return SetIntegerValue(EMPRopProxyMode, aMode);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetProxyMode
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetProxyMode(TInt& aMode)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetProxyMode()"));
    return GetIntegerValue(EMPRopProxyMode, aMode);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetProxyHostNameL
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetProxyHostNameL(const TDesC& aHostName)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetProxyHostNameL(%S)"),&aHostName);
    CMPRopSettingItem* item = NULL;
    TInt error = GetItem(EMPRopProxyHostName, item);

    if (!error)
        {
        delete item->iStringValue;
        item->iStringValue = NULL;
        item->iStringValue = aHostName.AllocL();

        item->iValueChanged = ETrue;
        item->iError = KErrNone;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetProxyHostNameL() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetProxyHostName
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetProxyHostName(TDes& aHostName)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetProxyHostName(%S)"),&aHostName);
    CMPRopSettingItem* item = NULL;
    TInt error = GetItem(EMPRopProxyHostName, item);

    if (!error)
        {
        error = item->iError;
        }

    if (!error)
        {
        // Fail safe: aHostName's length is not exceeded in the copy operation
        aHostName.Copy(item->iStringValue->Left(aHostName.MaxLength()));
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetProxyHostName() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetProxyPort
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetProxyPort(TInt aPort)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetProxyPort(%d)"),aPort);
    return SetIntegerValue(EMPRopProxyPort, aPort);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetProxyPort
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetProxyPort(TInt& aPort)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetProxyPort()"));
    return GetIntegerValue(EMPRopProxyPort, aPort);
    }
        
// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetDefaultAp
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetDefaultAp(TUint32 aApId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetDefaultAp(%d)"),aApId);
    TInt error = KErrNone;

    TUint32 tmp = 0;
    TRAP( error, tmp = IapIdFromWapIdL( aApId ) );
    if ( !error )
        {
        error = SetIntegerValue(EMPRopDefaultAP, tmp );
        }
    
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetDefaultAp() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetDefaultAp
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetDefaultAp(TUint32& aApId)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetDefaultAp()"));
    TInt tmp = 0;
      
	TInt error = GetIntegerValue(EMPRopDefaultAP, tmp);
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetDefaultAp - Got iap id %d"),tmp);
	if ( tmp != 0 )
	    {
		TRAP( error, aApId = WapIdFromIapIdL(static_cast<TUint32>(tmp)) );
	    }
	else {
        aApId = 0;
        }
    
	return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetBandwidthControlMode
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetBandwidthControlMode(TInt /*aMode*/)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::SetBandwidthControlMode()"));
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetBandwidthControlMode
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetBandwidthControlMode(TInt& /*aMode*/)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetBandwidthControlMode()"));
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetMaxBandwidth
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetMaxBandwidth(TInt aMaxBw, TDataBearer aBearer)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsModelForROP::SetMaxBandwidth(%d,%d)"),aMaxBw,aBearer);
    TInt error = KErrNone;

    switch (aBearer)
        {
        case EBearerGPRS:
            error = SetIntegerValue(EMPRopGPRSMaxBw, aMaxBw);
            break;
        case EBearerEGPRS:
            error = SetIntegerValue(EMPRopEGPRSMaxBw, aMaxBw);
            break;
        case EBearerWCDMA:
            error = SetIntegerValue(EMPRopWCDMAMaxBw, aMaxBw);
            break;
        case EBearerWLAN:
            error = SetIntegerValue(EMPRopWLANMaxBw, aMaxBw);
            break;
        case EBearerHSDPA:
            error = SetIntegerValue(EMPRopHSDPAMaxBw, aMaxBw);
            break;
        default:
            error = KErrNotSupported;
            break;
        }
    
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetMaxBandwidth() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetMaxBandwidth
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetMaxBandwidth(TInt& aMaxBw, TDataBearer aBearer)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetMaxBandwidth()"));
    TInt error = KErrNone;
    
    switch (aBearer)
        {
        case EBearerGPRS:
            error = GetIntegerValue(EMPRopGPRSMaxBw, aMaxBw);
            break;
        case EBearerEGPRS:
            error = GetIntegerValue(EMPRopEGPRSMaxBw, aMaxBw);
            break;
        case EBearerWCDMA:
            error = GetIntegerValue(EMPRopWCDMAMaxBw, aMaxBw);
            break;
        case EBearerWLAN:
            error = GetIntegerValue(EMPRopWLANMaxBw, aMaxBw);
            break;
        case EBearerHSDPA:
            error = GetIntegerValue(EMPRopHSDPAMaxBw, aMaxBw);
            break;
        default:
            error = KErrNotSupported;
            break;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetMaxBandwidth() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetConnectionTimeout
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetConnectionTimeout(TInt aTimeout)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetConnectionTimeout(%d)"),aTimeout);
    return SetIntegerValue(EMPRopConnTimeout, aTimeout * KMPRopSecondsMultiplier);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetConnectionTimeout
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetConnectionTimeout(TInt& aTimeout)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetConnectionTimeout()"));
    TInt error = GetIntegerValue(EMPRopConnTimeout, aTimeout);
    if (!error)
        {
        aTimeout = aTimeout / KMPRopSecondsMultiplier;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetConnectionTimeout() ret %d"),error);
    return error;
    }
        
// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetServerTimeout
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetServerTimeout(TInt aTimeout)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetServerTimeout(%d)"),aTimeout);
    return SetIntegerValue(EMPRopServerTimeout, aTimeout * KMPRopSecondsMultiplier);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetServerTimeout
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetServerTimeout(TInt& aTimeout)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetServerTimeout()"));
    TInt error = GetIntegerValue(EMPRopServerTimeout, aTimeout);
    if (!error)
        {
        aTimeout = aTimeout / KMPRopSecondsMultiplier;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetServerTimeout() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetMinUDPPort
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetMinUDPPort(TInt aPort)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetMinUDPPort(%d)"),aPort);
    return SetIntegerValue(EMPRopMinUDPPort, aPort);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetMinUDPPort
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetMinUDPPort(TInt& aPort)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetMinUDPPort()"));
    return GetIntegerValue(EMPRopMinUDPPort, aPort);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetMaxUDPPort
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetMaxUDPPort(TInt aPort)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetMaxUDPPort(%d)"),aPort);
    return SetIntegerValue(EMPRopMaxUDPPort, aPort);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetMaxUDPPort
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetMaxUDPPort(TInt& aPort)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetMaxUDPPort()"));
    return GetIntegerValue(EMPRopMaxUDPPort, aPort);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetControllerVersionInfo
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetControllerVersionInfo(TDes& aVersion)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetControllerVersionInfo(%S)"),&aVersion);
    return GetStringValue(EMPRopCntrlVersion, aVersion);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetControllerBuildDate
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetControllerBuildDate(TDes& aBldDate)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetControllerBuildDate(%S)"),&aBldDate);
    return GetStringValue(EMPRopCntrlBldDate, aBldDate);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetControllerAdditionalInfo
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetControllerAdditionalInfo(TDes& aAdditionalInfo)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetControllerAdditionalInfo(%S)"),&aAdditionalInfo);
    return GetStringValue(EMPRopCntrlPlatform, aAdditionalInfo);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetDemandBwFactor
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetDemandBwFactor(TInt aFactor)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetDemandBwFactor(%d)"),aFactor);
    return SetIntegerValue(EMPRopDemandBwFactor, aFactor);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetDemandBwFactor
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetDemandBwFactor(TInt& aFactor)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetDemandBwFactor()"));
    return GetIntegerValue(EMPRopDemandBwFactor, aFactor);
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetSustainBandwidth
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetSustainBandwidth(TInt aSustainBw, TDataBearer aBearer)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsModelForROP::SetSustainBandwidth(%d,%d)"),aSustainBw,aBearer);
    TInt error = KErrNone;

    switch (aBearer)
        {
        case EBearerGPRS:
            error = SetIntegerValue(EMPRopGPRSSustainBw, aSustainBw);
            break;
        case EBearerEGPRS:
            error = SetIntegerValue(EMPRopEGPRSSustainBw, aSustainBw);
            break;
        case EBearerWCDMA:
            error = SetIntegerValue(EMPRopWCDMASustainBw, aSustainBw);
            break;
        case EBearerWLAN:
            error = SetIntegerValue(EMPRopWLANSustainBw, aSustainBw);
            break;
        case EBearerHSDPA:
            error = SetIntegerValue(EMPRopHSDPASustainBw, aSustainBw);
            break;
        default:
            error = KErrNotSupported;
            break;
        }
    
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetSustainBandwidth() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetSustainBandwidth
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetSustainBandwidth(TInt& aSustainBw, TDataBearer aBearer)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetSustainBandwidth()"));
    TInt error = KErrNone;
    
    switch (aBearer)
        {
        case EBearerGPRS:
            error = GetIntegerValue(EMPRopGPRSSustainBw, aSustainBw);
            break;
        case EBearerEGPRS:
            error = GetIntegerValue(EMPRopEGPRSSustainBw, aSustainBw);
            break;
        case EBearerWCDMA:
            error = GetIntegerValue(EMPRopWCDMASustainBw, aSustainBw);
            break;
        case EBearerWLAN:
            error = GetIntegerValue(EMPRopWLANSustainBw, aSustainBw);
            break;
        case EBearerHSDPA:
            error = GetIntegerValue(EMPRopHSDPASustainBw, aSustainBw);
            break;
        default:
            error = KErrNotSupported;
            break;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetSustainBandwidth() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetSustainBwPresetsL
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetSustainBwPresetsL(RArray<TInt>& aBwArray, TDataBearer aBearer)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetSustainBwPresetsL()"));
    TInt error = KErrNone;
    
    switch (aBearer)
        {
        case EBearerGPRS:
            error = GetArrayL(EMPRopGPRSSustainBwPresets, aBwArray);
            break;
        case EBearerEGPRS:
            error = GetArrayL(EMPRopEGPRSSustainBwPresets, aBwArray);
            break;
        case EBearerWCDMA:
            error = GetArrayL(EMPRopWCDMASustainBwPresets, aBwArray);
            break;
        case EBearerWLAN:
            error = GetArrayL(EMPRopWLANSustainBwPresets, aBwArray);
            break;
        case EBearerHSDPA:
            error = GetArrayL(EMPRopHSDPASustainBwPresets, aBwArray);
            break;
        default:
            error = KErrNotSupported;
            break;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetSustainBwPresetsL() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetMaxBwPresetsL
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetMaxBwPresetsL(RArray<TInt>& aBwArray, TDataBearer aBearer)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetMaxBwPresetsL()"));
    TInt error = KErrNone;
    
    switch (aBearer)
        {
        case EBearerGPRS:
            error = GetArrayL(EMPRopGPRSMaxBwPresets, aBwArray);
            break;
        case EBearerEGPRS:
            error = GetArrayL(EMPRopEGPRSMaxBwPresets, aBwArray);
            break;
        case EBearerWCDMA:
            error = GetArrayL(EMPRopWCDMAMaxBwPresets, aBwArray);
            break;
        case EBearerWLAN:
            error = GetArrayL(EMPRopWLANMaxBwPresets, aBwArray);
            break;
        case EBearerHSDPA:
            error = GetArrayL(EMPRopHSDPAMaxBwPresets, aBwArray);
            break;
        default:
            error = KErrNotSupported;
            break;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetMaxBwPresetsL() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetIntegerValue
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::SetIntegerValue(TInt aId, TInt aValue)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsModelForROP::SetIntegerValue(%d,%d)"),aId,aValue);
    CMPRopSettingItem* item = NULL;
    TInt error = GetItem(aId, item);

    if (!error)
        {
        item->iIntValue = aValue;
        item->iValueChanged = ETrue;
        item->iError = KErrNone; 
        }
   
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetIntegerValue() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetIntegerValue
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetIntegerValue(TInt aId, TInt& aValue)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetIntegerValue(%d)"),aId);
    CMPRopSettingItem* item = NULL;
    TInt error = GetItem(aId, item);
    
    if (!error)
        {
        error = item->iError;
        }

    if (!error)
        {
        if (item->iType == EMPRopConfTypeInteger)
            {
            aValue = item->iIntValue;
            }
        else
            {
            // Setting value's type is not integer
            error = KErrNotFound;
            }
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetIntegerValue() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetStringValue
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetStringValue(TInt aId, TDes& aString)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsModelForROP::GetStringValue(%d,%S)"),aId,&aString);
    CMPRopSettingItem* item = NULL;
    TInt error = GetItem(aId, item);
    
    if (!error)
        {
        error = item->iError;
        }
    
    if (!error)
        {
        if (item->iType == EMPRopConfTypeString)
            {
            HBufC* string = item->iStringValue;

            if (aString.MaxLength() >= string->Length())
                {
                aString.Copy(*string);    
                }
            else
                {
                error = KErrOverflow;
                }
            }
        else
            {
           // Setting value's type is not string
            error = KErrNotFound;
            }
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetStringValue() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetArrayL
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetArrayL(TInt aId, RArray<TInt>& aArray)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::GetArrayL()"));
    CMPRopSettingItem* item = NULL;
    TInt error = GetItem(aId, item);
    
    if (!error)
        {
        error = item->iError;
        }

    if (!error)
        {
        if (item->iType == EMPRopConfTypeIntArray)
            {
            TInt count = item->iIntArray.Count();
            aArray.Reset();

            for (TInt index = 0; index < count; ++index)
                {
                User::LeaveIfError(aArray.Append(item->iIntArray[index]));
                }
            }
        else
            {
            error = KErrNotFound;
            }
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetArrayL() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::GetItem
// -----------------------------------------------------------------------------
//
TInt CMPSettingsModelForROP::GetItem(TInt aId, CMPRopSettingItem*& aItem)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetItem(%d)"),aId);
    CMPRopSettingItem* item = NULL;
    TInt count = iItems->Count();
    TInt error = KErrNone;
    aItem = NULL;

    for (TInt index = 0; index < count; ++index)
        {
        item = iItems->At(index);

        if (aId == item->iId)
            {
            aItem = item;
            break;
            }
        }

    if (!aItem)
        {
        error = KErrNotFound;
        }

    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::GetItem() ret %d"),error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetAutoDisconTimeL
// -----------------------------------------------------------------------------
//

void CMPSettingsModelForROP::SetAutoDisconTimeL( const TInt aTime )
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetAutoDisconTimeL(%d)"),aTime);
    if ( iClient->Set( KMPAutoDisconnectTime, aTime ) == KErrNotFound )
        {
        User::LeaveIfError( iClient->Create( KMPAutoDisconnectTime, aTime ) );   
        }       
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::AutoDisconTimeL
// -----------------------------------------------------------------------------
//

TInt CMPSettingsModelForROP::AutoDisconTimeL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::AutoDisconTimeL()"));
    TInt time = 0;
    User::LeaveIfError( iClient->Get( KMPAutoDisconnectTime, time ) );   
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::AutoDisconTimeL() ret %d"),time);
    return time;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetVideoRepeatL
// -----------------------------------------------------------------------------
//

void CMPSettingsModelForROP::SetVideoRepeatL( TBool aRepeat )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetVideoRepeatL(%d)"),aRepeat);
    iClient->Set( KMPRepeat, aRepeat );  
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::IsVideoRepeatOnL
// -----------------------------------------------------------------------------
//

TBool CMPSettingsModelForROP::IsVideoRepeatOnL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::IsVideoRepeatOnL()"));
    TInt repeat;
    User::LeaveIfError( iClient->Get( KMPRepeat, repeat ) );   
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::IsVideoRepeatOnL() ret %d"),repeat);
    return repeat;
    }

// ----------------------------------------------------------------------------
// CMPSettingsModelForROP::InitializeCentralRepositoryL
//
// Creating and setting keys for the Central Repository
// ----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::InitializeCentralRepositoryL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::InitializeCentralRepositoryL()"));
    iClient = CRepository::NewL( KCRUidMediaPlayerSettings );
    iMediaPlayerClient = CRepository::NewL( KCRUidMediaPlayerFeatures );
    }

// ----------------------------------------------------------------------------
// CMPSettingsModelForROP::UninitializeCentralRepositoryL
//
// Removes Central Repository objects
// ----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::UninitializeCentralRepository()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::UninitializeCentralRepository()"));
    delete iClient;
    iClient = NULL;

	delete iMediaPlayerClient;
	iMediaPlayerClient = NULL;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetDefaultViewL
// -----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::SetDefaultViewL( TBool aView )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::SetDefaultViewL(%d)"),aView);
    iClient->Set( KMPDefaultView, aView );  
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::IsDefaultViewOnL
// -----------------------------------------------------------------------------
//
TBool CMPSettingsModelForROP::IsDefaultViewOnL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::IsDefaultViewOnL()"));
    TInt view;
	User::LeaveIfError( iClient->Get( KMPDefaultView, view ) );   
	MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::IsDefaultViewOnL() ret %d"),view);
    return view;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::SetRockerKeysL
// -----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::SetRockerKeysL( TBool aRockerKeys )
    {
    iClient->Set( KMPRockerKeys, aRockerKeys );  
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::ShowRockerKeysL
// -----------------------------------------------------------------------------
//
TBool CMPSettingsModelForROP::ShowRockerKeysL()
    {
	if (IsRockerKeysSupportedL())
		{
		TInt rockerKeys;
		User::LeaveIfError( iClient->Get( KMPRockerKeys, rockerKeys ) );   
		return rockerKeys;
		}
	else
		{
		return EFalse;
		}
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::IsRockerKeysSupportedL
// -----------------------------------------------------------------------------
//
TBool CMPSettingsModelForROP::IsRockerKeysSupportedL()
    {
	TBool isRockerKeysSupported( EFalse );
    TInt value( 0 );
    // Check if feature is enabled. See MediaPlayerVariant.hrh.
    if ( !iMediaPlayerClient->Get( KMPLocalVariation, value ) )
		{
        isRockerKeysSupported = static_cast<TBool> ( value & KMediaPlayerRockerKeys );
		}

	RDebug::Print(_L("#MP# CMPSettingsModelForROP::CMPSettingsModelForROP(): return %d"),isRockerKeysSupported);
    return isRockerKeysSupported;
    }

// -----------------------------------------------------------------------------
// CMPSettingsModelForROP::LocateResourceFileL
// -----------------------------------------------------------------------------
//
void CMPSettingsModelForROP::LocateResourceFileL( TFileName& aFileName, RFs& aFs )
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::LocateResourceFileL()"));
    
    // Get the path & file name with the drive not specified.
    TParse parse;
    parse.Set( KMPSettROPResource, &KDC_RESOURCE_FILES_DIR, NULL );
    TPtrC rscFile = parse.FullName();

    // This is done to ensure upgraded file is used first.
    TFindFile find( aFs );
    TInt err = find.FindByDir( rscFile, KNullDesC );
    
    if ( err )
        {
        MPX_DEBUG1(_L("#MS# CMPSettingsModelForROP::LocateResourceFileL() LEAVE: KErrNotFound"));
        User::Leave( KErrNotFound );
        }
    else
        {
        aFileName.Append( find.File() );
        }
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::LocateResourceFileL(%S)"),&aFileName);
    }

// -----------------------------------------------------------------------------
// CVcxConnUtilImpl::WapIdFromIapIdL
// -----------------------------------------------------------------------------
//
TUint32 CMPSettingsModelForROP::WapIdFromIapIdL( TUint32 aIapId )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::WapIdFromIapIdL(%d)"),aIapId);
    
    TUint32 wap = 0;
#ifdef __WINSCW__    
    wap = aIapId;
#else    
    
    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );
    
    // WapIpBearer table contains the mapping between wap and iap id's.
    CCDWAPIPBearerRecord* wapBearerRecord = 
        static_cast<CCDWAPIPBearerRecord*>( CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
        
    CleanupStack::PushL( wapBearerRecord );
    
    wapBearerRecord->iWAPIAP = aIapId;
    
    TBool found = wapBearerRecord->FindL( *db );
    if ( !found )
        {
        User::Leave(KErrNotFound);
        }

    wap = static_cast<TUint32>( wapBearerRecord->iWAPAccessPointId );
    
    CleanupStack::PopAndDestroy( wapBearerRecord );
    CleanupStack::PopAndDestroy( db );

#endif    
    
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::IapIdFromWapIdL() - return wap id: %d "), wap);
    return wap;
    }

// -----------------------------------------------------------------------------
// CVcxConnUtilImpl::IapIdFromWapIdL
// -----------------------------------------------------------------------------
//
TUint32 CMPSettingsModelForROP::IapIdFromWapIdL( TUint32 aWapId )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::IapIdFromWapIdL(%d)"),aWapId);
    
    TUint32 iap = 0;
#ifdef __WINSCW__
    iap = aWapId;
#else    
    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );
    
    // WapIpBearer table contains the mapping between wap and iap id's.
    CCDWAPIPBearerRecord* wapBearerRecord = 
        static_cast<CCDWAPIPBearerRecord*>( CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
        
    CleanupStack::PushL( wapBearerRecord );
    
    wapBearerRecord->iWAPAccessPointId = aWapId;
    
    TBool found = wapBearerRecord->FindL( *db );
    
    if ( !found )
        {
        User::Leave(KErrNotFound);
        }

    iap = static_cast<TUint32>( wapBearerRecord->iWAPIAP );
    
    CleanupStack::PopAndDestroy( wapBearerRecord );
    CleanupStack::PopAndDestroy( db );

#endif
    
    MPX_DEBUG2(_L("#MS# CMPSettingsModelForROP::IapIdFromWapIdL() - return iap id: %d "), iap);
    return iap;
    }

//  End of File  
