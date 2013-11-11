/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contacts Group object conversion routines
*
*/


// INCLUDES
#include <utf.h>
#include <cntdef.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <vcard.h>
#include <vprop.h> 
#include <vutil.h>
#include <s32mem.h>

#include "contactsgrpconverter.h"
#include "logger.h"

// CONSTANTS
/*
vCard format example:
BEGIN:VCARD
VERSION:2.1
FN:GroupName
X-CNTGRP:4;14;54
END:VCARD
*/

// Group label property
_LIT8(KCntGrpFN, "FN");
// Group contacts propetry
_LIT8(KCntGrpXCntGrp, "X-CNTGRP");

// -----------------------------------------------------------------------------
// CContactsGrpConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CContactsGrpConverter* CContactsGrpConverter::NewL()
    {
    CContactsGrpConverter* self = CContactsGrpConverter::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CContactsGrpConverter* CContactsGrpConverter::NewLC()
    {
    TRACE_FUNC_ENTRY;
    CContactsGrpConverter* self = new (ELeave) CContactsGrpConverter();
    CleanupStack::PushL( self );
    self->ConstructL();
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::CContactsGrpConverter
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
CContactsGrpConverter::CContactsGrpConverter()
    {
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CContactsGrpConverter::ConstructL()
    {
    ResetL();
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::~CContactsGrpConverter
// Destructor.
// -----------------------------------------------------------------------------
CContactsGrpConverter::~CContactsGrpConverter()
    {
    TRACE_FUNC_ENTRY;
    delete iContactsIdArray;
    delete iGroupLabel;
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::GroupLabel
// returns group label
// -----------------------------------------------------------------------------
const TDesC& CContactsGrpConverter::GroupLabel() const
    {
    return *iGroupLabel;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::ItemsContained
// returns contact items belonging to this group
// -----------------------------------------------------------------------------
const CContactIdArray& CContactsGrpConverter::ItemsContained() const
    {
    return *iContactsIdArray;
    }
    
// -----------------------------------------------------------------------------
// CContactsGrpConverter::ImportDbItemL
// Imports group item
// -----------------------------------------------------------------------------
void CContactsGrpConverter::ImportDbItemL( CContactGroup& aItem )
    {
    TRACE_FUNC_ENTRY;
    if ( aItem.Type() != KUidContactGroup )
        {
        User::Leave( KErrNotSupported );
        }
    ResetL();
    
    if ( aItem.HasItemLabelField() )
        {
        TPtrC label = aItem.GetGroupLabelL();
        delete iGroupLabel;
        iGroupLabel = NULL;
        iGroupLabel = label.AllocL();
        }
    else
        {
        delete iGroupLabel;
        iGroupLabel = NULL;
        iGroupLabel = KNullDesC().AllocL();
        }
    
    // this is NULL, if no items
    CContactIdArray* tempIdArray = aItem.ItemsContainedLC();
    if ( tempIdArray )
        {
        delete iContactsIdArray;
        iContactsIdArray = tempIdArray;
        CleanupStack::Pop( tempIdArray );
        }
    
    LOGGER_WRITE_1("GroupName: %S", iGroupLabel);
    if ( iContactsIdArray )
        {
        LOGGER_WRITE_1("contacts count: %d", iContactsIdArray->Count() );
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::ExportVCardL
// Export contact group as vCard
// -----------------------------------------------------------------------------
void CContactsGrpConverter::ExportVCardL( CBufBase& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    RBufWriteStream stream( aBuffer, 0);
    CleanupClosePushL( stream );
    ExportVCardL( stream );
    CleanupStack::PopAndDestroy( &stream );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::ExportL
// Exports contact group as to vCard
// -----------------------------------------------------------------------------
void CContactsGrpConverter::ExportVCardL( RWriteStream& aWriteStream )
    {
    TRACE_FUNC_ENTRY;
    CParserVCard* vCardParser = CParserVCard::NewL();
    CleanupStack::PushL( vCardParser );
    
    // add group name
    CParserPropertyValue* FNvalue = CParserPropertyValueHBufC::NewL( *iGroupLabel );
    // Associate the property params and property value with the FN property
    CParserProperty* FNproperty=CParserProperty::NewL( *FNvalue, KCntGrpFN, NULL );
    // Finally add the FN property to the vCard using the parser object.
    vCardParser->AddPropertyL( FNproperty );
    
    // add group contacts
    const TInt KDefaultGranularity = 5;
    CDesCArrayFlat* flatArray = new (ELeave) CDesCArrayFlat(KDefaultGranularity);
    CleanupStack::PushL( flatArray );
    TInt count = iContactsIdArray->Count();
    const TInt KMaxIdLength = 10;
    TBuf<KMaxIdLength> idnum;
    for ( TInt i=0; i<count; i++ )
        {
        const TContactItemId id = (*iContactsIdArray)[i];
        idnum.Zero();
        idnum.AppendNum( id );
        flatArray->AppendL( idnum );
        }
    CParserPropertyValueCDesCArray* grPropertyValueArray =
        new (ELeave) CParserPropertyValueCDesCArray( flatArray );
    CleanupStack::Pop( flatArray );
    
    CParserProperty* grProperty = 
        CParserProperty::NewL( *grPropertyValueArray, KCntGrpXCntGrp, NULL );
    vCardParser->AddPropertyL( grProperty );
    
    // Set default sharacterset
    vCardParser->SetDefaultCharSet( Versit::EUTF8CharSet );
    
    vCardParser->ExternalizeL( aWriteStream );
    
    CleanupStack::PopAndDestroy( vCardParser );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::ImportVCardL
// Import vCard item
// -----------------------------------------------------------------------------
void CContactsGrpConverter::ImportVCardL( const TDesC8& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    ResetL();
    CParserVCard* vCardParser = CParserVCard::NewL();
    vCardParser->SetDefaultCharSet( Versit::EUTF8CharSet );
    CleanupStack::PushL( vCardParser );
    
    RDesReadStream stream( aBuffer );
    vCardParser->InternalizeL( stream ); // To import a vCard.
    stream.Close(); // Close the file stream once the import is done.
    
    CArrayPtr<CParserProperty>* properties = vCardParser->ArrayOfProperties( EFalse );
    LOGGER_WRITE_1("properties count: %d", properties->Count());
    for (TInt i=0; i<properties->Count(); i++)
        {
        const CParserProperty* property = properties->At(i);
        // FN
        if ( property->Name() == KCntGrpFN )
            {
            LOGGER_WRITE("group name found");
            delete iGroupLabel;
            iGroupLabel = NULL;
            CParserPropertyValue* pvalue = property->Value();
            TPtrC val = (static_cast<CParserPropertyValueHBufC*> (pvalue))->Value();
            iGroupLabel = val.AllocL();
            LOGGER_WRITE_1("iGroupLabel: %S", iGroupLabel);
            }
        // X-CNTGRP
        else if ( property->Name() == KCntGrpXCntGrp )
            {
            LOGGER_WRITE("group contacts found");
            CParserPropertyValue* pvalue = property->Value();
            TPtrC val = (static_cast<CParserPropertyValueHBufC*> (pvalue))->Value();
            SetContactItemsL( val );
            }
        else
            {
            // skip other properties
            }
        }
    CleanupStack::PopAndDestroy( vCardParser );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::ResetL
// Reset all data
// -----------------------------------------------------------------------------
void CContactsGrpConverter::ResetL()
    {
    delete iGroupLabel;
    iGroupLabel = NULL;
    iGroupLabel = KNullDesC().AllocL();
    if ( iContactsIdArray )
        {
        iContactsIdArray->Reset();
        }
    else
        {
        iContactsIdArray = CContactIdArray::NewL();
        }
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::SetContactItemsL
// Parses contact items from aValue
// -----------------------------------------------------------------------------
void CContactsGrpConverter::SetContactItemsL( const TDesC& aValue )
    {
    TRACE_FUNC_ENTRY;
    const TInt KDefaultGranularity = 5;
    RArray<TPtrC> arr(KDefaultGranularity);
    CleanupClosePushL( arr );
    const TChar separator(';');
    SplitL( aValue, separator, arr );
    if ( iContactsIdArray )
        {
        iContactsIdArray->Reset();
        }
    else
        {
        iContactsIdArray = CContactIdArray::NewL();
        }
    for (TInt i=0; i<arr.Count(); i++ )
        {
        TLex lex( arr[i] );
        TInt32 id;
        TInt err = lex.Val( id );
        if ( err )
            {
            LOGGER_WRITE_1("lex.val returned err: %d, leaving with KErrCorrupt", err);
            iContactsIdArray->Reset();
            User::Leave( KErrCorrupt );
            }
        LOGGER_WRITE_1("add: %d", id);
        iContactsIdArray->AddL( id );
        }
    CleanupStack::PopAndDestroy( &arr );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::SplitL
// Function splits string (eg "name1, name2, name3") into substrings.
// -----------------------------------------------------------------------------
void CContactsGrpConverter::SplitL(const TDesC& aText, const TChar aSeparator, 
                    RArray<TPtrC>& aArray)
    {
    TRACE_FUNC_ENTRY;
    TPtrC ptr;
    ptr.Set(aText);

    for (;;)
        {
        TInt pos=ptr.Locate(aSeparator);
        if (pos==KErrNotFound)
            {
            TrimAll( ptr );
            if ( ptr.Length() > 0 )
                {
                aArray.AppendL(ptr);
                }
            break;
            }

        TPtrC subStr=ptr.Left(pos); // get pos characters starting from position 0
        TrimAll( subStr );
        if ( subStr.Length() > 0 )
            {
            aArray.AppendL(subStr);
            }

        if ( !(ptr.Length()>pos+1) )
            {
            break;
            }
            
        ptr.Set( ptr.Mid(pos+1) ); // get all characters starting from position pos+1
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpConverter::TrimAll
// Trims all whitespaces from beginning and end of string
// -----------------------------------------------------------------------------
void CContactsGrpConverter::TrimAll( TPtrC& aValue )
    {
    TRACE_FUNC_ENTRY;
    TInt valueStart(0);
    while ( valueStart < aValue.Length() 
            && (aValue[valueStart] == ' '
            || aValue[valueStart] == '\r'
            || aValue[valueStart] == '\n') )
        {
        valueStart++;
        }
    TInt valueEnd(aValue.Length()-1);
    while ( valueEnd > 0 
            && (aValue[valueEnd] == ' '
            || aValue[valueEnd] == '\r'
            || aValue[valueEnd] == '\n') )
        {
        valueEnd--;
        }
    valueEnd++;
    aValue.Set( aValue.Mid(valueStart, valueEnd - valueStart));
    TRACE_FUNC_EXIT;
    }

