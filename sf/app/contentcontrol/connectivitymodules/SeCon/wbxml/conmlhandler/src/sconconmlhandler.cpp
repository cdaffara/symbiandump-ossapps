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
* Description:  ConML parser/generator
*
*/


// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <s32mem.h>

#include "sconconmlhandler.h"
#include "debug.h"

// -----------------------------------------------------------------------------
// CreateCSConConMLHandlerL
// Creates a new instance of CSConConMLHandler
// -----------------------------------------------------------------------------
//  
EXPORT_C CSConConMLHandler* CreateCSConConMLHandlerL()
    {
    LOGGER_ENTERFN( "CreateCSConConMLHandlerL()" );
    LOGGER_LEAVEFN( "CreateCSConConMLHandlerL()" );
    return CSConConMLHandler::NewL();
    }
    
// -----------------------------------------------------------------------------
// CSConConMLHandler::CSConConMLHandler()
// Default constructor
// -----------------------------------------------------------------------------
//  
CSConConMLHandler::CSConConMLHandler()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConConMLHandler::CSConConMLHandler()
// Default constructor
// -----------------------------------------------------------------------------
//  
CSConConMLHandler::~CSConConMLHandler()
    {
    LOGGER_ENTERFN( "CSConConMLHandler::~CSConConMLHandler()" );
    
    if ( iParseBuffer )
        {
        LOGGER_WRITE( "CSConConMLHandler::~CSConConMLHandler() : phase 1/4 " );
        delete iParseBuffer;
        iParseBuffer = NULL;
        }
    
    if ( iBuffer )
        {
        LOGGER_WRITE( "CSConConMLHandler::~CSConConMLHandler() : phase 2/4 " );
        delete iBuffer;
        iBuffer = NULL;
        }
    
    if ( iGenerator )
        {
        LOGGER_WRITE( "CSConConMLHandler::~CSConConMLHandler() : phase 3/4 " );
        delete iGenerator;
        iGenerator = NULL;
        }

    if ( iStringTable )
        {
        LOGGER_WRITE( "CSConConMLHandler::~CSConConMLHandler() : phase 4/4 " );
        delete iStringTable;
        iStringTable = NULL;
        }

    iElemStack.Close();
    
    LOGGER_LEAVEFN( "CSConConMLHandler::~CSConConMLHandler()" );
    }
        
// -----------------------------------------------------------------------------
// CSConConMLHandler::CSConConMLHaNewL()
// Creates a new instance of CSConConMLHandler
// -----------------------------------------------------------------------------
//  
CSConConMLHandler* CSConConMLHandler::NewL()
    {
    LOGGER_ENTERFN( "CSConConMLHandler::NewL()" );
    CSConConMLHandler* self = new ( ELeave ) CSConConMLHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOGGER_LEAVEFN( "CSConConMLHandler::NewL()" );
    return self;
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSConConMLHandler::ConstructL()
    {
    LOGGER_ENTERFN( "CSConConMLHandler::ConstructL()" );
    iGenerator = CSConConMLGenerator::NewL();
    iBuffer = CBufFlat::NewL( KSConBufferGranularity );
    iParseBuffer = CBufFlat::NewL( KSConDefaultDocumentSize );
    LOGGER_LEAVEFN( "CSConConMLHandler::ConstructL()" );
    }
    
// -----------------------------------------------------------------------------
// CSConConMLHandler::ParseDocumentL()
// Handles the document parsing, document in aInput
// -----------------------------------------------------------------------------
//  
TInt CSConConMLHandler::ParseDocumentL( 
    CBufFlat& aInput, MWBXMLConMLCallback* aCallback )
    {
    LOGGER_ENTERFN( "CSConConMLHandler::ParseDocument()" );
    TInt ret ( KErrNone );
        
    iPos = 0;
    iParseBuffer->Reset();
    iParseBuffer->ResizeL( aInput.Size() );
    iParseBuffer->Write( 0, aInput.Ptr(0) );
    iHeaderParsed = EFalse;
    iGenerator->SetCallback( aCallback );

    while (ret == KWBXMLParserErrorOk )
        {
        ret = ParseL();
        }

    if ( ret == KWBXMLParserErrorEof ) 
        {
        ret = KErrNone;
        }
    LOGGER_WRITE_1( "CSConConMLHandler::ParseDocument()\
     returned %d", ret );   
    return ( ret );
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::GenerateDocument()
// Starts the generating of a document
// -----------------------------------------------------------------------------
//  
TInt CSConConMLHandler::GenerateDocument( ConML_ConMLPtr_t aContent )
    {
    LOGGER_ENTERFN( "CSConConMLHandler::GenerateDocument()" );
    iGenerator->GenerateConMLDocument ( aContent );
    LOGGER_LEAVEFN( "CSConConMLHandler::GenerateDocument()" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::WBXMLDocument()
// Returns the generated WBXML -document
// -----------------------------------------------------------------------------
//  
TPtrC8 CSConConMLHandler::WBXMLDocument()
    {
    return iGenerator->WBXMLDocument();
    }
    
// -----------------------------------------------------------------------------
// CSConConMLHandler::XMLDocument()
// Returns the generated XML -document
// -----------------------------------------------------------------------------
//  
TPtrC8 CSConConMLHandler::XMLDocument()
    {
    return iGenerator->XMLDocument();
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::ParseL()
// Parses the next document tag / header if not parsed yet
// -----------------------------------------------------------------------------
//  
TInt CSConConMLHandler::ParseL()
    {
    LOGGER_ENTERFN( "CSConConMLHandler::ParseL()" );
    if( !iHeaderParsed )
        {
        TConMLParserError result(KWBXMLParserErrorOk);
        TRAPD(err,  result = DoParseDocumentHeaderL());
        if( err == KErrEof )
            {
            LOGGER_WRITE( "CSConConMLHandler::ParseL() : KWBXMLParserErrorEofTooEarly " );
            return KWBXMLParserErrorEofTooEarly;
            }
        return result;
        }
    LOGGER_LEAVEFN( "CSConConMLHandler::ParseL()" );
    return DoParseDocumentBodyL();
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::DoParseDocumentHeaderL()
// Parses the document header
// -----------------------------------------------------------------------------
//  
TConMLParserError CSConConMLHandler::DoParseDocumentHeaderL()
    {
    LOGGER_ENTERFN( "CSConConMLParser::DoParseDocumentHeaderL()" );
    iHeaderParsed = ETrue;

    // Version
    TUint8 version = ReadUint8L();
    
    // PublicId
    TInt32 publicId = ReadMUint32L();

    if( publicId == 0 )
        {
        publicId = ReadMUint32L();
        }
    else
        {
        publicId = -publicId;
        }

    // Charset
    TUint32 charSet = ReadMUint32L();

    // String table
    ReadStringTableL();

    // Document begins now
    if( publicId < 0 )
        {
        iGenerator->StartDocument(version, -publicId, charSet);
        }
    else
        {
        iGenerator->StartDocument(version, 
            StringTableString(publicId), charSet);
        }
    LOGGER_LEAVEFN( "CSConConMLHandler::DoParseDocumentHeaderL()" );
    return KWBXMLParserErrorOk;
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::DoParseDocumentBodyL()
// Parses the next element of the document
// -----------------------------------------------------------------------------
//  
TConMLParserError CSConConMLHandler::DoParseDocumentBodyL()
    {
    LOGGER_ENTERFN( "CSConConMLHandler::DoParseDocumentBodyL()" );
    TUint8 id(0);
    TRAPD(err, id = ReadUint8L());

    if( err != KErrNone )
        {
        if( err == KErrEof )
            {
            iGenerator->EndDocument();
            if( iElemStack.Count() > 0 )
                {
                LOGGER_WRITE( "CSConConMLHandler::DoParseDocumentBodyL() : KWBXMLParserErrorEofTooEarly " );
                return KWBXMLParserErrorEofTooEarly;
                }
            return KWBXMLParserErrorEof;
            }
        User::Leave(err);
        }
    
    switch (id) 
        {
        case END:
            {
            if( !iElemStack.Count() )
                {
                LOGGER_WRITE( "CSConConMLHandler::DoParseDocumentBodyL() : KWBXMLParserErrorinvalidDocument " );
                User::Leave( KWBXMLParserErrorinvalidDocument );
                }
            TUint8 tag( iElemStack.operator[](iElemStack.Count() - 1 ));
            iGenerator->EndElementL( tag );
            iElemStack.Remove(iElemStack.Count() - 1);
            }
            break;
    
        case STR_I: 
            iGenerator->CharactersL(ReadStrIL());
            break;
        
        case OPAQUE:
            iGenerator->CharactersL(ReadOpaqueL());
            break;

        default: 
            HandleElementL(id);
            break;
        }
    LOGGER_LEAVEFN( "CSConConMLHandler::DoParseDocumentBodyL()" );
    return KWBXMLParserErrorOk;
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::ReadUint8L()
// Reads one byte from the document
// -----------------------------------------------------------------------------
//  
TUint8 CSConConMLHandler::ReadUint8L()
    {
    if ( iPos == iParseBuffer->Size()) 
        {
        User::Leave ( KErrEof );
        }
    TUint8 value;
    HBufC8* data = HBufC8::NewLC( sizeof(TUint8) );
    TPtr8 ptr = data->Des();
    iParseBuffer->Read(iPos, ptr, sizeof(TUint8));
    iPos+= sizeof(TUint8);
    value = ptr[0];
    CleanupStack::PopAndDestroy(1); //data 
    return value;
    }
    
// -----------------------------------------------------------------------------
// CSConConMLHandler::ReadMUint32L()
// Reads multibyte coding from the document
// -----------------------------------------------------------------------------
//  
TUint32 CSConConMLHandler::ReadMUint32L()
    {
    TUint32 result = 0;
    TUint8 c;
    
    do  {
        c = ReadUint8L();
        result = (result << 7) | (c & 0x7f);
        } while ( c & 0x80 );
    
    return result;
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::ReadStrIL()
// Reads string data from the document
// -----------------------------------------------------------------------------
//  
TPtrC8 CSConConMLHandler::ReadStrIL()
    {
    iBuffer->Reset();
    RBufWriteStream bws(*iBuffer);
    TUint8 c;
    while( (c = ReadUint8L()) != 0 )
        {
        bws.WriteUint8L(c);
        }
    bws.CommitL();
    return iBuffer->Ptr(0);
    }
    
// -----------------------------------------------------------------------------
// CSConConMLHandler::ReadStringTableL()
// Handles the document's string table
// -----------------------------------------------------------------------------
//  
void CSConConMLHandler::ReadStringTableL()
    {
    delete iStringTable;
    iStringTable = NULL;
    TUint32 strTblLen = ReadMUint32L();

    if( strTblLen > 0 )
        {
        if( iPos+strTblLen > iParseBuffer->Size()) 
            {
            LOGGER_WRITE( "CSConConMLHandler::ReadStringTableL() : LEAVE KErrEof " );
            User::Leave ( KErrEof );
            }
        iStringTable = HBufC8::NewL(strTblLen);
        TPtr8 ptr = iStringTable->Des();
        iParseBuffer->Read(iPos, ptr, strTblLen);
        iPos+=strTblLen;
        }
    }
    
// -----------------------------------------------------------------------------
// CSConConMLHandler::StringTableString()
// Returns a value from the string table according to the given index
// -----------------------------------------------------------------------------
//  
TPtrC8 CSConConMLHandler::StringTableString( TUint32 aIndex )
    {
    TPtrC8 temp(iStringTable->Mid(aIndex));
    TInt pos = temp.Find(KWBXMLNull());
    if( pos != KErrNotFound )
        {
        temp.Set(temp.Left(pos));
        }
    return temp;
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::HandleElementL()
// Starts a new element in the document
// -----------------------------------------------------------------------------
//  
void CSConConMLHandler::HandleElementL( TUint8 aId )
    {
    TUint8 tag(TUint8(aId & 0x3f));
        
    iGenerator->StartElementL( tag );

    if( aId & 0x40 ) 
        {
        iElemStack.Append( tag );
        }
    else 
        {
        iGenerator->EndElementL(tag);
        }
    }

// -----------------------------------------------------------------------------
// CSConConMLHandler::ReadOpaqueL()
// Reads opaque data from the document
// -----------------------------------------------------------------------------
//  
TPtrC8 CSConConMLHandler::ReadOpaqueL()
    {
    iBuffer->Reset();
    RBufWriteStream bws(*iBuffer);
    TUint32 length = ReadMUint32L();
    if( iPos+length > iParseBuffer->Size()) 
        {
        LOGGER_WRITE( "CSConConMLHandler::ReadOpaqueL() : LEAVE KErrEof " );
        User::Leave ( KErrEof );
        }
    HBufC8* data = HBufC8::NewLC( length );
    TPtr8 ptr = data->Des();
    iParseBuffer->Read( iPos, ptr, length );
    iPos+= length;
    bws.WriteL(ptr, length);
    bws.CommitL();
    CleanupStack::PopAndDestroy(1); // data
    
    return iBuffer->Ptr(0);
    }

