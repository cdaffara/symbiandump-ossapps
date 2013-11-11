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




#ifndef CREATORSCRIPTPARSER_H_
#define CREATORSCRIPTPARSER_H_

#include <e32base.h>
#include <xml/contenthandler.h>
#include <xml/parser.h>

using namespace Xml;

class CCreatorScriptElement;
class CCreatorEngine;
class CCreatorScriptElementCache;

class TCreatorScriptElementInfo
{
public:
    enum TElementStatus
    {
    EStatusUndefined,   // undefined
    EStatusParsing,     // currently being parsed
    EStatusParsed       // parsed
    };
    TCreatorScriptElementInfo()
        {
        iStatus = EStatusUndefined;
        iElement = 0;
        }
    TCreatorScriptElementInfo(TElementStatus aStatus, CCreatorScriptElement* aElement)
        {
        iStatus = aStatus;
        iElement = aElement;
        };
    
    TElementStatus iStatus;    
    CCreatorScriptElement* iElement;
};

class CCreatorScriptParser : public CBase, public MContentHandler
{
public:

    virtual ~CCreatorScriptParser();
    static CCreatorScriptParser* NewL(CCreatorEngine* aEngine);
    static CCreatorScriptParser* NewLC(CCreatorEngine* aEngine);    
    void ParseL(const TDesC& aFileName);
    void ParseL(RFile& aFile);
    
    TInt GetError() const;
    
private:

    CCreatorScriptParser();
    void ConstructL(CCreatorEngine* aEngine);
    
    /**
     * Converts 8 bit descriptor to 16 bit descriptor
     * @param aDes 8 bit descriptor to be converted
     * @return Pointer to the new 16 bit descriptor     
     */
    HBufC* Convert8BitTo16BitLC(const TDesC8& aInput);
    const TCreatorScriptElementInfo& LastElementInfo() const;
    void GetTextFileMode(RFile& aFile, TInt& aFileSize);

    
public:
    // From MContentHandler:    
    void OnStartDocumentL(const RDocumentParameters &aDocParam, TInt aErrorCode);
    void OnEndDocumentL(TInt aErrorCode);
    void OnStartElementL(   const RTagInfo& aElement, 
                            const RAttributeArray& aAttributes, 
                            TInt aErrorCode);
    void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
    void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    void OnStartPrefixMappingL( const RString& aPrefix, 
                                const RString& aUri, 
                                TInt aErrorCode);
    void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
    void OnProcessingInstructionL(  const TDesC8& aTarget, 
                                    const TDesC8& aData, 
                                    TInt aErrorCode);
    void OnError(TInt aErrorCode);
    TAny* GetExtendedInterface(const TInt32 aUid);
    
private:

    enum TFileFormat
        {
        EFormatANSIASCII,
        EFormatUTF8,
        EFormatUTF16LE,
        EFormatUTF16BE
        };
    // XML parser
    CParser* iParser;
    
    /**
     * Element stack holding the elements that are currently being parsed.
     */
    RArray<TCreatorScriptElementInfo> iElementStack;
    
    /**
     * Cache object containig the elements that are referred in future and thus should
     * not be deleted after parsing.
     */
    CCreatorScriptElementCache* iElementCache;
    
    /**
     * Pointer to the engine
     */
    CCreatorEngine* iEngine;
    
    TCreatorScriptElementInfo iDefaultElement;
    
    TFileFormat iScriptTextFormat;
    TInt iLastError;
};

#endif /*CREATORSCRIPTPARSER_H_*/
