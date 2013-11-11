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
#include <utf.h>
#include <xml/documentparameters.h>
#include "creator_scriptparser.h"
#include "creator_scriptelementfactory.h"
#include "creator_traces.h"

using namespace Xml;

CCreatorScriptParser::CCreatorScriptParser() 
: iDefaultElement(TCreatorScriptElementInfo(TCreatorScriptElementInfo::EStatusUndefined, 0))
    {        
    }
void CCreatorScriptParser::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorScriptParser::ConstructL");
    iEngine = aEngine;
    }

CCreatorScriptParser* CCreatorScriptParser::NewL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorScriptParser::NewL");
    CCreatorScriptParser* self = CCreatorScriptParser::NewLC(aEngine);    
    CleanupStack::Pop();
    return self;
    }
CCreatorScriptParser* CCreatorScriptParser::NewLC(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorScriptParser::NewLC");
    CCreatorScriptParser* self = new (ELeave) CCreatorScriptParser;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorScriptParser::~CCreatorScriptParser()
    {
    delete iParser;
    if( iElementStack.Count() > 0 )
        {
        delete iElementStack[0].iElement;        
        }
    iElementStack.Reset();
    iElementStack.Close();
    }

void CCreatorScriptParser::GetTextFileMode(RFile& aFile, TInt& aFileSize) 
    { 
    LOGSTRING("Creator: CCreatorScriptParser::GetTextFileMode");
    iScriptTextFormat = EFormatANSIASCII; 

    // if we are working with text files, check the type first   
    TBuf8<4> BOM; 

    // first check for UTF-16 
    if (aFileSize >= 2 && aFile.Read(0, BOM, 2) == KErrNone) 
        { 
        if (BOM.Length()==2 && BOM[0]==0xFF && BOM[1]==0xFE) 
            { 
            iScriptTextFormat = EFormatUTF16LE; 
            aFileSize -= 2; 
            return; 
            } 
        else if (BOM.Length()==2 && BOM[0]==0xFE && BOM[1]==0xFF) 
            { 
            iScriptTextFormat = EFormatUTF16BE; 
            aFileSize -= 2; 
            return;                 
            }       
        }
        // then check for UTF-8 
    if (aFileSize >= 3 && aFile.Read(0, BOM, 3) == KErrNone) 
        { 
        if (BOM.Length()==3 && BOM[0]==0xEF && BOM[1]==0xBB && BOM[2]==0xBF) 
            { 
            iScriptTextFormat = EFormatUTF8; 
            aFileSize -= 3; 
            return; 
            } 
        }
        
    // none of those, seek back to beginning 
    TInt pos(0); 
    aFile.Seek(ESeekStart, pos); 
    }    
 


void CCreatorScriptParser::ParseL(const TDesC& aFileName)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::ParseL: %S", &aFileName);
    // Create and start XML-parser
    if( iParser == 0)
        {
        _LIT8(KXmlMimeType, "text/xml");
        iParser = CParser::NewL(KXmlMimeType, *this);        
        }

    RFs filesession;
    CleanupClosePushL(filesession);
    User::LeaveIfError( filesession.Connect() );
    Xml::ParseL(*iParser, filesession, aFileName);
    
    CleanupStack::PopAndDestroy(&filesession); // filesession    
    }

void CCreatorScriptParser::ParseL(RFile& aFile)
    {
    LOGSTRING("Creator: CCreatorScriptParser::ParseL");
    TInt filesize = 0;
    aFile.Size(filesize);
    GetTextFileMode(aFile, filesize);
    // Create and start XML-parser    
    if( iParser == 0)
        {
        _LIT8(KXmlMimeType, "text/xml");
        iParser = CParser::NewL(KXmlMimeType, *this);        
        }
    Xml::ParseL(*iParser, aFile);
    }

void CCreatorScriptParser::OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnStartDocumentL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);    
    }

void CCreatorScriptParser::OnEndDocumentL(TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnEndDocumentL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    iEngine->SortCommands();
    }

void CCreatorScriptParser::OnStartPrefixMappingL(   const RString& /*aPrefix*/, 
                                                    const RString& /*aUri*/, 
                                                    TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnStartPrefixMappingL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }

void CCreatorScriptParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnEndPrefixMappingL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }
void CCreatorScriptParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnIgnorableWhiteSpaceL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }
void CCreatorScriptParser::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnSkippedEntityL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }
void CCreatorScriptParser::OnProcessingInstructionL(  const TDesC8& /*aTarget*/, 
                                    const TDesC8& /*aData*/, 
                                    TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnProcessingInstructionL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }
void CCreatorScriptParser::OnError(TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnError: %d", aErrorCode);
    
    iLastError = aErrorCode;
    
    // Delete all elements from the stack:
    while(iElementStack.Count() > 0 )
        {
        TCreatorScriptElementInfo previousElem = LastElementInfo();    
        delete previousElem.iElement;
        previousElem.iElement = 0;
        iElementStack.Remove(iElementStack.Count()-1);
        }
    }
TAny* CCreatorScriptParser::GetExtendedInterface(const TInt32 aUid)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::GetExtendedInterface (UID: %d)", aUid);
    return this;
    }

const TCreatorScriptElementInfo& CCreatorScriptParser::LastElementInfo() const
{
    LOGSTRING("Creator: CCreatorScriptParser::LastElementInfo");
    if(iElementStack.Count() > 0)
        {
        return iElementStack[iElementStack.Count()-1];
        }
    return iDefaultElement;
}

/**
 * Create an element and add attributes to it.
 */
void CCreatorScriptParser::OnStartElementL(  const RTagInfo& aElement,
                                            const RAttributeArray& aAttributes,
                                            TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnStartElementL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    const TDesC8& elementName8 = aElement.LocalName().DesC();        
    HBufC* elementName16 = Convert8BitTo16BitLC(elementName8);
        
    const TCreatorScriptElementInfo& context = LastElementInfo();
    
    CCreatorScriptElement* element = 0;
    if( context.iElement )
        {
        HBufC* fullContext = HBufC::NewL(
                context.iElement->Context().Length() + 
                context.iElement->Name().Length() + 
                2);        
        CleanupStack::PushL(fullContext);
        if(context.iElement->Context() != KNullDesC && 
           context.iElement->Context().Length() > 0 )
            {
            fullContext->Des().Copy(context.iElement->Context());
            _LIT(KContextSeparator, "::");
            fullContext->Des().Append(TPtrC(KContextSeparator));
            }
        const TDesC& parentName = context.iElement->Name();
        if(parentName.Length() > 0 )
            {            
            fullContext->Des().Append(parentName);
            }
        element = TCreatorScriptElementFactory::CreateElementL( iEngine,
                                                                elementName16->Des(), 
                                                                fullContext->Des());
        CleanupStack::PopAndDestroy(fullContext); // fullContext        
        }
    else
        {
        element = TCreatorScriptElementFactory::CreateElementL(iEngine, elementName16->Des());        
        }
        
    CleanupStack::PopAndDestroy(elementName16);
    CleanupStack::PushL(element);
    
    if( element == 0 )
        {
        // Add NULL pointer to stack. Otherwise the stack would go out of sync. in ::OnEndElement().
        TCreatorScriptElementInfo info(TCreatorScriptElementInfo::EStatusParsing, element);
        iElementStack.AppendL(info);   
        CleanupStack::Pop(element);
        return;        
        }
        
    // Add attributes to the element:
    TInt attrCount = aAttributes.Count();
    for(TInt i = 0; i < attrCount; ++i)
        {
        const RAttribute& attribute = aAttributes[i];
        const RTagInfo& nameInfo = attribute.Attribute();
        const TDesC8& attrName8 = nameInfo.LocalName().DesC();
        const TDesC8& value8 = attribute.Value().DesC();
        // Convert data to 16 bit:
        HBufC* attrName16 = Convert8BitTo16BitLC(attrName8);
        HBufC* value16 = Convert8BitTo16BitLC(value8);
        CCreatorScriptAttribute* scriptAttr = CCreatorScriptAttribute::NewLC(attrName16->Des(), value16->Des());
        element->AddAttributeL(scriptAttr);
        CleanupStack::Pop(scriptAttr);
        CleanupStack::PopAndDestroy(value16);
        CleanupStack::PopAndDestroy(attrName16);
        }
    
    // Save element to the stack:
    TCreatorScriptElementInfo info(TCreatorScriptElementInfo::EStatusParsing, element);
    iElementStack.AppendL(info);
    CleanupStack::Pop(element);
    }

/**
 * Executes the command if needed. Also caches the element for future use, if needed.
 */
void CCreatorScriptParser::OnEndElementL(const RTagInfo& /*aElement*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnEndElementL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
     
    if( iElementStack.Count() == 0 )
        {
        return;
        }
    // Get element from the stack:    
    TCreatorScriptElementInfo elementInfo = LastElementInfo();
    
    if( elementInfo.iElement == 0 )
        {
        // Remove element from the stack:        
        iElementStack.Remove(iElementStack.Count()-1);
        return;
        }
        
    // Execute the command if this is a command element:
    if( elementInfo.iElement->IsCommandElement() )
        {
        elementInfo.iElement->ExecuteCommandL();
        }
    
    // Remove element from the stack:
    iElementStack.Remove(iElementStack.Count()-1);
    
    // Get the previous element from the stack:
    if( iElementStack.Count() > 0 )
        {
        TCreatorScriptElementInfo previousElem = LastElementInfo();
        
        if( previousElem.iElement->IsRoot() )
            {
            // This element is not a sub-element (except of the root element, of course).        
            if( elementInfo.iElement->IsCacheNeeded() )
                {
                // Add element to the cache, since it will be needed in future.            
                elementInfo.iElement->AddToCacheL();
                }
            //else
            //    {
                // Not needed anymore, so element can be deleted:
                delete elementInfo.iElement;
                elementInfo.iElement = 0;
           //     }
            }
        else
            {
            // There is a parent element (other than root), so we add this element
            // as a sub-element fot it.
            previousElem.iElement->AddSubElementL(elementInfo.iElement);
            }
        }
    else
        {
        // Delete root element, which will delete recursively all sub-elements:
        delete elementInfo.iElement;
        elementInfo.iElement = 0;
        }
    }

/**
 * Add content to element.
 */
void CCreatorScriptParser::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorScriptParser::OnContentL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    if( iElementStack.Count() > 0 )
        {
        HBufC* contentData = Convert8BitTo16BitLC(aBytes);
        TCreatorScriptElementInfo elementInfo = iElementStack[iElementStack.Count()-1];
        if( elementInfo.iElement )
            {
            elementInfo.iElement->AppendContentL(*contentData);
            }
        CleanupStack::PopAndDestroy(contentData);
        }
    }

HBufC16* CCreatorScriptParser::Convert8BitTo16BitLC(const TDesC8& aInput) 
    {     
    LOGSTRING("Creator: CCreatorScriptParser::Convert8BitTo16BitLC");
    HBufC16* output = NULL;
    
    output = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aInput);
    
    CleanupStack::PushL(output);
    return output;          
    } 

TInt CCreatorScriptParser::GetError() const 
{
    return iLastError;
}
