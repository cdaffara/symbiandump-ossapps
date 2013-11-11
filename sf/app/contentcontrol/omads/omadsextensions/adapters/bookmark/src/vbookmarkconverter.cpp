/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Bookmark object conversion routines
*
*/


// INCLUDES
#include <utf.h>
#include <favouritesitem.h>
#include <feedsserverfolderitem.h>
#include "vbookmarkconverter.h"
#include "logger.h"

// CONSTANTS
_LIT8(KVBMKBegin, "BEGIN");
_LIT8(KVBMKEnd, "END");
_LIT8(KVBMKVbkm, "VBKM");
_LIT8(KVBMKEnv, "ENV");
_LIT8(KVBMKVersion, "VERSION");         
_LIT8(KVBMKVer, "1.0");             
_LIT8(KVBMKUrl, "URL");                 
_LIT8(KVBMKTitle, "TITLE");     
_LIT8(KVBMKXIrmcUrl, "X-IRMC-URL");
_LIT8(KVBMKInternetShortcut, "[InternetShortcut]"); 
_LIT8(KVBMKPrintable, "QUOTED-PRINTABLE");  
_LIT8(KVBMKLinefeed, "\r\n");
_LIT8(KVBMKColon, ":");
_LIT8(KVBMKAssignment, "=");
_LIT8(KVBMKSemicolon, ";");
_LIT8(KVBMKNone, "");
_LIT(KEmptyString, "");
_LIT8(KVBMKCharset, "CHARSET");
_LIT8(KVBMKUtf8, "UTF-8");

// -----------------------------------------------------------------------------
// CVBookmarkConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CVBookmarkConverter* CVBookmarkConverter::NewL()
    {
    CVBookmarkConverter* self = CVBookmarkConverter::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVBookmarkConverter::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CVBookmarkConverter* CVBookmarkConverter::NewLC()
    {
    CVBookmarkConverter* self = new (ELeave) CVBookmarkConverter();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVBookmarkConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CVBookmarkConverter::ConstructL()
    {
    iVersion = HBufC::NewL(1);
    iTitle = HBufC::NewL(1);
    iUrl = HBufC::NewL(1);
    iInternetShortcut = HBufC::NewL(1);
    }

// -----------------------------------------------------------------------------
// CVBookmarkConverter::CVBookmarkConverter
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
CVBookmarkConverter::CVBookmarkConverter()
    {
    }

// -----------------------------------------------------------------------------
// CVBookmarkConverter::~CVBookmarkConverter
// Destructor.
// -----------------------------------------------------------------------------
CVBookmarkConverter::~CVBookmarkConverter()
    {
    delete iVersion;
    delete iUrl;
    delete iTitle;
    delete iInternetShortcut;
    }

const TDesC& CVBookmarkConverter::GetVersion() const
    {
    return *iVersion;
    }

const TDesC& CVBookmarkConverter::GetUrl() const
    {
    return *iUrl;
    }

const TDesC& CVBookmarkConverter::GetTitle() const
    {
    return *iTitle;
    }

const TDesC& CVBookmarkConverter::GetInternetShortcut() const
    {
    return *iInternetShortcut;
    }

void CVBookmarkConverter::SetVersionL( const TDesC& aVersion )
    {
    delete iVersion;
    iVersion = NULL;
    iVersion = aVersion.AllocL();
    }
    
void CVBookmarkConverter::SetVersionL( const TDesC8& aVersion )
    {
    delete iVersion;
    iVersion = NULL;
    iVersion = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aVersion );  
    }   
    
void CVBookmarkConverter::SetUrlL( const TDesC& aUrl )
    {
    delete iUrl;
    iUrl = NULL;
    iUrl = aUrl.AllocL();
    }
    
void CVBookmarkConverter::SetUrlL( const TDesC8& aUrl )
    {
    delete iUrl;
    iUrl = NULL;
    iUrl = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aUrl );      
    }   
    
void CVBookmarkConverter::SetTitleL( const TDesC& aTitle )
    {
    delete iTitle;
    iTitle = NULL;
    iTitle = aTitle.AllocL();
    }
        
void CVBookmarkConverter::SetTitleL( const TDesC8& aTitle, TBookmarkEncoding aEncoding )
    {
    delete iTitle;
    iTitle = NULL;
    
    if ( aEncoding == EQuotedPrintable )
        {
        HBufC8* decodedData = DecodeQuotedPrintableLC( aTitle );
        iTitle = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *decodedData );
        CleanupStack::PopAndDestroy( decodedData );
        }
    else
        {
        iTitle = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aTitle );
        }        
    }
    
HBufC8* CVBookmarkConverter::DecodeQuotedPrintableLC( const TDesC8& aEncodedData )
    {
    TInt encodedLength = aEncodedData.Length();
    
    HBufC8* decodeBuffer = HBufC8::NewLC( encodedLength );
    TPtr8 ptrDecodeBuffer = decodeBuffer->Des();
    
    TInt i(0);
    while ( i < encodedLength )
        {
        TChar nextChar = aEncodedData[i++];
        
        if ( nextChar == '=' )
            {
            if ( i + 2 > encodedLength )
                {
                ptrDecodeBuffer.Append( '=' );
                if ( i + 1 == encodedLength )
                    {
                    ptrDecodeBuffer.Append( aEncodedData[i++] );
                    }     
                }
            else
                {
                TPtrC8 data = aEncodedData.Mid( i, 2 );
                i += 2;    
                
                if ( data.Compare(KVBMKLinefeed) == 0 )
                    {
                    // Do nothing
                    // '=' at the end of line is soft line break, not to be decoded
                    }
                else
                    {
                    TLex8 hexChar( data );
                    TUint8 value( 0 );
                    
                    if ( hexChar.Val( value, EHex ) == KErrNone )
                        {
                        ptrDecodeBuffer.Append( TChar( value ) );
                        }
                    else
                        {
                        // Decoding failed, put the data at itself
                        ptrDecodeBuffer.Append( '=' );
                        ptrDecodeBuffer.Append( data );
                        }    
                    }
                }
            }
        else
            {
            ptrDecodeBuffer.Append( nextChar );
            }    
        }
        
    return decodeBuffer;    
    }

void CVBookmarkConverter::SetInternetShortcutL( const TDesC& aInternetShortcut )
    {
    delete iInternetShortcut;
    iInternetShortcut = NULL;
    iInternetShortcut = aInternetShortcut.AllocL();
    }
    
void CVBookmarkConverter::SetInternetShortcutL( const TDesC8& aInternetShortcut )
    {
    delete iInternetShortcut;
    iInternetShortcut = NULL;
    iInternetShortcut = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aInternetShortcut );        
    }   

void CVBookmarkConverter::ExportDbItemL( CFavouritesItem& aItem )
    {
    if ( iTitle->Des().Length() > KFavouritesMaxName )
        {
        TPtrC title = iTitle->Des().Left( KFavouritesMaxName );
        aItem.SetNameL( title );
        }
    else
        {
        aItem.SetNameL( *iTitle );
        }
        
    if ( iUrl->Des().Length() > KFavouritesMaxUrl )
        {
        TPtrC url = iUrl->Des().Left( KFavouritesMaxUrl );
        aItem.SetUrlL( url );
        }
    else
        {
        aItem.SetUrlL( *iUrl );
        }   
    }

void CVBookmarkConverter::ImportDbItemL( const CFavouritesItem& aItem )
    {
    ResetL();
    
    if( aItem.Type() != CFavouritesItem::EItem )
        {
        User::Leave( KErrNotSupported );
        }
    
    SetVersionL( KVBMKVer );
    SetUrlL( aItem.Url() );
    SetInternetShortcutL( aItem.Url() );
    SetTitleL( aItem.Name() );
    }

void CVBookmarkConverter::ImportDbItemL( const CFolderItem& aItem )
    {
    ResetL();
    
    if( aItem.IsFolder() )
        {
        User::Leave( KErrNotSupported );
        }
    
    SetVersionL( KVBMKVer );
    SetUrlL( aItem.SourceUrl() );
    SetInternetShortcutL( aItem.SourceUrl() );
    SetTitleL( aItem.Name() );
    }

void CVBookmarkConverter::ExportVBookmarkL( CBufBase& aBuffer )
    {
    LOGGER_ENTERFN( "CVBookmarkConverter::ExportVBookmarkL" );
    
    iDesc = NULL;
    iBuffer = &aBuffer;
    iWriteBufPosition = 0;
    iWriteBufSize = aBuffer.Size();     
    
    ExportL();  
    iBuffer->ResizeL( iWriteBufPosition );    
    
    LOGGER_LEAVEFN( "CVBookmarkConverter::ExportVBookmarkL" );
    }

void CVBookmarkConverter::ExportVBookmarkL( TDes8& aBuffer )
    {
    iDesc = &aBuffer;
    iBuffer = NULL;
    
    ExportL();
    }

void CVBookmarkConverter::ResetL()
    {
    SetVersionL( KEmptyString );
    SetUrlL( KEmptyString );
    SetTitleL( KEmptyString );    
    SetInternetShortcutL( KEmptyString );    
    }
    
void CVBookmarkConverter::ExportL()
    {
    LOGGER_ENTERFN( "CVBookmarkConverter::ExportL" );
    
    WriteMessageLineL( KVBMKBegin, KVBMKVbkm, KVBMKColon ); // Begin tag
    
    HBufC8* version = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *iVersion );
    CleanupStack::PushL( version );
    WriteMessageLineL( KVBMKVersion, *version, KVBMKColon ); // Version
    CleanupStack::PopAndDestroy( version );
    
    HBufC8* url = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *iUrl );
    CleanupStack::PushL( url );
    // Url
    WriteL( KVBMKUrl );
    WriteL( KVBMKSemicolon );
    WriteL( KVBMKCharset );
    WriteL( KVBMKAssignment );
    WriteL( KVBMKUtf8 );
    WriteL( KVBMKColon );
    WriteL( *url );
    WriteL( KVBMKLinefeed );
    CleanupStack::PopAndDestroy( url );
    
    HBufC8* title = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *iTitle );
    CleanupStack::PushL( title );
    // Title
    WriteL( KVBMKTitle );
    WriteL( KVBMKSemicolon );
    WriteL( KVBMKCharset );
    WriteL( KVBMKAssignment );
    WriteL( KVBMKUtf8 );
    WriteL( KVBMKColon );
    WriteL( *title );
    WriteL( KVBMKLinefeed );
    CleanupStack::PopAndDestroy( title );          

    if( iInternetShortcut->Length() > 0 )
        {
        WriteEnvelopeL();
        }
        
    WriteMessageLineL( KVBMKEnd, KVBMKVbkm, KVBMKColon ); // End tag
    
    LOGGER_LEAVEFN( "CVBookmarkConverter::ExportL" );       
    }
    
TInt CVBookmarkConverter::ImportVBookmarkL( const TDesC8& aBuffer )
    {
    LOGGER_ENTERFN( "CVBookmarkConverter::ImportVBookmarkL" );
    
    ResetL();
        
    TInt error = KErrNone;
    TInt position = 0;
    TPtrC8 tag;
    TPtrC8 value;
    TPtrC8 properties;
    error = ReadTagAndValue( aBuffer, position, tag, value, properties );
    if( error != KErrNone )
        {
        LOGGER_WRITE_1( "ReadTagAndValue err: %d", error );
        return error;
        }
        
    if ( ( tag != KVBMKBegin ) || ( value != KVBMKVbkm ) )
        {
        LOGGER_WRITE( "Invalid tag" );
        return KErrNotFound;
        }
    
    while ( ( error = ReadTagAndValue( aBuffer, position, tag, value, properties ) ) == KErrNone )
        {
        // Version
        if ( tag == KVBMKVersion )
            {
            SetVersionL( value );
            }
        
        // Url
        else if ( tag == KVBMKUrl )
            {
            SetUrlL( value );
            }
        
        // Title
        else if ( tag == KVBMKTitle )
            {
            TBookmarkEncoding encoding = Encoding( properties );
            SetTitleL( value, encoding );
            }
            
        // Beginning of envelope    
        else if ( ( tag == KVBMKBegin ) && ( value == KVBMKEnv ) )
            {
            // Read the envelope, quit if error
            error = ReadEnvelopeL( aBuffer, position );
            if( error != KErrNone )
                {
                return error;
                }
            }       
                
        // End of Bookmark
        else if ( ( tag == KVBMKEnd ) && ( value == KVBMKVbkm ) )
            {
            return error;
            }
        
        else
            {
            // Unknown data, skip
            }
        }

    LOGGER_LEAVEFN( "CVBookmarkConverter::ImportVBookmarkL" );      
    return error;
    }

void CVBookmarkConverter::WriteMessageLineL( const TDesC8& aMsgLineTag,
    const TDesC8& aMsgLineData, const TDesC8& aSeparator )
    {
    WriteL( aMsgLineTag );
    WriteL( aSeparator );
    WriteL( aMsgLineData );
    WriteL( KVBMKLinefeed );
    }

TInt CVBookmarkConverter::ReadTagAndValue( const TDesC8& aBuffer, TInt& aPosition, 
    TPtrC8& aTag, TPtrC8& aValue, TPtrC8& aProperties )
    {
    LOGGER_ENTERFN( "CVBookmarkConverter::ReadTagAndValue" );   
    
    TPtrC8 start = aBuffer.Mid( aPosition );
    TInt delimeter = start.Find( KVBMKColon );
    TInt linefeed = start.Find( KVBMKLinefeed );
    TInt semicolon = start.Find( KVBMKSemicolon );
    
    
    // Did we find a linefeed?
    if ( linefeed == KErrNotFound )
        {
        // Check whether file ends without newline
        if( aBuffer.Length() <= ( aPosition + start.Length() ) )
            {
            // Set linefeed at the end of the file
            linefeed = start.Length();
            }
        else
            { 
            return KErrNotFound;
            }
        }

    // Did we find a semicolon?
    if ( semicolon != KErrNotFound )
        {
        // It must be before the delimeter (property identifiers reside in this block)
        if ( semicolon >= delimeter || semicolon < 0 )
            {
            semicolon = KErrNotFound;
            }
        }
    
    // Did we find the delimeter
    if( delimeter == KErrNotFound )
        {
        return KErrNotFound;
        }

    // Linefeed must reside behind the delimeter
    if ( linefeed <= delimeter )
        {
        return KErrNotFound;
        }
    
    aTag.Set( start.Left( semicolon != KErrNotFound ? semicolon : delimeter ) );
    IgnoreSpaces( aTag );

    // Extract value
    aValue.Set( start.Mid( delimeter + 1, (linefeed - 1) - delimeter ) );
    IgnoreSpaces( aValue );
    
    // Extract properties if found
    if ( semicolon != KErrNotFound )
        {
        aProperties.Set( start.Mid( semicolon, delimeter - semicolon ) );
        IgnoreSpaces( aProperties );
        }
    else
        {
        aProperties.Set( KVBMKNone() );
        }     
    
    // update position
    aPosition += ( linefeed + KVBMKLinefeed().Length() );
    
    LOGGER_LEAVEFN( "CVBookmarkConverter::ReadTagAndValue" );       
    return KErrNone;
    }
    
TInt CVBookmarkConverter::ReadEnvelopeL( const TDesC8& aBuffer, TInt& aPosition )
    {
    LOGGER_ENTERFN( "CVBookmarkConverter::ReadEnvelopeL" ); 
        
    TInt error = KErrNone;
    TPtrC8 tag;
    TPtrC8 value;
    TPtrC8 properties;

    while ( ( error = ReadTagAndValue( aBuffer, aPosition, tag, value, properties ) ) == KErrNone )
        {
        // End of Bookmark
        if ( tag == KVBMKXIrmcUrl )
            {
            error = ReadAssignment( aBuffer, aPosition, tag, value );
            if ( error != KErrNone )
                {
                return error;
                }
                
            if ( tag == KVBMKInternetShortcut )
                {
                error = ReadAssignment( aBuffer, aPosition, tag, value );
                if( error != KErrNone )
                    {
                    return error;
                    }
                
                if ( tag == KVBMKUrl )
                    {
                    SetInternetShortcutL( value );
                    }
                else
                    {
                    return KErrNotFound;
                    }               
                }
            else
                {
                return KErrNotFound;
                }
            }
        
        else if ( ( tag == KVBMKEnd ) && ( value == KVBMKEnv ) )
            {
            return error;
            }
        else
            {
            // Unknown data, skip
            }
        }

    LOGGER_LEAVEFN( "CVBookmarkConverter::ReadEnvelopeL" );         
    return error;
    }

TInt CVBookmarkConverter::ReadAssignment( const TDesC8& aBuffer,
    TInt& aPosition, TPtrC8& aTag, TPtrC8& aValue )
    {
    LOGGER_ENTERFN( "CVBookmarkConverter::ReadAssignment" );    
        
    TPtrC8 start = aBuffer.Mid( aPosition );
    TInt assignment = start.Find( KVBMKAssignment );
    TInt linefeed = start.Find( KVBMKLinefeed );
    
    // Did we find the delimeter and the linefeed
    if ( ( assignment == KErrNotFound) || ( linefeed == KErrNotFound ) )
        {
        return KErrNotFound;
        }
    // Linefeed must reside behind the delimeter
    if ( linefeed <= assignment )
        {
        return KErrNotFound;
        }
    // Extract tag
    aTag.Set( start.Left( assignment ) );
    IgnoreSpaces( aTag );

    // Extract value
    aValue.Set( start.Mid( assignment + 1, ( linefeed - 1 ) - assignment ) );
    IgnoreSpaces( aValue );
    
    // update position
    aPosition += ( linefeed + KVBMKLinefeed().Length() );

    LOGGER_LEAVEFN( "CVBookmarkConverter::ReadAssignment" );        
    return KErrNone;
    }

void CVBookmarkConverter::WriteEnvelopeL()
    {
    WriteMessageLineL( KVBMKBegin, KVBMKEnv, KVBMKColon ); // Begin tag

    // "X-IRMC-URL;QUOTED-PRINTABLE:="
    WriteL( KVBMKXIrmcUrl );
    WriteL( KVBMKSemicolon );
    WriteL( KVBMKPrintable );
    WriteL( KVBMKColon );
    WriteL( KVBMKAssignment );
    WriteL( KVBMKLinefeed );
    
    WriteMessageLineL( KVBMKInternetShortcut, KVBMKNone, KVBMKAssignment );
    
    HBufC8* internetShortcut = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *iInternetShortcut );
    CleanupStack::PushL( internetShortcut );
    WriteMessageLineL( KVBMKUrl, *internetShortcut, KVBMKAssignment );
    CleanupStack::PopAndDestroy( internetShortcut );
    
    WriteMessageLineL( KVBMKEnd, KVBMKEnv, KVBMKColon ); // End tag
    }
    
void CVBookmarkConverter::WriteL( const TDesC8 &aData )
    {
    if ( iBuffer )
        {
        TInt newPosition = iWriteBufPosition + aData.Length();

        if ( newPosition > iWriteBufSize )
            {
            TInt expandStep = newPosition - iWriteBufSize + 100;
            iBuffer->ExpandL( iWriteBufSize, expandStep );
            iWriteBufSize += expandStep;
            }

        iBuffer->Write( iWriteBufPosition, aData );
        iWriteBufPosition = newPosition;
        }
    else if( iDesc )
        {
        iDesc->Append( aData );       
        }
    }
    
void CVBookmarkConverter::IgnoreSpaces( TPtrC8& aData )
    {
    TInt last = aData.Length() - 1;
    
    TInt begin;
    for ( begin = 0; begin <= last; begin++ )
        {
        if ( !TChar( aData[begin] ).IsSpace() )
            {
            break;
            }
        }
        
    TInt end;   
    for ( end = last; end >= 0; end-- )
        {
        if ( !TChar( aData[end] ).IsSpace() )
            {
            break;
            }
        }
        
    TInt length = end - begin + 1;  
                
    if ( ( begin != 0 || end != last ) && length > 0 )
        {
        TPtrC8 newData = aData.Mid( begin, length );
        aData.Set( newData );
        }
    }
    
TBookmarkEncoding CVBookmarkConverter::Encoding( TPtrC8& aProperties )
    {
    TBookmarkEncoding encoding = ENoEncoding;
    
    TInt length = aProperties.Length();
    
    if ( aProperties.Length() > 0 )
        {
        if ( aProperties.Find( KVBMKPrintable ) > 0 )
            {
            encoding = EQuotedPrintable;
            }    
        }
    
    return encoding;
    }
