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


#ifndef _SCONCONMLHANDLER_H
#define _SCONCONMLHANDLER_H

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "sconconmlgenerator.h"
#include "sconwbxmldefs.h"
#include "sconconmlhandlererror.h"
#include "sconconmldtd.h"

// -----------------------------------------------------------------------------
// Definitions and constants
// -----------------------------------------------------------------------------

_LIT8(KWBXMLNull, "\x00");

const TInt KSConBufferGranularity( 32 );
const TInt KSConDefaultDocumentSize( 2048 );

// -----------------------------------------------------------------------------
// TWBXMLStackItem
// -----------------------------------------------------------------------------
NONSHARABLE_STRUCT( TWBXMLStackItem )
    {
public:
    inline TWBXMLStackItem( TUint8 aTag );
    inline TUint8 Tag() const;

private:
    TUint8 iTag;
    };

#include "sconwbxmlparser.inl"

// -----------------------------------------------------------------------------
// CSConConMLHandler - WBXML parsing
// -----------------------------------------------------------------------------
class CSConConMLHandler : public CBase
    {
public:
    static CSConConMLHandler* NewL();
    ~CSConConMLHandler();
    
    /**
    * Starts the parsing of a WBXML document
    * @param aInput - the WBXML document to be parsed
    * @param aCallback - Call back function to call 
    * after the document is parsed
    * @return An error code
    */
    virtual TInt ParseDocumentL( CBufFlat& aInput,  
        MWBXMLConMLCallback* aCallback  );
    
    /**
    * Starts the generating of a WBXML and XML document
    * @param aContent - the data to be generated into the document
    * @return An error code
    */
    virtual TInt GenerateDocument( ConML_ConMLPtr_t aContent );
    
    /**
    * Returns the generated document in XML -format
    * @param none
    * @return The document as TPtrC8
    */
    virtual TPtrC8 XMLDocument();
    
    /**
    * Returns the generated document in WBXML -format
    * @param none
    * @return The document as TPtrC8
    */
    virtual TPtrC8 WBXMLDocument();
    
private:
    CSConConMLHandler();
    void ConstructL();
    
    TInt ParseL();
    TConMLParserError DoParseDocumentHeaderL();
    TConMLParserError DoParseDocumentBodyL();
    TUint8 ReadUint8L();
    TUint32 ReadMUint32L();
    TPtrC8 ReadStrIL();
    TPtrC8 StringTableString( TUint32 aIndex );
    void ReadStringTableL();    
    TPtrC8 ReadOpaqueL();
    void HandleElementL( TUint8 aId );
    
private:
    CSConConMLGenerator* iGenerator;
    HBufC8* iStringTable;
    CBufBase* iBuffer;
    TBool iHeaderParsed;
    RArray< TUint > iElemStack;
    TInt iPos;
    CBufFlat* iParseBuffer;
    MWBXMLConMLCallback* iCallback;
    };

    IMPORT_C CSConConMLHandler* CreateCSConConMLHandlerL();
    typedef CSConConMLHandler* (*TSConCreateCSConConMLHandlerFunc) (); 
    
#endif
