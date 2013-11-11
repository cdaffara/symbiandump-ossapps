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

#include "MemSpyViewMemoryTrackingAutoStartConfig.h"

// System includes
#include <AknQueryDialog.h>
#include <charconv.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyengineprocessmemorytracker.h>
#include <memspy/engine/memspyenginehelperprocess.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"

// Literal constants
_LIT8( KMemSpyProcessMemoryTrackerXmlFileMimeType, "text/xml" );
_LIT8( KMemSpyXmlSpec_MasterSection, "memspy_process_memory_tracking" );
_LIT8( KMemSpyXmlSpec_EntryProcess, "process" );
_LIT8( KMemSpyXmlSpec_EntryProcess_SID, "sid" );
_LIT8( KMemSpyXmlSpec_HexPrefix, "0x" );



CMemSpyViewMemoryTrackingAutoStartConfig::CMemSpyViewMemoryTrackingAutoStartConfig( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


CMemSpyViewMemoryTrackingAutoStartConfig::~CMemSpyViewMemoryTrackingAutoStartConfig()
    {
    iProcessUids.Close();
    //
    delete iParser;
    delete iConverter;
    //
    delete iXMLFileNameInstallTime;
    delete iXMLFileNameUserSupplied;
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // We only convert from UTF-8 to UTF-16
	iParser = CParser::NewL( KMemSpyProcessMemoryTrackerXmlFileMimeType, *this );
    iConverter = CCnvCharacterSetConverter::NewL();
    if  ( iConverter->PrepareToConvertToOrFromL( KCharacterSetIdentifierUtf8, iEngine.FsSession() ) == CCnvCharacterSetConverter::ENotAvailable )
	    {
		User::Leave(KErrNotFound);
		}

    // Search for standard files
    FindXmlInstallTimeL();
    FindXmlUserSuppliedL();

    // Copy items to our own temporary list...
    const RArray<TUid>& list = iEngine.HelperProcess().MemoryTrackingAutoStartProcessList();
    for( TInt i=0; i<list.Count(); i++ )
        {
        User::LeaveIfError( iProcessUids.Append( list[ i ] ) );
        }

    // Reset the title
    _LIT( KCustomTitle, "Process-Specific Memory Tracking" );
    SetTitleL( KCustomTitle );
    }


TMemSpyViewType CMemSpyViewMemoryTrackingAutoStartConfig::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemMemoryTrackingAutoStartConfig;
    }


CMemSpyViewBase* CMemSpyViewMemoryTrackingAutoStartConfig::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );

    return parent;
    }


TBool CMemSpyViewMemoryTrackingAutoStartConfig::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdMemoryTrackingAutoStartItemAdd:
        OnCmdItemAddL();
        break;
    case EMemSpyCmdMemoryTrackingAutoStartItemEdit:
        OnCmdItemEditL();
        break;
    case EMemSpyCmdMemoryTrackingAutoStartItemDelete:
        OnCmdItemDeleteL();
        break;
    case EMemSpyCmdMemoryTrackingAutoStartItemDeleteAll:
        OnCmdItemDeleteAllL();
        break;
    case EMemSpyCmdMemoryTrackingAutoStartItemImport:
        OnCmdItemImportL();
        break;
    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::HandleListBoxItemActionedL( TInt /*aIndex*/ )
    {
    OnCmdItemEditL();
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<128> buf;
    _LIT( KProcUidFormatSpec, "\t0x%08x" );

    const TInt count = iProcessUids.Count();
    for( TInt i=0; i<count; i++ )
        {
        const TUid procUid = iProcessUids[ i ];
        //
        buf.Format( KProcUidFormatSpec, procUid.iUid );
        model->AppendL( buf );
        }

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    iListBox->HandleItemAdditionL();
    iListBox->DrawDeferred();
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if  ( aResourceId == MenuCascadeResourceId() )
        {
        const TInt count = iProcessUids.Count();
        const TInt index = iListBox->CurrentItemIndex();

        // We cannot edit an item if none exists
        aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingAutoStartItemEdit, ( count == 0 || index < 0 || index > count - 1 ) );
        aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingAutoStartItemDelete, ( count == 0 || index < 0 || index > count - 1 ) );
        aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingAutoStartItemDeleteAll, ( count <= 1 || index > count - 1 ) );

        // We can only import items if an XML file is available
        const TBool xmlAvailable = ( iXMLFileNameInstallTime->Length() || iXMLFileNameUserSupplied->Length() );
        aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingAutoStartItemImport, !xmlAvailable );
        }
    }


TUint CMemSpyViewMemoryTrackingAutoStartConfig::MenuCascadeResourceId() const
    {
    return R_MEMSPY_MENUPANE_MEMORY_TRACKING_AUTOSTART;
    }


TInt CMemSpyViewMemoryTrackingAutoStartConfig::MenuCascadeCommandId() const
    {
    return EMemSpyCmdMemoryTrackingAutoStart;
    }
   
    
void CMemSpyViewMemoryTrackingAutoStartConfig::OnCmdItemAddL()
    {
    const TUid uid = ShowDialogL( KNullUid );
    if ( uid != KNullUid )
        {
        User::LeaveIfError( iProcessUids.Append( uid ) );
        SaveChangesL();
        RefreshL();
        iListBox->HandleItemAdditionL();
        iListBox->DrawDeferred();
        }
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnCmdItemEditL()
    {
    const TInt index = iListBox->CurrentItemIndex();
    if ( index >= 0 && index < iProcessUids.Count() )
        {
        TUid processUid = iProcessUids[ index ];
        processUid = ShowDialogL( processUid );
        //
        if ( processUid != KNullUid )
            {
            iProcessUids[ index ] = processUid;
            SaveChangesL();
            RefreshL();
            iListBox->HandleItemAdditionL();
            iListBox->DrawDeferred();
            }
        }
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnCmdItemDeleteL()
    {
    const TInt index = iListBox->CurrentItemIndex();
    if ( index >= 0 && index < iProcessUids.Count() )
        {
        iProcessUids.Remove( index );
        SaveChangesL();
        RefreshL();
        iListBox->HandleItemRemovalL();
        iListBox->DrawDeferred();
        
        const TInt newIndex = index - 1;
        const TInt count = iProcessUids.Count();
        if ( newIndex >=0 && newIndex < count )
            {
            iListBox->SetCurrentItemIndex( index );
            }
        }
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnCmdItemDeleteAllL()
    {
    iProcessUids.Reset();
    SaveChangesL();
    RefreshL();
    iListBox->HandleItemRemovalL();
    iListBox->DrawDeferred();
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnCmdItemImportL()
    {
    if  ( iXMLFileNameInstallTime->Length() )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        if  ( importDialog->ExecuteLD( R_MEMSPY_MEMORY_TRACKING_IMPORT_FROM_INSTALLED_XML_DIALOG ) )
            {
            TRAP_IGNORE( ParseL( *iXMLFileNameInstallTime ) );
            }
        }
    if  ( iXMLFileNameUserSupplied->Length() )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        if  ( importDialog->ExecuteLD( R_MEMSPY_MEMORY_TRACKING_IMPORT_FROM_USERDEFINED_XML_DIALOG ) )
            {
            TRAP_IGNORE( ParseL( *iXMLFileNameUserSupplied ) );
            }
        }

    SaveChangesL();
    RefreshL();
    iListBox->HandleItemRemovalL();
    iListBox->DrawDeferred();
    }


TUid CMemSpyViewMemoryTrackingAutoStartConfig::ShowDialogL( TUid aUid )
    {
    TUid uid = KNullUid;
    //
    _LIT( KProcessUidFormat, "%x" );
    TBuf<128> buf;
    buf.Format( KProcessUidFormat, aUid );
    //
    CAknTextQueryDialog* dialog = new(ELeave) CAknTextQueryDialog( buf );
    const TBool dialogActioned = ( dialog->ExecuteLD( R_MEMSPY_MEMORY_TRACKING_PROCESS_UID_DIALOG ) );
    //
    if ( dialogActioned )
        {
        // Validate the text
        uid = ValidateProcessUid( buf );
        }
    //
    return uid;
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::SaveChangesL()
    {
    iEngine.HelperProcess().SetMemoryTrackingAutoStartProcessListL( iProcessUids );
    Settings().StoreSettingsL();
    }


TUid CMemSpyViewMemoryTrackingAutoStartConfig::ValidateProcessUid( const TDesC& aUid )
    {
    TUid ret = KNullUid;
    TBool isValid = ETrue;
    //
    const TInt length = aUid.Length();
    for( TInt i=0; i<length && isValid; i++ )
        {
        const TChar charValue = aUid[ i ];
        //
        switch( charValue )
            {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'c':
        case 'C':
        case 'd':
        case 'D':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
            break;
        default:
            isValid = EFalse;
            break;
            }
        }
    //
    if ( isValid )
        {
        TUint32 val = 0;
        TLex lexer( aUid );
        if ( lexer.Val( val, EHex ) == KErrNone )
            {
            ret.iUid = val;
            }
        }
    //
    return ret;
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::FindXmlInstallTimeL()
    {
    RFs& fsSession = iEngine.FsSession();
    //
    TFileName* fileName = new(ELeave) TFileName();
    CleanupStack::PushL( fileName );
    //
    fsSession.PrivatePath( *fileName );
    //
    TFindFile findFile( fsSession );
    if  ( findFile.FindByPath( KMemSpyProcessMemoryTrackingAutoStartConfigFileName, fileName ) == KErrNone )
        {
        iXMLFileNameInstallTime = findFile.File().AllocL();
        }
    else
        {
        iXMLFileNameInstallTime = KNullDesC().AllocL();
        RDebug::Print( _L("CMemSpyViewMemoryTrackingAutoStartConfig::FindXmlInstallTimeL() - NO FILE FOUND") );
        }
    //
    CleanupStack::PopAndDestroy( fileName );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::FindXmlUserSuppliedL()
    {
    TFindFile findFile( iEngine.FsSession() );
    if  ( findFile.FindByPath( KMemSpyProcessMemoryTrackingAutoStartConfigFileName, &KMemSpyProcessMemoryTrackingAutoStartConfigSearchPath ) == KErrNone )
        {
        iXMLFileNameUserSupplied = findFile.File().AllocL();
        }
    else
        {
        iXMLFileNameUserSupplied = KNullDesC().AllocL();
        RDebug::Print( _L("CMemSpyViewMemoryTrackingAutoStartConfig::FindXmlUserSuppliedL() - NO FILE FOUND") );
        }
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::ParseL( const TDesC& aFileName )
    {
	Xml::ParseL( *iParser, iEngine.FsSession(), aFileName );
    //
    const TInt error = iParserErrorCode;
    iParserErrorCode = KErrNone;
    //
	User::LeaveIfError( error );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnSectionProcessL( const RAttributeArray& aAttributes )
    {
    TUid uid = KNullUid;
    TBool gotSID = EFalse;
    //
    const TInt count = aAttributes.Count();
	for( TInt i=0; i<count; i++ )
		{
		const TPtrC8 attrib( aAttributes[ i ].Attribute().LocalName().DesC() );
		const TPtrC8 value( aAttributes[ i ].Value().DesC() );
        //
		if  ( attrib.CompareF( KMemSpyXmlSpec_EntryProcess_SID ) == 0 )
			{
            if  ( gotSID )
                {
                User::LeaveIfError( KErrCorrupt );
                }
            else
                {
                if  ( value.Length() >= 8 )
                    {
                    TRadix radix = EDecimal;
                    TPtrC8 pValue( value );
                    //
                    if  ( value.Length() == 10 && value.Left( 2 ) == KMemSpyXmlSpec_HexPrefix )
                        {
                        pValue.Set( value.Mid( 2 ) );
                        radix = EHex;
                        }
                    //
                    TUint32 uidVal = 0;
                    TLex8 lexer( pValue );
                    const TInt err = lexer.Val( uidVal, radix );
                    User::LeaveIfError( err );
                    uid.iUid = uidVal;
                    gotSID = ETrue;
                    }
                }
			}
        }
    //
    if  ( gotSID )
        {
        User::LeaveIfError( iProcessUids.Append( uid ) );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnStartDocumentL( const RDocumentParameters& /*aDocParam*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnEndDocumentL( TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnStartElementL( const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    //
	const TPtrC8 name( aElement.LocalName().DesC() );
    //
    if  ( name.CompareF( KMemSpyXmlSpec_MasterSection ) == 0 )
		{
		iSeenMasterSection = ETrue;
		}
    else if ( iSeenMasterSection && name.CompareF( KMemSpyXmlSpec_EntryProcess ) == 0 )
        {
        OnSectionProcessL( aAttributes );
        }
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnEndElementL( const RTagInfo& /*aElement*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnContentL( const TDesC8& /*aBytes*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnStartPrefixMappingL( const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnEndPrefixMappingL( const RString& /*aPrefix*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnSkippedEntityL( const RString& /*aName*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnProcessingInstructionL( const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode )
    {
    User::LeaveIfError( aErrorCode );
    }


void CMemSpyViewMemoryTrackingAutoStartConfig::OnError( TInt aErrorCode )
    {
    iParserErrorCode = aErrorCode;
    }


TAny* CMemSpyViewMemoryTrackingAutoStartConfig::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    return NULL;
    }

