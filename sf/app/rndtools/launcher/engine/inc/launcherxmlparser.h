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

#ifndef LAUNCHERXMLPARSER_H_
#define LAUNCHERXMLPARSER_H_

#include <e32base.h>
#include <xml/contenthandler.h>
#include <xml/parser.h>
#include "launcherdllelement.h"
#include "launcherparserobserver.h"

using namespace Xml;

const TUint KXMLBufferSize = 1024;

class CLauncherDLLElement;

/**
 * Parses DLL data from the given XML-file
 */
class CLauncherXMLParser : public CActive, public MContentHandler
{
public:

    virtual ~CLauncherXMLParser();
    static CLauncherXMLParser* NewL(RFs& aFs);
    static CLauncherXMLParser* NewLC(RFs& aFs);
    
    /**
     * Parses the given XML-file and notifies the observer.
     * @param aFilePath XML-file's path
     * @param aObserver Pointer to the observer instance.
     */
    void ParseL(const TDesC& aFilePath, MLauncherParserObserver* aObserver);
        
protected:
    
    // from CActive    
    void DoCancel();    
    void RunL();
    
private:

    CLauncherXMLParser(RFs& aFs);
    void ConstructL();
    
    /**
     * Tells whether the current XML element is a sub-element of the 'dll'-element.
     * @return ETrue if the current XML-element is sub-element of the 'dll'-element.
     */
    TBool IsDataElement();

    /**
     * Deletes current data buffer and zeroes the pointer.
     */
    void ClearXMLDataBuffer();
    
    /**
     * Converts 8-bit descriptor to 32-bit unsigned integer
     * @return Converted 32-bit unsigned integer value.
     * @param aStr 8-bit descriptor to be converted.
     */
    TUint32 ConvertDes8ToUint32L(const TDesC8& aStr);
                
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
    CParser* iParser;  // XML parser
    RFs& iFileSession;
    RFile iFile; // XML file 
    TBuf8<KXMLBufferSize> iXMLDataBuffer;
    MLauncherParserObserver* iObserver;
    CLauncherDLLElement* iCurrentDllElement;
    TBuf8<KMaxFileName> iCurrentContent;
    TBuf8<KMaxName> iCurrentElementName;
    TBool iIgnoreError;
    TInt  iParsedBytes;
};

#endif /* LAUNCHERXMLPARSER_H_ */
