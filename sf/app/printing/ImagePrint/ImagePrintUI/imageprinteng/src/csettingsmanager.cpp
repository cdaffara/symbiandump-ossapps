/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <fbs.h>
#include <eikenv.h>
#include <e32std.h>

#include "tprintcapability.h"
#include "csettingsmanager.h"
#include "ctemplatesettings.h"
#include "cprintsettings.h"
#include "ctintcapability.h"
#include "clistcapability.h"
#include "cfloatcapability.h"
#include "ctemplatedata.h"
#include "printcapabilitycodes.h"
#include "settingsconverter.h"
#include "crealfactory.h"
#include "imageprint.h"
#include "cimageprint.h"
#include "imageprintvariant.h"
#include <centralrepository.h>
#include "imageprintprivatecrkeys.h"
#include "mdiscovery.h"
#include "clog.h"

namespace
    {
    const TInt KDefaultPrinterNameMaxLength = 100;
    const TInt KTemplateSize = 16;
    const TInt KTemplates[] = {1554,1552,1565,1555,1501,1553,1551,1558,1502,1504,1564,1506,1509,1512,1513,1516};
    }

#include "clog.h"

// CONSTRUCTION
CSettingsManager* CSettingsManager::NewL(
    CRealFactory* aFactory )
    {
    CSettingsManager* self = CSettingsManager::NewLC(
        aFactory );
    CleanupStack::Pop();    // self

    return self;
    }

CSettingsManager* CSettingsManager::NewLC(
    CRealFactory* aFactory )
    {
    CSettingsManager* self =
        new ( ELeave ) CSettingsManager( aFactory );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Constrcutor
CSettingsManager::CSettingsManager(
    CRealFactory* aFactory ) :
      iFactory( aFactory ),
      iActivePage( 1 )
    {
    }

// Destructor
CSettingsManager::~CSettingsManager()
    {
    delete iPrintSettings;
    delete iTemplateSettings;
    delete iCurrentPrinterName;
    delete iCRSession;
    iNumberOfCopiesArray.Close();
    iNumberOfCopiesCumulativeArray.Close();
    }

// 2nd phase constructor
void CSettingsManager::ConstructL()
    {
    iPrintSettings = new ( ELeave ) CPrintSettings;
    iTemplateSettings = CTemplateSettings::NewL( iFactory );

    iCRSession  = CRepository::NewL( KCRUidImagePrint );
    }

// Clones the current template settings
CTemplateSettings* CSettingsManager::TemplateSettingsL()
    {
    return iTemplateSettings->CloneL();
    }

// Changes active template to a new one
void CSettingsManager::SetActiveTemplateL(
    TUint aUid )
    {
    LOG1( "CSettingsManager::SetActiveTemplateL START || aUid = %d", aUid );
    // Set active template to preview manager, if one available
    iTemplateSettings->iActiveTemplateUid = aUid;

    CListCapability* capab = static_cast<CListCapability*>(
        FetchCapabilityL( EPrintCapabLayout ) );
    capab->iValue = aUid;
    LOG1( "CSettingsManager::SetActiveTemplateL || iImagesOnPage = %d", iImagesOnPage );
    }

// Returns the number of pages
TUint CSettingsManager::NumOfPagesL()
    {
    // Always 1, fix this for the release 2
    // return iFactory->Engine()->GetNumPrintPages();

    TInt num( 1 );
    return num;
    }

// Returns the number of copies
TInt CSettingsManager::NumberOfCopies( RArray<TInt>& aArray )
    {
    TInt err( KErrNone );
    aArray.Reset();
    for( TInt i=0; i<iNumberOfCopiesArray.Count(); ++i )
        {
        err = aArray.Append( iNumberOfCopiesArray[ i ] );
        if( err != KErrNone )
            {
            return err;
            }
        }
    return KErrNone;
    }

TInt CSettingsManager::NumberOfCopies( TInt& aNumberOfCopies, TInt aIndex )
    {
    if( -1 < aIndex && aIndex < iNumberOfCopiesArray.Count() )
        {
        aNumberOfCopies = iNumberOfCopiesArray[ aIndex ];
        return KErrNone;
        }
    else
        {
        return KErrArgument;
        }
    }

// Sets the number of copies
TInt CSettingsManager::SetNumberOfCopiesL( const RArray<TInt>& aArray )
    {
    TInt err( KErrNone );
    TInt numberOfCopiesCumulative( 1 );
    iNumberOfCopiesArray.Close();
    iNumberOfCopiesCumulativeArray.Close();
    for( TInt i=0; i<aArray.Count(); ++i )
        {
        err = iNumberOfCopiesArray.Append( aArray[ i ] );
        if( err != KErrNone )
            {
            return err;
            }
        err = iNumberOfCopiesCumulativeArray.Append( numberOfCopiesCumulative );
        numberOfCopiesCumulative += aArray[ i ];
        if( err != KErrNone )
            {
            return err;
            }
        }
    return err;
    }

TInt CSettingsManager::UpdateNumberOfCopiesToServer()
    {
    return iFactory->Engine()->SetNumberOfCopies( iNumberOfCopiesArray );
    }

// Returns
TBool CSettingsManager::IsMMC()
    {
    TBool retVal = EFalse;
    TInt supportedProtocols = iFactory->DiscoveryIF()->SupportedProtocols();

	if( (supportedProtocols & KImagePrint_PrinterProtocol_DPOF)  && 
    	( iPrinterUid == iMMCUid ))
		{
		retVal = ETrue;
		}
	LOG1( "CSettingsManager::IsMMC: %d ", retVal );
    return retVal;
    }

// Changes active page for drawing a new preview
void CSettingsManager::SetActivePageL(
    TUint aActivePage )
    {
    }

// Returns current active page
TUint CSettingsManager::CurActivePageL()
    {
    TUint num( 1 );
    return num;
    }

// Clones current print settings
CPrintSettings* CSettingsManager::PrinterSettingsL()
    {
    return iPrintSettings->CloneL();
    }

// Sets all settings, not supported at the moment
void CSettingsManager::SetPrinterSettingsL(
    CPrintSettings& /*aSettings*/ )
    {
    User::Leave( KErrNotSupported );
    }

// Sets a single setting
TInt CSettingsManager::SetPrinterSettingL(
    CBaseCapability& aCapab )
    {
    LOG( "CSettingsManager::SetPrinterSettingL START" );
    TInt getSettings( 0 );
    TInt err = iFactory->Engine()->SetJobSettingL(
        aCapab.iUid, aCapab.iValue, getSettings );
    getSettings  = 0;
    User::LeaveIfError( err );

    CBaseCapability* capab = FetchCapabilityL( aCapab.iUid );
    capab->iValue = aCapab.iValue;

#ifdef IMG_PRINT_DYNAMIC_PRINT_SETTINGS
    // Do not initialize if only template was changed
    if( aCapab.iUid != 500 )
        {
        LOG( "CSettingsManager::SetPrinterSettingL || aCapab.iUid != 500" );
        InitCapabilitiesL( iPrinterUid );
        InitTemplateSettingsL();
        }
     else
        {
        LOG( "CSettingsManager::SetPrinterSettingL || aCapab.iUid == 500" );
        SetActiveTemplateL( aCapab.iValue );
        }
#else
     InitCapabilitiesL( iPrinterUid );
     InitTemplateSettingsL();
#endif

    LOG1( "CSettingsManager::SetPrinterSettingL getSettings = %d", getSettings );
    return getSettings;
    }

// Initializes the printer settings
void CSettingsManager::InitSettingsL(
    TInt aPrinterUid,
    TInt aMMCUid )
    {
    LOG1( "CSettingsManager::InitSettingsL aPrinterUid = %d", aPrinterUid );
    LOG1( "CSettingsManager::InitSettingsL aMMCUid     = %d", aMMCUid     );
    
    iMMCUid = aMMCUid;
    InitCapabilitiesL( aPrinterUid );
    InitTemplateSettingsL();
    iPrinterUid = aPrinterUid;
    }

// Inititalizes the capabilities
void CSettingsManager::InitCapabilitiesL(
    TInt aPrinterUid )
    {
    LOG1("CSettingsManager::InitCapabilitiesL Begin: %d", aPrinterUid);
    RArray<TInt> capabIDs;
    CleanupClosePushL( capabIDs );

    // Get list of IDs
    iFactory->Engine()->GetPrinterCapabilityIDsL( aPrinterUid, capabIDs );
    CPrintSettings* printSettings = new ( ELeave ) CPrintSettings;
    CleanupStack::PushL( printSettings );

    TInt i( 0 );
    TInt num( capabIDs.Count() );
    LOG1("CSettingsManager::InitCapabilitiesL number of capabs: %d", num);
    for ( i = 0; i < num; i++ )
        {
        TPrintCapability capab;
        iFactory->Engine()->GetPrinterCapabilityL( aPrinterUid, capabIDs[i], capab );
        capab.iType = TPrintCapability::Enum;

        CBaseCapability* capability = 0;

        // Create capability
        switch ( capab.iType )
            {
            case TPrintCapability::Int:
                capability = SettingsConverter::AddTIntCapabilityLC(
                    capab, i, capabIDs[i] );
                break;

            case TPrintCapability::Enum:
                capability = SettingsConverter::AddListCapabilityLC(
                    capab, i, capabIDs[i], iFactory );
                break;

            case TPrintCapability::Float:
                capability = SettingsConverter::AddFloatCapabilityLC(
                    capab, i, capabIDs[i] );
                break;

            default:
                User::Leave( KErrUnknown );
                break;
            }
        iFactory->Engine()->GetJobSetting( capabIDs[i], capability->iValue );

        // Add the capability to the list
        User::LeaveIfError(
            printSettings->iCapabilities.Append( capability ) );
        CleanupStack::Pop();    // capability
        }

    delete iPrintSettings;
    iPrintSettings = printSettings;

    CleanupStack::Pop();    // printSettings
    CleanupStack::PopAndDestroy();  // capabIDs
    LOG("CSettingsManager::InitCapabilitiesL End");
    }

// Initializes the templates
void CSettingsManager::InitTemplateSettingsL()
    {
    LOG("CSettingsManager::InitTemplateSettingsL Begin");
    CTemplateSettings* settings = CTemplateSettings::NewL( iFactory );
    CleanupStack::PushL( settings );
    TInt templateBuffer = iFactory->SettingsIF()->TemplateUid();

    TInt i( 0 );
    TInt num( iPrintSettings->iCapabilities.Count() );
    for ( i = 0; i < num; i++ )
        {
        if ( iPrintSettings->iCapabilities[i]->iUid == EPrintCapabLayout )
            {
            TInt j( 0 );
            CListCapability* layouts =
                static_cast<CListCapability*>
                        ( iPrintSettings->iCapabilities[i] );
            for ( j = 0; j < layouts->iEnumIDs.Count(); j++ )
                {
                // Create new template data with the icon and Uid value
                CTemplateData* data = CTemplateData::NewL();
                data->iUid = layouts->iEnumIDs[j];
                CleanupStack::PushL( data );
                LOG1( "CSettingsManager::InitTemplateSettingsL, UID: %d", data->iUid );
                User::LeaveIfError( settings->iTemplates.Append( data ) );
                CleanupStack::Pop( data );
                }
            }
        }

    // Sort the templates to "standard" order
    CTemplateSettings* sortsettings = CTemplateSettings::NewL( iFactory );
    CleanupStack::PushL( sortsettings );
    for ( TInt i = 0; i < KTemplateSize; i++ )
        {
        for ( TInt j = 0; j < settings->iTemplates.Count(); j++ )
            {
            if ( KTemplates[i] == settings->iTemplates[j]->iUid )
                {
                CTemplateData* data = CTemplateData::NewL();
                data->iUid = settings->iTemplates[j]->iUid;
                CleanupStack::PushL( data );
                LOG1( "CSettingsManager::InitTemplateSettingsL, UID: %d", data->iUid );
                User::LeaveIfError( sortsettings->iTemplates.Append( data ) );
                CleanupStack::Pop( data );
                break;
                }
            }
        }

    CleanupStack::Pop( sortsettings );
    CleanupStack::PopAndDestroy( settings );
    delete iTemplateSettings;
    iTemplateSettings = sortsettings;

    TBool templateFound = EFalse;
    if ( iTemplateSettings->iTemplates.Count() > 0 )
        {
        for (TInt g = 0; g < iTemplateSettings->iTemplates.Count(); g++) 
        	{
	        if (iTemplateSettings->iTemplates[g]->iUid == templateBuffer)
	        	{
	        		iTemplateSettings->iActiveTemplateUid = templateBuffer;
	        		templateFound = ETrue;
	        	}
        	}
        if (!templateFound){
        iTemplateSettings->iActiveTemplateUid = 
        iTemplateSettings->iTemplates[0]->iUid;
        }
        
        SetActiveTemplateL( iTemplateSettings->iActiveTemplateUid );
        }
    // If there are no templates available, draw the preview using
    // 1/page template
    else
        {
        LOG("CSettingsManager::InitTemplateSettingsL || No templates available!");
        SetActiveTemplateL( EPrintCapabLayout1UpMedium );
        }
    LOG("CSettingsManager::InitTemplateSettingsL End");
    }

// Fetches capability from the list and returns it
CBaseCapability* CSettingsManager::FetchCapabilityL(
    TInt aUid )
    {
    LOG1("CSettingsManager::FetchCapabilityL Begin: %d", aUid);
    CBaseCapability* capab = NULL;
    TInt found( KErrNotFound );

    TInt i( 0 );
    TInt num( iPrintSettings->iCapabilities.Count() );
    for ( i = 0; i < num && found != KErrNone; i++ )
        {
        if ( iPrintSettings->iCapabilities[i]->iUid == aUid )
            {
            capab = iPrintSettings->iCapabilities[i];
            found = KErrNone;
            }
        }

    User::LeaveIfError( found );
    LOG("CSettingsManager::FetchCapabilityL End");
    return capab;
    }

TInt CSettingsManager::TemplateUid()
    {
    LOG1( "CSettingsManager::TemplateUid || iActiveTemplateUid = %d", iTemplateSettings->iActiveTemplateUid );
    return iTemplateSettings->iActiveTemplateUid;
    }

TBool CSettingsManager::UpdatePageNumberL( TInt aCurrentImage )
    {
    TBool retValue = EFalse;
    TUint pageNumber;

    if( iNumberOfCopiesCumulativeArray.Count() > aCurrentImage-1 )
        {
        aCurrentImage = iNumberOfCopiesCumulativeArray[aCurrentImage-1];
        }

    pageNumber = aCurrentImage / iImagesOnPage;
    if ( ( aCurrentImage % iImagesOnPage ) != 0 )
        {
        pageNumber++;
        }

    if ( iActivePage != pageNumber )
        {
        iActivePage = pageNumber;
        retValue = ETrue;
        }
    return retValue;
    }

TInt CSettingsManager::PrinterUid()
    {
    return iPrinterUid;
    }


TBool CSettingsManager::GetUsbState()
    {
    return iUsbConnected;
    }

void CSettingsManager::SetUsbState( TBool aConnected )
    {
    iUsbConnected = aConnected;
    }

TInt CSettingsManager::GetDefaultPrinterProtocol()
    {
    TInt retValue;
    TInt err = iCRSession->Get( KDefaultPrinterProtocol, retValue );

    LOG2("CSettingsManager::GetDefaultPrinterProtocol: %d, err: %d", retValue, err);
    return ( err ) ? err : retValue;
    }


TInt CSettingsManager::SetDefaultPrinterProtocol( TInt aProtocol )
    {
    TInt err = iCRSession->Set( KDefaultPrinterProtocol, aProtocol );
    LOG2("CSettingsManager::SetDefaultPrinterProtocol: %d, err: %d", aProtocol, err);
    return err;
    }

TInt CSettingsManager::GetCurrentPrinterProtocol()
    {
    LOG1("CSettingsManager::GetCurrentPrinterProtocol: %d", iCurrentPrinterProtocol );
    return iCurrentPrinterProtocol;
    }

void CSettingsManager::SetCurrentPrinterProtocol( TInt aProtocol )
    {
    LOG1("CSettingsManager::SetCurrentPrinterProtocol: %d", aProtocol );
    iCurrentPrinterProtocol = aProtocol;
    }

TUint CSettingsManager::GetCurrentPrinterVendor()
    {
    LOG1("CSettingsManager::GetCurrentPrinterVendor() vendor: %d",iVendor);
    return iVendor;
    }

void CSettingsManager::SetCurrentPrinterVendor( TUint aVendor )
    {
    LOG1("CSettingsManager::SetCurrentPrinterVendor() vendor: %d",aVendor);
    iVendor = aVendor;
    }

TInt CSettingsManager::GetDefaultPrintID()
    {
    TInt retValue;
    TInt err = iCRSession->Get( KDefaultPrinterID, retValue );
    LOG2("CSettingsManager::GetDefaultPrintID: get: %d, err: %d", retValue, err);
    return ( err ) ? err : retValue;
    }

TInt CSettingsManager::SetDefaultPrinterID( TInt aId )
    {
    TInt err = iCRSession->Set( KDefaultPrinterID, aId );
    LOG2("CSettingsManager::SetDefaultPrinterID: set: %d, err: %d", aId, err);
    return err;
    }

HBufC* CSettingsManager::GetDefaultPrinterNameLC()
    {
    HBufC* name = HBufC::NewLC( KDefaultPrinterNameMaxLength );
    TPtr ptr( name->Des() );
    TInt err = iCRSession->Get( KDefaultPrinterName, ptr );
    LOG2("CSettingsManager::GetDefaultPrinterNameLC: name: %S, err: %d", &ptr, err);
    return name;
    }

TInt CSettingsManager::SetDefaultPrinterNameL( const TDesC& aPrinterName )
    {
    TInt err = KErrNone;
    HBufC* stringholder = HBufC::NewLC( aPrinterName.Length() );
        *stringholder = aPrinterName;

        // Clear the printer name string from all formatting tabs & numbers
        TPtr ptr = stringholder->Des();
        TInt pos = stringholder->Find( KTabChar );
        if ( pos == 1 ) // 1st possible icon formatting tab is at pos 1
            {
            ptr.Delete( 0, 2 ); // delete the first icon tab and the number previous to it
            pos = stringholder->Find( KTabChar );
            }
        while ( pos != KErrNotFound &&
                pos != stringholder->Length() - 2 )
            {
            ptr.Delete( pos, 1); // delete all middle tabs
            pos = stringholder->Find( KTabChar );
            }
        // 2nd possible iconformatting tab is 2nd last char
        if ( pos != KErrNotFound &&
             pos == stringholder->Length() - 2 )
            {
            // delete the 2nd icon formatting tab and the number next to it
            ptr.Delete( stringholder->Length() - 2, 2 );
            }

        err = iCRSession->Set( KDefaultPrinterName, *stringholder );
        LOG2("CSettingsManager::SetDefaultPrinterNameL: name: %S err: %d", &ptr, err);
        CleanupStack::PopAndDestroy( stringholder );
    return err;
    }

HBufC* CSettingsManager::GetCurrentPrinterName()
    {
    TPtr ptr = iCurrentPrinterName->Des();
    LOG1("CSettingsManager::GetCurrentPrinterName: name: %S",  &ptr );
    return iCurrentPrinterName;
    }

TInt CSettingsManager::SetCurrentPrinterName( const TDesC& aPrinterName )
    {
    if( iCurrentPrinterName )
        {
        delete iCurrentPrinterName;
        iCurrentPrinterName = NULL;
        }
    
    TInt err;
    HBufC* tmpBuf = aPrinterName.Alloc();
    if( !tmpBuf )
        {
        return KErrGeneral;
        }
    else
        {
        err = KErrNone;    
        iCurrentPrinterName = tmpBuf;
        }        
    
    // Clear the printer name string from all formatting tabs & numbers
    TPtr ptr = iCurrentPrinterName->Des();
    TInt pos = iCurrentPrinterName->Find( KTabChar );
    if ( pos == 1 ) // 1st possible icon formatting tab is at pos 1
        {
        ptr.Delete( 0, 2 ); // delete the first icon tab and the number previous to it
        pos = iCurrentPrinterName->Find( KTabChar );
        }
    while ( pos != KErrNotFound &&
            pos != iCurrentPrinterName->Length() - 2 )
        {
        ptr.Delete( pos, 1); // delete all middle tabs
        pos = iCurrentPrinterName->Find( KTabChar );
        }
    // 2nd possible iconformatting tab is 2nd last char
    if ( pos != KErrNotFound &&
         pos == iCurrentPrinterName->Length() - 2 )
        {
        // delete the 2nd icon formatting tab and the number next to it
        ptr.Delete( iCurrentPrinterName->Length() - 2, 2 );
        }

    LOG1("CSettingsManager::SetCurrentPrinterName: name: %S", &ptr );
    return err;
    }
    
HBufC* CSettingsManager::GetCurrentPaperSizeTextL()
    {    
    HBufC* retValue = NULL;
    CListCapability* paperSizeCapab = NULL;
    // Searches the paper size capability from the capability list
    // Search the UID for paper size (= 4)
    TInt counter = iPrintSettings->iCapabilities.Count();
    for( TInt i = 0 ; i < counter ; i++ )
        {

        //Use the searched ListCapability
        if( iPrintSettings->iCapabilities[ i ]->iUid == EPrintCapabPaperSize )
            {
            paperSizeCapab = static_cast< CListCapability* >
                                   ( iPrintSettings->iCapabilities[ i ] );
            }
        }
    
    
    TBool found( EFalse );

    if ( paperSizeCapab )
        {
        const TInt expectedNumberOfStrings( 2 );
        CDesCArrayFlat* strings = 
            new ( ELeave ) CDesCArrayFlat( expectedNumberOfStrings );
        CleanupStack::PushL( strings );

        TInt i( 0 );
        TInt counter = paperSizeCapab->iEnumIDs.Count();
        for ( i = 0; i < counter && !found; i++ )
            {
            if ( paperSizeCapab->iEnumIDs[i] ==
                 paperSizeCapab->iValue )
                {
                retValue = paperSizeCapab->iTexts[i];
                }
            }
        CleanupStack::PopAndDestroy();	// strings
        }
    return retValue;
    }

//  End of File
