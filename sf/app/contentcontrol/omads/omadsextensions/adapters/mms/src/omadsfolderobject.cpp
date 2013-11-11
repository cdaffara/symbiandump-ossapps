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
* Description:  OMA DS Folder object XML parser
*
*/


#include <SenXmlUtils.h>
#include <utf.h>
#include "omadsfolderobject.h"
#include "logger.h"

_LIT8(KOMADSFolderElement, "Folder");
_LIT8(KOMADSNameElement, "name");
_LIT8(KOMADSCreatedElement, "created");
_LIT8(KOMADSModifiedElement, "modified");

_LIT(KFolderBeginTag, "<Folder>");
_LIT(KFolderEndTag, "</Folder>");
_LIT(KNameBeginTag, "<name>");
_LIT(KNameEndTag, "</name>");
_LIT(KCreatedBeginTag, "<created>");
_LIT(KCreatedEndTag, "</created>");
_LIT(KModifiedBeginTag, "<modified>");
_LIT(KModifiedEndTag, "</modified>");
_LIT(KDateFormat, "%04d%02d%02dT%02d%02d%02d");


COMADSFolderObject* COMADSFolderObject::NewLC()
    {
    COMADSFolderObject* pSelf = new (ELeave) COMADSFolderObject;
    CleanupStack::PushL( pSelf );
    pSelf->ConstructL();
    return pSelf;
    }

COMADSFolderObject* COMADSFolderObject::NewL()
    {
    COMADSFolderObject* pSelf = COMADSFolderObject::NewLC();
    CleanupStack::Pop( pSelf );    
    return pSelf;
    }

COMADSFolderObject::~COMADSFolderObject()
    {
    delete iXmlReader;
    }

void COMADSFolderObject::ConstructL()
    {
    iXmlReader = CSenXmlReader::NewL();
    iXmlReader->SetContentHandler(*this);
    }

TInt COMADSFolderObject::ImportFolderXml( RFs& aFs, const TDesC& aFilename )
    {
    LOGGER_ENTERFN( "COMADSFolderObject::ImportFolderXml" );    
        
    TInt error;
    
    Reset();
    TRAP( error, iXmlReader->ParseL( aFs, aFilename ) )
    if( error != KErrNone )
        {
        LOGGER_LEAVEFN( "COMADSFolderObject::ImportFolderXml" );
        return error;
        }
    
    LOGGER_LEAVEFN( "COMADSFolderObject::ImportFolderXml" );
    return iError;
    }
    
TInt COMADSFolderObject::ImportFolderXml( const TDesC8& aData )
    {
    LOGGER_ENTERFN( "COMADSFolderObject::ImportFolderXml" );
    LOG(aData);     
    
    TInt error;
    
    Reset();
    
    TRAP(error, iXmlReader->ParseL(aData) );
    if(error != KErrNone )
        {
        LOGGER_LEAVEFN( "COMADSFolderObject::ImportFolderXml" );
        return error;
        }
    LOGGER_LEAVEFN( "COMADSFolderObject::ImportFolderXml" );
    return iError;
    }
    
void COMADSFolderObject::ExportFolderXmlL( CBufBase& aBuffer )
    {
    iDesc = NULL;
    iBuffer = &aBuffer;
    iWriteBufPosition = 0;
    iWriteBufSize = aBuffer.Size();     

    ExportL();
    }
    
void COMADSFolderObject::ExportFolderXmlL( TDes8& aBuffer )
    {
    iDesc = &aBuffer;
    iBuffer = NULL;
    
    ExportL();
    }
    
void COMADSFolderObject::ExportL()
    {
    // Folder
    WriteL( KFolderBeginTag );
    
    // Name
    WriteL( KNameBeginTag );
    WriteEncodedXmlL( iName );
    WriteL( KNameEndTag );
    
    // Created Date
    TBuf<32> tempdate;
    tempdate.Format( KDateFormat, iCreatedDate.Year(), iCreatedDate.Month() + 1,
        iCreatedDate.Day(), iCreatedDate.Hour(), iCreatedDate.Minute(), iCreatedDate.Second() );
    
    WriteL( KCreatedBeginTag );
    WriteL( tempdate );
    WriteL( KCreatedEndTag );
    
    // Modified Date
    tempdate.Format( KDateFormat, iModifiedDate.Year(), iModifiedDate.Month() + 1,
        iModifiedDate.Day(), iModifiedDate.Hour(), iModifiedDate.Minute(), iModifiedDate.Second() );
    
    WriteL( KModifiedBeginTag );
    WriteL( tempdate );
    WriteL( KModifiedEndTag );
    
    // Folder end
    WriteL(KFolderEndTag);
    }
    
void COMADSFolderObject::WriteL( const TDesC &aData )
    {   
    CnvUtfConverter converter;
    User::LeaveIfError( converter.ConvertFromUnicodeToUtf8( iTemp, aData ) );
                    
    if ( iBuffer )
        {
        TInt newPosition = iWriteBufPosition + iTemp.Length();

        if ( newPosition > iWriteBufSize )
            {
            TInt expand = newPosition - iWriteBufSize;
            iBuffer->ExpandL( iWriteBufSize, expand );
            iWriteBufSize += expand;
            }

        iBuffer->Write( iWriteBufPosition, iTemp );
        iWriteBufPosition = newPosition;
        }
    else if( iDesc )
        {
        iDesc->Append( iTemp );       
        }   
    }
    
void COMADSFolderObject::WriteEncodedXmlL( const TDesC &aData )
    {   
    CnvUtfConverter converter;
    User::LeaveIfError( converter.ConvertFromUnicodeToUtf8( iTemp, aData ) );
    
    HBufC8* xmlData = SenXmlUtils::EncodeHttpCharactersLC( iTemp ); 
                    
    if ( iBuffer )
        {
        TInt newPosition = iWriteBufPosition + xmlData->Length();

        if ( newPosition > iWriteBufSize )
            {
            TInt expand = newPosition - iWriteBufSize;
            iBuffer->ExpandL( iWriteBufSize, expand );
            iWriteBufSize += expand;
            }

        iBuffer->Write( iWriteBufPosition, *xmlData );
        iWriteBufPosition = newPosition;
        }
    else if( iDesc )
        {
        iDesc->Append( *xmlData );
        }   
        
    CleanupStack::PopAndDestroy( xmlData );   
    }           
    
TInt COMADSFolderObject::StartDocument()
    {
    return KErrNone;
    }

TInt COMADSFolderObject::EndDocument()
    {
    return KErrNone;
    }

TInt COMADSFolderObject::StartElement(const TDesC8& /*aURI*/, 
                  const TDesC8& /*aLocalName*/, 
                  const TDesC8& aName, 
                  const RAttributeArray& /*apAttrs*/)
    {
    LOGGER_ENTERFN( "COMADSFolderObject::StartElement" );
    LOG( aName );    
    iCurrentElement.Copy( aName );
    LOGGER_LEAVEFN( "COMADSFolderObject::StartElement" );
    return KErrNone;
    }
                                    
TInt COMADSFolderObject::EndElement( const TDesC8& /*aURI*/, 
    const TDesC8& /*aLocalName*/, const TDesC8& /*aName*/ )
    {
    LOGGER_ENTERFN( "COMADSFolderObject::EndElement" );        
    iCurrentElement.SetLength(0);
    LOGGER_LEAVEFN( "COMADSFolderObject::EndElement" );
    return KErrNone;
    }

TInt COMADSFolderObject::Characters( const TDesC8& aBuf, TInt aStart, TInt aLength )
    {
    LOGGER_ENTERFN( "COMADSFolderObject::Characters" );
    LOG(aBuf);    
    
    TInt error;
    
    if( iCurrentElement.Compare( KOMADSFolderElement ) == 0 )
        {
        LOGGER_LEAVEFN( "COMADSFolderObject::Characters" );
        return KErrNone;
        }
    
    else if ( iCurrentElement.Compare( KOMADSNameElement ) == 0 )
        {                
        CnvUtfConverter converter;
        TBuf<KMaxFolderNameLength> buf;
        error = converter.ConvertToUnicodeFromUtf8( buf, aBuf.Mid( aStart, aLength ) );
        TInt length = buf.Length() + iName.Length();
        
        if ( error > 0 )
            {
            LOGGER_WRITE_1("Too long name, number of uncorverted bytes: %d", error);
            }
        else if ( error != KErrNone )
            {
            LOGGER_WRITE_1("ConvertToUnicodeFromUtf8 failed with %d", error);
            iError = error;
            }
        else if ( length > iName.MaxLength() )
            {
            LOGGER_WRITE_1("Too long name total: %d", length);
            }
        else
            {
            iName.Append( buf );
            }       
        }
    else if ( iCurrentElement.Compare( KOMADSCreatedElement ) == 0 )
        {
        error = ParseDateString( aBuf.Mid( aStart, aLength ), iCreatedDate );
        if( error != KErrNone )
            {
            iError = error;
            }
        }
    else if ( iCurrentElement.Compare( KOMADSModifiedElement ) == 0 )
        {
        error = ParseDateString( aBuf.Mid( aStart, aLength ), iModifiedDate );
        if ( error != KErrNone )
            {
            iError = error;
            }
        }
    LOGGER_LEAVEFN( "COMADSFolderObject::Characters" );
    return KErrNone;
    }

TInt COMADSFolderObject::ParseDateString(const TDesC8& aString, TDateTime& aDateTime)
    {
    // Ensure we don't read beyond the buffer limits
    if ( aString.Length() < 15 )
        {
        return KErrArgument;
        }
    // Extract the fields from the string
    TLex8 yearDesc( aString.Mid( 0, 4 ) );
    TLex8 monthDesc( aString.Mid( 4, 2 ) );
    TLex8 dayDesc( aString.Mid( 6, 2 ) );
    // Skip one character here, it's supposed to be 'T'
    TLex8 hourDesc( aString.Mid( 9, 2 ) );
    TLex8 minuteDesc( aString.Mid( 11, 2 ) );
    TLex8 secondDesc( aString.Mid( 13, 2 ) );
    

    TInt year, month, day, hour, minute, second;
    TInt error;
    
    // Fetch the values to temporary variables
    if ( ( error = yearDesc.Val(year) ) != KErrNone )
        return error;
    if ( ( error = monthDesc.Val(month) ) != KErrNone )
        return error;
    if ( ( error = dayDesc.Val(day) ) != KErrNone )
        return error;
    if ( ( error = hourDesc.Val(hour) ) != KErrNone )
        return error;
    if ( ( error = minuteDesc.Val(minute) ) != KErrNone )
        return error;
    if ( ( error = secondDesc.Val(second) ) != KErrNone )
        return error;

    // Assign values to datetime object
    if ( ( error = aDateTime.SetYear(year) ) != KErrNone )
        return error;
    if ( ( error = aDateTime.SetMonth((TMonth)(month-1) )) != KErrNone )
        return error;
    if ( ( error = aDateTime.SetDay(day) ) != KErrNone )
        return error;
    if ( ( error = aDateTime.SetHour(hour) ) != KErrNone )
        return error;
    if ( ( error = aDateTime.SetMinute(minute) ) != KErrNone )
        return error;
    if ( ( error = aDateTime.SetSecond(second) ) != KErrNone )
        return error;
        
    return KErrNone;
    }

void COMADSFolderObject::Reset()
    {
    iError = KErrNone;
    iName.SetLength( 0 );
    iCreatedDate.Set( 2005, EJanuary, 1, 0, 0, 0, 0 );
    iModifiedDate.Set( 2005, EJanuary, 1, 0, 0, 0, 0 );
    }
    
TInt COMADSFolderObject::Error( TInt aErrorCode )
    {
    iError = aErrorCode;
    return KErrNone;
    }
