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




#ifndef CREATORRANDOMDATAPARSER_H_
#define CREATORRANDOMDATAPARSER_H_

#include <e32base.h>
#include <badesca.h>
#include <xml/contenthandler.h>
#include <xml/parser.h>

//using namespace Xml;

class CCreatorRandomDataParser : public CBase, public Xml::MContentHandler
	{

public:
    enum TRandomDataType
    	{
    	EFirstName,
    	ESurname,
    	ECompany,
    	EAddress,
    	EJobTitle,
    	EPhoneNumber,
    	EGroupName,
    	EMeetingReason,
    	EMeetingPlace,
    	EMemoText,
    	EAnniversaryReason,
    	EToDoText,
    	EReminderText,
    	EMessageSubject,
    	EMessageText,
    	ECity,
    	ECountry,
    	EPostcode,
    	EState,
    	EPobox,
    	EPrefix,
    	ESuffix,
    	ELandmarkName,
    	ELandmarkDescription
    	};

public:

    virtual ~CCreatorRandomDataParser();
    static CCreatorRandomDataParser* NewL();
    static CCreatorRandomDataParser* NewLC();    
    CDesCArrayFlat* ParseL(const TDesC& aFileName, const TRandomDataType aDataType);
    
    TInt GetError() const;
    
private:

    CCreatorRandomDataParser();
    void ConstructL();
    
    /**
     * Converts 8 bit descriptor to 16 bit descriptor
     * @param aDes 8 bit descriptor to be converted
     * @return Pointer to the new 16 bit descriptor     
     */
    HBufC* Convert8BitTo16BitLC(const TDesC8& aInput);
    void GetTextFileMode(RFile& aFile, TInt& aFileSize);

    
public:
    // From MContentHandler:    
    void OnStartDocumentL(const Xml::RDocumentParameters &aDocParam, TInt aErrorCode);
    void OnEndDocumentL(TInt aErrorCode);
    void OnStartElementL(const Xml::RTagInfo& aElement, 
                         const Xml::RAttributeArray& aAttributes, 
                         TInt aErrorCode);
    void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
    void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    void OnStartPrefixMappingL(const RString& aPrefix, 
                               const RString& aUri, 
                               TInt aErrorCode);
    void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
    void OnProcessingInstructionL(const TDesC8& aTarget, 
                                  const TDesC8& aData, 
                                  TInt aErrorCode);
    void OnError(TInt aErrorCode);
    TAny* GetExtendedInterface(const TInt32 aUid);
    
private:
	TRandomDataType iDataType;
	CDesCArray* iElementNameArray;
	HBufC* iContentData;
	TBool iInsideRootElement;
	TBool iContentParsingActive;

	CDesCArrayFlat* iResultArray;
	
    enum TFileFormat
        {
        EFormatANSIASCII,
        EFormatUTF8,
        EFormatUTF16LE,
        EFormatUTF16BE
        };
    
    // XML parser
    Xml::CParser* iParser;
    
    TFileFormat iScriptTextFormat;
    TInt iLastError;
};

#endif /*CREATORRANDOMDATAPARSER_H_*/
