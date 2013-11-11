/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "MemSpyViewSystemConfig.h"

// System includes
#include <hal.h>
#include <bautils.h>
#include <eikenv.h>
#ifdef __EPOC32__
#include <e32rom.h>
#endif

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperfilesystem.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"

// Literal constants
_LIT( KMemSpyItemValueNotKnown, "Unknown" );



CMemSpyViewSystemConfig::CMemSpyViewSystemConfig( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


CMemSpyViewSystemConfig::~CMemSpyViewSystemConfig()
    {
    delete iModel;
    }


void CMemSpyViewSystemConfig::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "System Configuration" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewSystemConfig::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewSystemConfig::ViewType() const
    {
    return EMemSpyViewTypeSystemConfig;
    }


CMemSpyViewBase* CMemSpyViewSystemConfig::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewSystemConfig::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    return child;
    }


void CMemSpyViewSystemConfig::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    delete iModel;
    iModel = model;

    TBuf<128> valueBuf;
    TInt value = 0;
    TInt value2 = 0;
    TMemSpySizeText sizeBuf;

    //
    _LIT(KItem1Format, "Manufacturer");
    GetManufacturer( valueBuf );
    AddItemL( KItem1Format, valueBuf );

    //
    _LIT(KItem2Format, "Machine Uid");
    GetMachineUid( valueBuf );
    AddItemL( KItem2Format, valueBuf );

    //
    _LIT(KItem1aFormat, "Model Uid");
    GetHALValueAsHexL( HALData::EModel, KItem1aFormat );

    //
    _LIT(KItem2aFormat, "Device Family");
    GetDeviceFamily( valueBuf );
    AddItemL( KItem2aFormat, valueBuf );

    //
    _LIT(KItem2bFormat, "Device Family Rev.");
    GetHALValueAsNumericL( HALData::EDeviceFamilyRev, KItem2bFormat );

    //
    _LIT(KItem3Format, "Hardware Rev.");
    GetHALValueAsNumericL( HALData::EManufacturerHardwareRev, KItem3Format );

    //
    _LIT(KItem4Format, "Software Rev.");
    GetHALValueAsNumericL( HALData::EManufacturerSoftwareRev, KItem4Format );

    //
    _LIT(KItem5Format, "Software Build");
    GetHALValueAsNumericL( HALData::EManufacturerSoftwareBuild, KItem5Format );

    //
    _LIT(KItem6Format, "CPU");
    GetCPU( valueBuf );
    AddItemL( KItem6Format, valueBuf );

    //
    _LIT(KItem7Format, "CPU ABI");
    GetCPUABI( valueBuf );
    AddItemL( KItem7Format, valueBuf );

    //
    _LIT(KItem8Format, "CPU Speed");
    _LIT(KItem8Suffix, "KHz");
    GetHALValueAsNumericL( HALData::ECPUSpeed, KItem8Format, &KItem8Suffix );

    //
    _LIT(KItem8aFormat, "Floating Point Support");
    GetHALValueAsYesNoL( HALData::EHardwareFloatingPoint, KItem8aFormat );

    //
    _LIT(KItem8bFormat, "System Tick Period");
    _LIT(KItem8bSuffix, "ms");
    GetHALValueAsNumericL( HALData::ESystemTickPeriod, KItem8bFormat, &KItem8bSuffix );

    //
    _LIT(KItem8cFormat, "Nano Tick Period");
    _LIT(KItem8cSuffix, "us");
    GetHALValueAsNumericL( HALData::ENanoTickPeriod, KItem8cFormat, &KItem8cSuffix );

    //
    _LIT(KItem9Format, "Startup Reason");
    GetStartupReason( valueBuf );
    AddItemL( KItem9Format, valueBuf );

    //
    _LIT(KItem10Format, "Language");
    GetHALValueAsNumericL( HALData::ELanguageIndex, KItem10Format, NULL, 4 );

    //
    _LIT(KItem10aFormat, "Locale");
    GetHALValueAsNumericL( HALData::ELocaleLoaded, KItem10aFormat, NULL, 4 );

    //
    _LIT(KItem11aFormat, "Clipboard Drive");
    GetHALValueAsDriveLetterL( HALData::EClipboardDrive, KItem11aFormat );

    //
    _LIT(KItem11bFormat, "System Drive (Hal)");
    GetHALValueAsDriveLetterL( HALData::ESystemDrive, KItem11bFormat );

    //
    _LIT(KItem11cFormat, "System Drive (F32)");
    TDriveNumber systemDrive = CMemSpyEngineHelperFileSystem::GetSystemDrive();
    AddItemL( systemDrive, KItem11cFormat );

    //
    _LIT(KItem11dFormat, "System Drive (Bafl)");
    value = BaflUtils::GetSystemDrive( systemDrive );
    if ( value == KErrNone )
        {
        AddItemL( systemDrive, KItem11dFormat );
        }
    else
        {
        MemSpyUiUtils::GetErrorText( valueBuf, value );
        AddItemL( KItem11dFormat, valueBuf );
        }

    //
    _LIT(KItem12Format, "Display Type");
    GetDisplayType( valueBuf );
    AddItemL( KItem12Format, valueBuf );
    //
    _LIT(KItem12aCaption, "Display Size");
    _LIT(KItem12aFormat, "%d x %d");
    value = value2 = 0;
    GetHALValue( HALData::EDisplayXPixels, value );
    GetHALValue( HALData::EDisplayYPixels, value2 );
    valueBuf.Format( KItem12aFormat, value, value2 );
    AddItemL( KItem12aCaption, valueBuf );

    //
    _LIT(KItem13Format, "Display Depth");
    _LIT(KItem13Suffix, "bpp");
    GetHALValueAsNumericL( HALData::EDisplayBitsPerPixel, KItem13Format, &KItem13Suffix );

    //
    _LIT(KItem14Format, "Display Mode Count");
    GetHALValueAsNumericL( HALData::EDisplayNumModes, KItem14Format );

    //
    _LIT(KItem14aFormat, "Current Mode");
    GetHALValueAsNumericL( HALData::EDisplayMode, KItem14aFormat );

    //
    _LIT(KItem15Format, "Display Address");
    GetHALValueAsHexL( HALData::EDisplayMemoryAddress, KItem15Format );

    //
    _LIT(KItem16Format, "Screen Count");
    GetHALValueAsNumericL( HALData::EDisplayNumberOfScreens, KItem16Format );

    //
    _LIT(KItem17Format, "Eikon Def. Disp. Mode");
    GetDisplayMode( valueBuf, static_cast<TMemSpyDisplayMode>( CEikonEnv::Static()->DefaultDisplayMode() ) );
    AddItemL( KItem17Format, valueBuf );

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iModel = NULL;
    }


void CMemSpyViewSystemConfig::GetManufacturer( TDes& aBuf )
    {
    TInt value = KErrGeneral;
    GetHALValue( HALData::EManufacturer, value );
    //
    switch( value )
        {
    case HALData::EManufacturer_Ericsson:
        {
        _LIT( KName, "Ericsson" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Motorola:
        {
        _LIT( KName, "Motorola" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Nokia:
        {
        _LIT( KName, "Nokia" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Panasonic:
        {
        _LIT( KName, "Panasonic" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Psion:
        {
        _LIT( KName, "Psion" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Intel:
        {
        _LIT( KName, "Intel" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Cogent:
        {
        _LIT( KName, "Cogent" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Cirrus:
        {
        _LIT( KName, "Cirrus" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_Linkup:
        {
        _LIT( KName, "Linkup" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EManufacturer_TexasInstruments:
        {
        _LIT( KName, "Texas Instruments" );
        aBuf.Copy( KName );
        }
        break;
    default:
        aBuf.Copy( KMemSpyItemValueNotKnown );
        break;
        }
    }


void CMemSpyViewSystemConfig::GetDisplayMode( TDes& aBuf, TMemSpyDisplayMode aMode )
    {
    switch( aMode )
        {
    case ENone:
        {
        _LIT( KName, "ENone" );
        aBuf.Copy( KName );
        }
    case EGray2:
        {
        _LIT( KName, "EGray2" );
        aBuf.Copy( KName );
        }
    case EGray4:
        {
        _LIT( KName, "EGray4" );
        aBuf.Copy( KName );
        }
    case EGray16:
        {
        _LIT( KName, "EGray16" );
        aBuf.Copy( KName );
        }
    case EGray256:
        {
        _LIT( KName, "EGray256" );
        aBuf.Copy( KName );
        }
    case EColor16:
        {
        _LIT( KName, "EColor16" );
        aBuf.Copy( KName );
        }
    case EColor256:
        {
        _LIT( KName, "EColor256" );
        aBuf.Copy( KName );
        }
    case EColor64K:
        {
        _LIT( KName, "EColor64K" );
        aBuf.Copy( KName );
        }
    case EColor16M:
        {
        _LIT( KName, "EColor16M" );
        aBuf.Copy( KName );
        }
    case ERgb:
        {
        _LIT( KName, "ERgb" );
        aBuf.Copy( KName );
        }
        break;
    case EColor4K:
        {
        _LIT( KName, "EColor4K" );
        aBuf.Copy( KName );
        }
        break;
    case EColor16MU:
        {
        _LIT( KName, "EColor16MU" );
        aBuf.Copy( KName );
        }
        break;
    case EColor16MA:
        {
        _LIT( KName, "EColor16MA" );
        aBuf.Copy( KName );
        }
        break;
    case EColor16MAP:
        {
        _LIT( KName, "EColor16MAP" );
        aBuf.Copy( KName );
        }
        break;
    default:
        aBuf.Copy( KMemSpyItemValueNotKnown );
        break;
        }
    }


void CMemSpyViewSystemConfig::GetDeviceFamily( TDes& aBuf )
    {
    TInt value = KErrGeneral;
    GetHALValue( HALData::EDeviceFamily, value );
    //
    switch( value )
        {
    case HALData::EDeviceFamily_Crystal:
        {
        _LIT( KName, "Crystal" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EDeviceFamily_Pearl:
        {
        _LIT( KName, "Pearl" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EDeviceFamily_Quartz:
        {
        _LIT( KName, "Quartz" );
        aBuf.Copy( KName );
        }
        break;
    default:
        aBuf.Copy( KMemSpyItemValueNotKnown );
        break;
        }
    }


void CMemSpyViewSystemConfig::GetCPU( TDes& aBuf )
    {
    TInt value = KErrGeneral;
    GetHALValue( HALData::ECPU, value );
    //
    switch( value )
        {
    case HALData::ECPU_ARM:
        {
        _LIT( KName, "ARM" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPU_MCORE:
        {
        _LIT( KName, "mCORE" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPU_X86:
        {
        _LIT( KName, "X86" );
        aBuf.Copy( KName );
        }
        break;
    default:
        aBuf.Copy( KMemSpyItemValueNotKnown );
        break;
        }
    }


void CMemSpyViewSystemConfig::GetCPUABI( TDes& aBuf )
    {
    TInt value = KErrGeneral;
    GetHALValue( HALData::ECPUABI, value );
    //
    switch( value )
        {
    case HALData::ECPUABI_ARM4:
        {
        _LIT( KName, "ARM4" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPUABI_ARMI:
        {
        _LIT( KName, "ARMI" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPUABI_THUMB:
        {
        _LIT( KName, "ARM4" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPUABI_MCORE:
        {
        _LIT( KName, "mCORE" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPUABI_MSVC:
        {
        _LIT( KName, "MSVC" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPUABI_ARM5T:
        {
        _LIT( KName, "ARM5T" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ECPUABI_X86:
        {
        _LIT( KName, "X86" );
        aBuf.Copy( KName );
        }
        break;
    default:
        aBuf.Copy( KMemSpyItemValueNotKnown );
        break;
        }
    }


void CMemSpyViewSystemConfig::GetStartupReason( TDes& aBuf )
    {
    TInt value = KErrGeneral;
    GetHALValue( HALData::ESystemStartupReason, value );
    //
    switch( value )
        {
    case HALData::ESystemStartupReason_Cold:
        {
        _LIT( KName, "Cold" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ESystemStartupReason_Warm:
        {
        _LIT( KName, "Warm" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::ESystemStartupReason_Fault:
        {
        _LIT( KName, "Fault" );
        aBuf.Copy( KName );
        }
        break;
    default:
        aBuf.Copy( KMemSpyItemValueNotKnown );
        break;
        }
    }


void CMemSpyViewSystemConfig::GetKeyboard( TDes& aBuf )
    {
    _LIT(KComma, ", ");
    TInt value = KErrGeneral;
    GetHALValue( HALData::EKeyboard, value );
    //
    aBuf.Zero();
    if  ( value & EKeyboard_Keypad )
        {
        _LIT( KName, "Keypad" );
        aBuf.Copy( KName );
        }


    if  ( value & EKeyboard_Full )
        {
        if  ( aBuf.Length() )
            {
            aBuf.Append( KComma );
            }

        _LIT( KName, "Full Keyboard" );
        aBuf.Copy( KName );
        }

    if  ( !aBuf.Length() )
        {
        aBuf.Copy( KMemSpyItemValueNotKnown );
        }
    }


void CMemSpyViewSystemConfig::GetMachineUid( TDes& aBuf )
    {
    TInt value = KErrGeneral;
    GetHALValue( HALData::EMachineUid, value );
    //
    switch( value )
        {
    case HALData::EMachineUid_Series5mx:
        {
        _LIT( KName, "Series 5mx" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_Brutus:
        {
        _LIT( KName, "Brutus" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_Cogent:
        {
        _LIT( KName, "Cogent" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_Win32Emulator:
        {
        _LIT( KName, "Win32 Emulator" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_WinC:
        {
        _LIT( KName, "WINC" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_CL7211_Eval:
        {
        _LIT( KName, "CL7211" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_LinkUp:
        {
        _LIT( KName, "LinkUp" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_Assabet:
        {
        _LIT( KName, "Assabet" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_IQ80310:
        {
        _LIT( KName, "IQ80310" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_Lubbock:
        {
        _LIT( KName, "Lubbock" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_Integrator:
        {
        _LIT( KName, "Integrator" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_Helen:
        {
        _LIT( KName, "Helen" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_X86PC:
        {
        _LIT( KName, "X86PC" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_OmapH2:
        {
        _LIT( KName, "OmapH2" );
        aBuf.Copy( KName );
        }
        break;
    case HALData::EMachineUid_OmapH4:
        {
        _LIT( KName, "OmapH4" );
        aBuf.Copy( KName );
        }
        break;
    default:
        {
        _LIT( KName, "0x%08x" );
        aBuf.Format( KName, value );
        }
        break;
        }
    }


void CMemSpyViewSystemConfig::GetDisplayType( TDes& aBuf )
    {
    TInt value = KErrGeneral;
    GetHALValue( HALData::EDisplayIsMono, value );
    //
    if  ( value == 0 )
        {
        _LIT( KName, "Colour" );
        aBuf.Copy( KName );
        }
    else if ( value == 1 )
        {
        _LIT( KName, "Mono" );
        aBuf.Copy( KName );
        }
    else
        {
        MemSpyUiUtils::GetErrorText( aBuf, value );
        }
    }


TInt CMemSpyViewSystemConfig::GetHALValue( HALData::TAttribute aAttribute, TInt& aValue )
    {
    aValue = KErrGeneral;
    const TInt error = HAL::Get( aAttribute, aValue );

#ifdef _DEBUG
    if  ( error != KErrNone )
        {
        RDebug::Printf("CMemSpyViewSystemConfig::GetHALValue() - aAttribute: %3d, error: %d, value: %d", aAttribute, error, aValue);
        }
#endif

    return error;
    }


TInt CMemSpyViewSystemConfig::GetHALValueAsNumericL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix, TInt aWidth )
    {
    TBuf<20> valueBuf; 
    TInt value = 0;
    //
    const TInt error = GetHALValue( aAttribute, value );
    if  ( error == KErrNone )
        {
        if  ( aWidth > 0 && aWidth < 12 )
            {
            valueBuf.NumFixedWidthUC( (TUint) value, EDecimal, aWidth );
            }
        else
            {
            valueBuf.Num( value );
            }

        AddItemL( aCaption, valueBuf, aSuffix );
        }
    else
        {
        MemSpyUiUtils::GetErrorText( valueBuf, error );
        AddItemL( aCaption, valueBuf );
        }
    //
    return error;
    }


TInt CMemSpyViewSystemConfig::GetHALValueAsHexL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix )
    {
    TBuf<20> valueBuf; 
    TInt value = 0;
    //
    const TInt error = GetHALValue( aAttribute, value );
    if  ( error == KErrNone )
        {
        MemSpyEngineUtils::FormatHex( valueBuf, value );
        AddItemL( aCaption, valueBuf, aSuffix );
        }
    else
        {
        MemSpyUiUtils::GetErrorText( valueBuf, error );
        AddItemL( aCaption, valueBuf );
        }
    //
    return error;
    }


TInt CMemSpyViewSystemConfig::GetHALValueAsYesNoL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix )
    {
    TBuf<20> valueBuf; 
    TInt value = 0;
    //
    const TInt error = GetHALValue( aAttribute, value );
    if  ( error == KErrNone || error == KErrNotSupported )
        {
        _LIT(KYes, "Yes");
        _LIT(KNo, "No");
        _LIT(KError, "Error: %d");

        if ( error < KErrNone )
            {
            valueBuf.Format( KError, error );
            }
        else if ( value == EFalse )
            {
            valueBuf.Copy( KNo );
            }
        else if ( error == KErrNone )
            {
            valueBuf.Copy( KYes );
            }

        AddItemL( aCaption, valueBuf, aSuffix );
        }
    else
        {
        MemSpyUiUtils::GetErrorText( valueBuf, error );
        AddItemL( aCaption, valueBuf );
        }
    //
    return error;
    }


TInt CMemSpyViewSystemConfig::GetHALValueAsDriveLetterL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix )
    {
    TInt value = 0;
    const TInt error = GetHALValue( aAttribute, value );
    //
    if  ( error == KErrNone && ( value >= EDriveA && value <= EDriveZ ) )
        {
        AddItemL( (TDriveNumber) value, aCaption, aSuffix );
        }
    else
        {
        _LIT( KItemNotSet, "Not Defined");
        AddItemL( aCaption, KItemNotSet );
        }
    //
    return error;
    }


void CMemSpyViewSystemConfig::AddItemL( const TDesC& aCaption, const TDesC& aValue, const TDesC* aSuffix )
    {
    _LIT(KItemFormat, "\t%S\t\t%S");
    //
    TBuf<KMaxFullName> item;
    item.Format( KItemFormat, &aCaption, &aValue );
    if  ( aSuffix ) 
        {
        _LIT(KSpace, " ");
        item.Append( KSpace );
        item.Append( *aSuffix );
        }
    //
    iModel->AppendL( item );
    }


void CMemSpyViewSystemConfig::AddItemL( TDriveNumber aDrive, const TDesC& aCaption, const TDesC* aSuffix )
    {
    TBuf<20> valueBuf; 
    //
    const TDriveUnit drive( aDrive );
    valueBuf.Copy( drive.Name() );
    //
    AddItemL( aCaption, valueBuf, aSuffix );
    }



