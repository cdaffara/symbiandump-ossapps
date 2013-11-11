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
#include "creator_randomdataparser.h"
#include "creator_traces.h"

// XML element names
_LIT(KRootElement, "creatordata");
_LIT(KFirstName, "firstname");
_LIT(KSurname, "surname");
_LIT(KCompany, "company");
_LIT(KAddress, "address");
_LIT(KJobTitle, "jobtitle");
_LIT(KPhoneNumber, "phonenumber");
_LIT(KGroupName, "groupname");
_LIT(KMeetingReason, "meetingreason");
_LIT(KMeetingPlace, "meetingplace");
_LIT(KMemoText, "memo");
_LIT(KAnniversaryReason, "anniversary");
_LIT(KToDoText, "todo");
_LIT(KReminderText, "reminder");
_LIT(KMessageSubject, "messagesubject");
_LIT(KMessageText, "messagetext");
_LIT(KCity, "city");
_LIT(KCountry, "country");
_LIT(KPostcode, "postcode");
_LIT(KState, "state");
_LIT(KPobox, "pobox");
_LIT(KPrefix, "prefix");
_LIT(KSuffix, "suffix");
_LIT(KLandmarkName, "landmarkname");
_LIT(KLandmarkDescription, "landmarkdescription");

using namespace Xml;

CCreatorRandomDataParser::CCreatorRandomDataParser() 
    {        
    }

void CCreatorRandomDataParser::ConstructL()
    {
    LOGSTRING("Creator: CCreatorRandomDataParser::ConstructL");
	iElementNameArray = new (ELeave) CDesCArrayFlat(32);
	iElementNameArray->InsertL(EFirstName, KFirstName);
	iElementNameArray->InsertL(ESurname, KSurname);
	iElementNameArray->InsertL(ECompany, KCompany);
	iElementNameArray->InsertL(EAddress, KAddress);
	iElementNameArray->InsertL(EJobTitle, KJobTitle);
	iElementNameArray->InsertL(EPhoneNumber, KPhoneNumber);
	iElementNameArray->InsertL(EGroupName, KGroupName);
	iElementNameArray->InsertL(EMeetingReason, KMeetingReason);
	iElementNameArray->InsertL(EMeetingPlace, KMeetingPlace);
	iElementNameArray->InsertL(EMemoText, KMemoText);
	iElementNameArray->InsertL(EAnniversaryReason, KAnniversaryReason);
	iElementNameArray->InsertL(EToDoText, KToDoText);
	iElementNameArray->InsertL(EReminderText, KReminderText);
	iElementNameArray->InsertL(EMessageSubject, KMessageSubject);
	iElementNameArray->InsertL(EMessageText, KMessageText);
	iElementNameArray->InsertL(ECity, KCity);
	iElementNameArray->InsertL(ECountry, KCountry);
	iElementNameArray->InsertL(EPostcode, KPostcode);
	iElementNameArray->InsertL(EState, KState);
	iElementNameArray->InsertL(EPobox, KPobox);
	iElementNameArray->InsertL(EPrefix, KPrefix);
	iElementNameArray->InsertL(ESuffix, KSuffix);
	iElementNameArray->InsertL(ELandmarkName, KLandmarkName);
	iElementNameArray->InsertL(ELandmarkDescription, KLandmarkDescription);
	iContentData = HBufC::NewL(16);
    iResultArray = new (ELeave) CDesCArrayFlat(16);
    }

CCreatorRandomDataParser* CCreatorRandomDataParser::NewL()
    {
    LOGSTRING("Creator: CCreatorRandomDataParser::NewL");
    CCreatorRandomDataParser* self = CCreatorRandomDataParser::NewLC();    
    CleanupStack::Pop();
    return self;
    }

CCreatorRandomDataParser* CCreatorRandomDataParser::NewLC()
    {
    LOGSTRING("Creator: CCreatorRandomDataParser::NewLC");
    CCreatorRandomDataParser* self = new (ELeave) CCreatorRandomDataParser;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCreatorRandomDataParser::~CCreatorRandomDataParser()
    {
    delete iParser;
    delete iResultArray;
    delete iContentData;
    delete iElementNameArray;
    }

void CCreatorRandomDataParser::GetTextFileMode(RFile& aFile, TInt& aFileSize) 
    { 
    LOGSTRING("Creator: CCreatorRandomDataParser::GetTextFileMode");
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
 


CDesCArrayFlat* CCreatorRandomDataParser::ParseL(const TDesC& aFileName, const TRandomDataType aDataType)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::ParseL: %S", &aFileName);
    
    iDataType = aDataType;
    
    // Create and start XML-parser
    if( iParser == 0)
        {
        _LIT8(KXmlMimeType, "text/xml");
        iParser = CParser::NewL(KXmlMimeType, *this);        
        }

    RFs filesession;
    CleanupClosePushL(filesession);
    User::LeaveIfError( filesession.Connect() );
    
    iResultArray->Reset();
    
    Xml::ParseL(*iParser, filesession, aFileName);
    
    CleanupStack::PopAndDestroy(&filesession); // filesession 
    
    CDesCArrayFlat* tempArray = new (ELeave) CDesCArrayFlat(8);
    CleanupStack::PushL(tempArray);
    for (TInt i = 0; i < iResultArray->MdcaCount(); i++)
    	{
    	tempArray->AppendL(iResultArray->MdcaPoint(i));
    	}
    CleanupStack::Pop(tempArray);
    return tempArray;
    }

void CCreatorRandomDataParser::OnStartDocumentL(const Xml::RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnStartDocumentL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);    
    iInsideRootElement = EFalse;
	iContentParsingActive = EFalse;
    }

void CCreatorRandomDataParser::OnEndDocumentL(TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnEndDocumentL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }

void CCreatorRandomDataParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, 
                                                     const RString& /*aUri*/, 
                                                     TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnStartPrefixMappingL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }

void CCreatorRandomDataParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnEndPrefixMappingL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }

void CCreatorRandomDataParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnIgnorableWhiteSpaceL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }

void CCreatorRandomDataParser::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnSkippedEntityL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }

void CCreatorRandomDataParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, 
                                                        const TDesC8& /*aData*/, 
                                                        TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnProcessingInstructionL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    }

void CCreatorRandomDataParser::OnError(TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnError: %d", aErrorCode);
    
    iLastError = aErrorCode;
    }

TAny* CCreatorRandomDataParser::GetExtendedInterface(const TInt32 aUid)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::GetExtendedInterface (UID: %d)", aUid);
    return this;
    }

void CCreatorRandomDataParser::OnStartElementL(const Xml::RTagInfo& aElement,
                                               const Xml::RAttributeArray& /*aAttributes*/,
                                               TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnStartElementL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);

    const TDesC8& elementName8 = aElement.LocalName().DesC();        
    HBufC* elementName16 = Convert8BitTo16BitLC(elementName8);
    if (iInsideRootElement || (elementName16->Des() == KRootElement))
    	{
    	iInsideRootElement = ETrue;
    	}
    if (iInsideRootElement && (elementName16->Des() == iElementNameArray->MdcaPoint(iDataType)))
    	{
        iContentParsingActive = ETrue;
        iContentData->Des().Zero();
    	}
    CleanupStack::PopAndDestroy(elementName16);
    }

void CCreatorRandomDataParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnEndElementL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);

    const TDesC8& elementName8 = aElement.LocalName().DesC();
    HBufC* elementName16 = Convert8BitTo16BitLC(elementName8);
    if (!iInsideRootElement || (elementName16->Des() == KRootElement))
    	{
    	iInsideRootElement = EFalse;
    	}
    CleanupStack::PopAndDestroy(elementName16);

    if (iContentParsingActive)
		{
	    if (iContentData->Des().Length() > 0)
	    	{
	    	iResultArray->AppendL(*iContentData);
	    	}
        iContentParsingActive = EFalse;
		}
    }

void CCreatorRandomDataParser::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
    LOGSTRING2("Creator: CCreatorRandomDataParser::OnContentL (Error code: %d)", aErrorCode);
    User::LeaveIfError(aErrorCode);
    
    if (iContentParsingActive)
    	{
        HBufC* data16 = Convert8BitTo16BitLC(aBytes);
        TInt newLen = iContentData->Des().Length() + data16->Des().Length();
        iContentData = iContentData->ReAllocL(newLen);
        iContentData->Des().Append(*data16);
        CleanupStack::PopAndDestroy(data16);
    	}
    }

HBufC16* CCreatorRandomDataParser::Convert8BitTo16BitLC(const TDesC8& aInput) 
    {     
    LOGSTRING("Creator: CCreatorRandomDataParser::Convert8BitTo16BitLC");
    HBufC16* output = NULL;
    
    output = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aInput);
    
    CleanupStack::PushL(output);
    return output;          
    } 

TInt CCreatorRandomDataParser::GetError() const 
	{
    return iLastError;
	}
