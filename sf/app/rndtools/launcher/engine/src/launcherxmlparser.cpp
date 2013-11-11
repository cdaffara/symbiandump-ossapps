/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <xml/documentparameters.h>
#include "launcherxmlparser.h"
#include "launchertraces.h"

_LIT8(KXmlMimeType, "text/xml");

/**
 * XML element names
 */
_LIT8(KDll, "dll");
_LIT8(KDllName, "dllname");
_LIT8(KUID1, "uid1");
_LIT8(KUID2, "uid2");
_LIT8(KUID3, "uid3");
_LIT8(KSID, "sid");
_LIT8(KCapability, "capability");

// ---------------------------------------------------------------------------

CLauncherXMLParser::CLauncherXMLParser(RFs& aFs)
:
CActive( EPriorityIdle ),
iParser(0),
iFileSession(aFs),
iParsedBytes(0)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::ConstructL()
    {
    LOGSTRING("Launcher: CLauncherXMLParser::ConstructL");
    }

// ---------------------------------------------------------------------------

CLauncherXMLParser* CLauncherXMLParser::NewL(RFs& aFs)
    {
    LOGSTRING("Launcher: CLauncherXMLParser::NewL");
    CLauncherXMLParser* self = CLauncherXMLParser::NewLC(aFs);    
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------

CLauncherXMLParser* CLauncherXMLParser::NewLC(RFs& aFs)
    {
    LOGSTRING("Launcher: CLauncherXMLParser::NewLC");
    CLauncherXMLParser* self = new (ELeave) CLauncherXMLParser(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------

CLauncherXMLParser::~CLauncherXMLParser()
    {
    LOGSTRING("Launcher: CLauncherXMLParser::~CLauncherXMLParser");
    Cancel();
    delete iParser;
    delete iCurrentDllElement;    
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::ClearXMLDataBuffer()
    {
    iXMLDataBuffer.Zero();
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::DoCancel()
    {
    LOGSTRING("Launcher: CLauncherXMLParser::DoCancel");
    iIgnoreError = ETrue;
    if( iParser )
        {
        TRAP_IGNORE(iParser->ParseEndL());
        }
    iFile.Close();
    ClearXMLDataBuffer();
    }

// ---------------------------------------------------------------------------
// Gives current buffer content to parser for processing. Then reads
// next part of the file in the buffer and waits for next scheduled run.

void CLauncherXMLParser::RunL()
    {
    TInt err = iStatus.Int();
    LOGSTRING2("Launcher: CLauncherXMLParser::RunL - iStatus: %d", err);
    if( err != KErrNone )
        {
        iObserver->DocumentParsedL(err); // Observer should cancel parsing        
        iFile.Close();
        ClearXMLDataBuffer();
        }
    else
        {        
        iParsedBytes += iXMLDataBuffer.Length();
        iObserver->ParsingProgressedL(iParsedBytes);
        // Check if the end of the file is reached:
        if( iXMLDataBuffer.Length() > 0)
            {
            // Parse the data in buffer 
            LOGSTRING("Launcher: CLauncherXMLParser::RunL: Starting XML parsing");
            if( iParser )
                {
                iParser->ParseL( iXMLDataBuffer );
                }
            else
                {
                LOGSTRING("CLauncherXMLParser::RunL - Error: NULL parser");
                User::Leave(KErrGeneral);
                }
            // Read new data from XML file to buffer:
            iFile.Read( iXMLDataBuffer, KXMLBufferSize, iStatus );
            SetActive();
            }
        else
            {
            // End of the file reached. Stop parsing and close the file:
            LOGSTRING("Launcher: CLauncherXMLParser::RunL: Data parsed. Stopping");
            if( iParser )
                {
                iParser->ParseEndL();
                }
            iFile.Close();
            ClearXMLDataBuffer();
            }    
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::ParseL(const TDesC& aFilePath, MLauncherParserObserver* aObserver)
    {
    LOGSTRING2("Launcher: CLauncherXMLParser::ParseL: %S", &aFilePath);
    iIgnoreError = EFalse;
    iParsedBytes = 0;
    if ( IsActive() )
        {
        Cancel();
        }
    
    if( aObserver == 0 )
        {
        LOGSTRING("Launcher: CLauncherXMLParser::ParseL: Error: Observer is a NULL pointer.");
        User::Leave(KErrArgument);
        }
    
    // Open the XML-file
    TInt err = iFile.Open( iFileSession, aFilePath, EFileRead );
    User::LeaveIfError( err );
    
    // Create and start XML-parser:
    delete iParser;
    iParser = 0;
    iParser = CParser::NewL(KXmlMimeType, *this);        
        
    // Set observer:
    iObserver = aObserver;
    
    // Initialize the buffer and read first part of the XML-file:
    ClearXMLDataBuffer();
    iFile.Read( iXMLDataBuffer, KXMLBufferSize, iStatus );
    SetActive();   
    iParser->ParseBeginL(); // Reset the parser to xml-filetype
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnStartDocumentL(const Xml::RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
    {    
    LOGSTRING2("Launcher: CLauncherXMLParser::OnStartDocumentL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnEndDocumentL(TInt aErrorCode)
    {    
    LOGSTRING2("Launcher: CLauncherXMLParser::OnEndDocumentL (Error code: %d)", aErrorCode);    
    iObserver->DocumentParsedL(aErrorCode);
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnStartPrefixMappingL( const RString& /*aPrefix*/, 
                                                const RString& /*aUri*/, 
                                                TInt aErrorCode)
    {    
    LOGSTRING2("Launcher: CLauncherXMLParser::OnStartPrefixMappingL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
    {
    LOGSTRING2("Launcer: CLauncherXMLParser::OnEndPrefixMappingL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
    {
    LOGSTRING2("Launcher: CLauncherXMLParser::OnIgnorableWhiteSpaceL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
    {
    LOGSTRING2("Launcher: CLauncherXMLParser::OnSkippedEntityL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnProcessingInstructionL(  const TDesC8& /*aTarget*/, 
                                    const TDesC8& /*aData*/, 
                                    TInt aErrorCode)
    {
    LOGSTRING2("Launcher: CLauncherXMLParser::OnProcessingInstructionL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnError(TInt aErrorCode)
    {
    LOGSTRING2("Launcher: CLauncherXMLParser::OnError: %d", aErrorCode);    
    if( iIgnoreError == EFalse )
        {        
        TRAP_IGNORE(iObserver->DocumentParsedL(aErrorCode));
        }
    }

// ---------------------------------------------------------------------------

TAny* CLauncherXMLParser::GetExtendedInterface(const TInt32 aUid)
    {
    (void)aUid;
    LOGSTRING2("Launcher: CLauncherXMLParser::GetExtendedInterface (UID: %d)", aUid);
    return 0;
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnStartElementL(   const RTagInfo& aElement,
                                            const RAttributeArray& /*aAttributes*/,
                                            TInt aErrorCode)
    {
    LOGSTRING2("Launcher: CLauncherXMLParser::OnStartElementL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    else
        {
        // Save XML-element name:
        iCurrentElementName = aElement.LocalName().DesC();
    
        // If this is 'dll'-element, initialize new DLL element instance:
        if( iCurrentDllElement == 0 && iCurrentElementName == KDll )
            {
            iCurrentDllElement = CLauncherDLLElement::NewL();
            }
        // Clear contents buffer:
        iCurrentContent.Zero();
        }
    }

// ---------------------------------------------------------------------------

void CLauncherXMLParser::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
    {   
    LOGSTRING2("Launcher: CLauncherXMLParser::OnEndElementL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    else
        {
        // Save XML-element's name:        
        iCurrentElementName = aElement.LocalName().DesC();    
        TUid tmpUID;

        if( IsDataElement() )
            {        
            // Check that we have a pointer to parent DLL element
            if( iCurrentDllElement == 0 )
                {
                LOGSTRING("Launcher: CLauncherXMLParser: Error in parsing xml (parent DLL element missing).");
                User::Leave(KErrGeneral);
                }
            // DLL name
            if( iCurrentElementName == KDllName )
                {
                TFileName dllName;
                dllName.Copy(iCurrentContent);
                iCurrentDllElement->SetNameL(dllName);
                }
            // UID1
            else if( iCurrentElementName == KUID1 )
                {
                tmpUID.iUid = ConvertDes8ToUint32L(iCurrentContent);
                iCurrentDllElement->SetUID1L(tmpUID);
                }
            // UID2
            else if( iCurrentElementName == KUID2 )
                {
                tmpUID.iUid = ConvertDes8ToUint32L(iCurrentContent);
                iCurrentDllElement->SetUID2L(tmpUID);
                }
            // UID3
            else if( iCurrentElementName == KUID3 )
                {
                tmpUID.iUid = ConvertDes8ToUint32L(iCurrentContent);
                iCurrentDllElement->SetUID3L(tmpUID);
                }
            // SID
            else if( iCurrentElementName == KSID )
                {
                tmpUID.iUid = ConvertDes8ToUint32L(iCurrentContent);
                iCurrentDllElement->SetSIDL(tmpUID);
                }
            // Capability
            else if( iCurrentElementName == KCapability )
                {            
                iCurrentDllElement->SetCapabilityL(ConvertDes8ToUint32L(iCurrentContent));
                }
            }
        else if( iCurrentElementName == KDll )
            {
            // DLL element parsed, give current DLL object to observer:
            iObserver->ElementParsedL(*iCurrentDllElement);
            }
        }
    }

// ---------------------------------------------------------------------------

TBool CLauncherXMLParser::IsDataElement()
    {
    if( iCurrentElementName == KDllName ||
        iCurrentElementName == KUID1 ||
        iCurrentElementName == KUID2 ||
        iCurrentElementName == KUID3 ||
        iCurrentElementName == KSID ||
        iCurrentElementName == KCapability )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Reads content of an xml-element.

void CLauncherXMLParser::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
    LOGSTRING2("Launcher: CLauncherXMLParser::OnContentL (Error code: %d)", aErrorCode);
    if( aErrorCode != KErrNone )
        {
        iObserver->DocumentParsedL(aErrorCode);
        }
    else if( iCurrentElementName.Length() == 0)
        {
        LOGSTRING("Launcher: CLauncherXMLParser: Error in parsing xml (element name missing).");
        User::Leave(KErrGeneral);
        }
    iCurrentContent.Append(aBytes);
    }

// ---------------------------------------------------------------------------

TUint32 CLauncherXMLParser::ConvertDes8ToUint32L(const TDesC8& aStr)
    {
    LOGSTRING("Launcher: CLauncherXMLParser::ConvertDes8ToUintL");
    TUint32 uintVal = 0;
    TLex8 lex(aStr);
    TInt errorCode=lex.Val(uintVal, EHex);
    User::LeaveIfError(errorCode);
    return uintVal;
    }
