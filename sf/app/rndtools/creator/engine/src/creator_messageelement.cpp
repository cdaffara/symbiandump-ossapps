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



#include "creator_messageelement.h"
#include "creator_traces.h"
#include "creator_message.h"

using namespace creatormsg;

/*
 * 
 */
CCreatorMessageElement* CCreatorMessageElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorMessageElement* self = new (ELeave) CCreatorMessageElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }
/*
 * 
 */
CCreatorMessageElement::CCreatorMessageElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;
    }
/*
 * Sets message type
 */
void CCreatorMessageElement::SetMessageTypeL(CMessagesParameters& aParameters, const TDesC& aMsgTypeStr ) const
    {
    if( CompareIgnoreCase(aMsgTypeStr, KSms) == 0 )
        {
        aParameters.iMessageType = ESMS;
        }
    else if( CompareIgnoreCase(aMsgTypeStr, KMms) == 0 )
        {
        aParameters.iMessageType = EMMS;
        }
    else if( CompareIgnoreCase(aMsgTypeStr, KAms) == 0 )
        {
        aParameters.iMessageType = EAMS;
        }
    else if( CompareIgnoreCase(aMsgTypeStr, KEmail) == 0 )
        {
        aParameters.iMessageType = EEmail;
        }
    else if( CompareIgnoreCase(aMsgTypeStr, KSmart) == 0 )
        {
        aParameters.iMessageType = ESmartMessage;
        }
    else if( CompareIgnoreCase(aMsgTypeStr, KIr) == 0 )
        {
        aParameters.iMessageType = EIrMessage;
        }
    else if( CompareIgnoreCase(aMsgTypeStr, KBt) == 0 )
        {
        aParameters.iMessageType = EBTMessage;
        }
    else
        {
        LOGSTRING2("ERROR in CCreatorMessageElement::SetMessageTypeL: Unknown message type: %S", &aMsgTypeStr);
        User::Leave(KErrGeneral);
        }
    }
/*
 * Returns maximum length of the body text, when randomlength=max is used.
 */
TInt CCreatorMessageElement::GetMaxBodyLength( const TDesC& aMsgType ) const
    {
    TInt len = 0;
    if( CompareIgnoreCase(aMsgType, KSms) == 0 )
        {
        len = 160;
        }
    else if( CompareIgnoreCase(aMsgType, KMms) == 0 )
        {
        len = 160;
        }    
    else if( CompareIgnoreCase(aMsgType, KEmail) == 0 )
        {
        len = 1024;
        }
    else
        {
        len = KUndef;
        }
    return len;
    }
/*
 * Returns random body text length
 */
TInt CCreatorMessageElement::GetRandomBodyLengthL(const TDesC& aRandomLenStr, const TDesC& aMsgType ) const
    {
    TInt len = 0;
    if( aRandomLenStr == KMax )
        {
        len = GetMaxBodyLength(aMsgType);
        }
    else if( aRandomLenStr == KDefault )
        {
        len = KUndef;        
        }
    else
        {
        len = ConvertStrToIntL(aRandomLenStr);
        }
    return len;
    }

/*
 * Creates random message address (To/From)
 * Returns phone number for SMS, MMS, AMS and Smart messages.
 * Email address for others
 */
HBufC* CCreatorMessageElement::CreateMessageAddressLC(const TDesC& msgType)
    {
    HBufC* toAddr;
    if( CompareIgnoreCase(msgType, KSms) == 0 || 
        CompareIgnoreCase(msgType, KMms) == 0 || 
        CompareIgnoreCase(msgType, KAms) == 0 || 
        CompareIgnoreCase(msgType, KSmart) == 0)
        {
        TPtrC temp = iEngine->RandomString(CCreatorEngine::EPhoneNumber);
        toAddr = HBufC::NewL(temp.Length());
        CleanupStack::PushL(toAddr);
        toAddr->Des().Copy(temp);
        }
    else
        {
        toAddr = iEngine->CreateEmailAddressLC();
        }
    return toAddr;
    }

/*
 *
 */
void CCreatorMessageElement::ExecuteCommandL()
    {
    LOGSTRING("Creator: CCreatorMessageElement::ExecuteCommandL");
    
    // Find out the message type:
    const CCreatorScriptAttribute* msgTypeAttr = this->FindAttributeByName(KType);
    TPtrC msgType;
    if( msgTypeAttr )
        {
        msgType.Set(msgTypeAttr->Value());
        }
    else
        {
        LOGSTRING("ERROR in CCreatorMessageElement::ExecuteCommandL: Type attribute is missing.");
        User::Leave(KErrGeneral); // type is required attribute
        }
    
    // Find out the amount of calendar entries:
    const CCreatorScriptAttribute* msgAmountAttr = this->FindAttributeByName(KAmount);
    TInt msgAmount = 1;    
    if( msgAmountAttr )
        {
        msgAmount = ConvertStrToIntL(msgAmountAttr->Value());
        }    
    
    // Get 'fields' element 
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
        {
        // Get sub-elements (i.e the message field elements)
        const RPointerArray<CCreatorScriptElement>& fields = fieldsElement->SubElements();
        
        // Create message entries, the amount of entries is defined by msgAmount:
        for( TInt cI = 0; cI < msgAmount; ++cI )
            {
            CMessagesParameters* param = new (ELeave) CMessagesParameters;
            CleanupStack::PushL(param);
            
            // Message type:
            if( msgTypeAttr )
                {
                SetMessageTypeL(*param, msgTypeAttr->Value());
                }           
            
            // Loop all the message field elements:
            for( TInt i = 0; i < fields.Count(); ++i )
                {
                CCreatorScriptElement* field = fields[i];
                TPtrC elemName = field->Name();
                TPtrC elemContent = field->Content();
                const RPointerArray<CCreatorScriptElement>& contactReferences = field->SubElements();
                const CCreatorScriptAttribute* randomAttr = field->FindAttributeByName(KRandomLength);
                const CCreatorScriptAttribute* amountAttr = field->FindAttributeByName(KAmount);
                const CCreatorScriptAttribute* increaseAttr = field->FindAttributeByName(KIncrease);
                TBool increase( EFalse );
                if ( increaseAttr )
                    {
                    increase = ConvertStrToBooleanL( increaseAttr->Value() );
                    }
                TInt fieldAmount = 1;
                if( amountAttr )
                    {
                    fieldAmount = ConvertStrToIntL(amountAttr->Value());
                    }               
                
                if( elemName == KTo )
                    {
                    // Recipient ('to'-field)
                    for( TInt amountIndex = 0; amountIndex < fieldAmount; ++amountIndex )
                        {
                        if( (randomAttr || elemContent.Length() == 0 ) && contactReferences.Count() == 0 )
                            {                        
                            // Random content
                            HBufC* toAddr = CreateMessageAddressLC(msgType);
                            if( toAddr )
                                {
                                param->iRecipientAddressArray.AppendL(toAddr);
                                CleanupStack::Pop(); // toAddr
                                }
                            }
                        else
                            {                            
                            if( elemContent.Length() > 0 && contactReferences.Count() == 0)
                                {
                                // Explicit recipient given
                                HBufC* recipient = HBufC::NewL( elemContent.Length() + 3 );
                                CleanupStack::PushL(recipient);
                                if ( increase )
                                    {
                                    IncreasePhoneNumL( elemContent, cI, recipient );
                                    }
                                else
                                    {
                                    recipient->Des().Copy(elemContent);
                                    }
                                param->iRecipientAddressArray.AppendL(recipient);
                                CleanupStack::Pop(); // recipient
                                }
                            else
                                {
                                // Recipients specified with contact-set-references (if any)
                                for( TInt csI = 0; csI < contactReferences.Count(); ++csI )
                                    {                            
                                    CCreatorScriptElement* contactSetRef = contactReferences[csI];
                                    AppendContactSetReferenceL(*contactSetRef, param->iRecipientLinkIds);                                
                                    }
                                }
                            }
                        }
                    }
                else if( elemName == KFrom )
                    {
                    // Sender ('from'-field)
                    // Amount attribute for sender is ignored, because there can be only one sender                 
                    delete param->iSenderAddress;
                    param->iSenderAddress = 0;
                    if( (randomAttr || elemContent.Length() == 0 ) && contactReferences.Count() == 0 )
                        {
                        // Get random address
                        param->iSenderAddress = CreateMessageAddressLC(msgType);
                        CleanupStack::Pop(); // param->iSenderAddress
                        }
                    else
                        {                        
                        if( elemContent.Length() > 0 && contactReferences.Count() == 0)
                            {
                            // Explicit sender address given
                            param->iSenderAddress = HBufC::NewL(elemContent.Length());
                            if ( increase )
                                {
                                IncreasePhoneNumL( elemContent, cI, param->iSenderAddress );
                                }
                            else
                                {
                                param->iSenderAddress->Des().Copy(elemContent);
                                }
                            }
                        else
                            {
                            // Senders specified with contact-set-references (if any)
                            for( TInt csI = 0; csI < contactReferences.Count(); ++csI )
                                {                            
                                CCreatorScriptElement* contactSetRef = contactReferences[csI];
                                AppendContactSetReferenceL(*contactSetRef, param->iSenderLinkIds);                                
                                }
                            }                           
                        }
                    }
                else if( elemName == KFolder )              
                    {
                    // Folder type
                    if( CompareIgnoreCase(elemContent, KSent) == 0 )
                        {
                        param->iFolderType = ESent;
                        }
                    else if( CompareIgnoreCase(elemContent, KInbox) == 0 )
                        {
                        param->iFolderType = EInbox;
                        }
                    else if( CompareIgnoreCase(elemContent, KOutbox) == 0 )
                        {
                        param->iFolderType = EOutbox;
                        }
                    else if( CompareIgnoreCase(elemContent, KDraft) == 0 )
                        {
                        param->iFolderType = EDrafts;
                        }                        
                    }
                else if( elemName == KSubject )
                    {
                    // Message subject
                    delete param->iMessageSubject;
                    param->iMessageSubject = 0;
                    
                    if( randomAttr || elemContent.Length() == 0 )
                        {
                        // Random data should be used
                        TPtrC temp = iEngine->RandomString(CCreatorEngine::EMessageSubject);
                        param->iMessageSubject = HBufC::NewL(temp.Length());
                        param->iMessageSubject->Des().Copy(temp);
                        }
                    else
                        {                        
                        param->iMessageSubject = HBufC::NewL(elemContent.Length());
                        param->iMessageSubject->Des().Copy(elemContent);
                        }
                    }
                else if( elemName == KText )
                    {
                    // Body text
                    delete param->iMessageBodyText;
                    param->iMessageBodyText = 0;
                    
                    if( randomAttr || elemContent.Length() == 0 )
                        {
                        // Put random text:
                        if( randomAttr && randomAttr->Value() != KDefault )
                            {
                            // Get the random length
                            TInt len = GetRandomBodyLengthL(randomAttr->Value(), msgTypeAttr->Value());
                            if( len != KUndef )
                                {
                                param->iMessageBodyText = iEngine->CreateRandomStringLC(len);
                                CleanupStack::Pop(); // param->iMessageBodyText
                                }
                            }
                        else
                            {
                            // Use default random data
                            TPtrC temp = iEngine->RandomString(CCreatorEngine::EMessageText);
                            param->iMessageBodyText = HBufC::NewL(temp.Length());
                            param->iMessageBodyText->Des().Copy(temp);
                            }
                        }
                    else
                        {
                        param->iMessageBodyText = HBufC::NewL(elemContent.Length());
                        param->iMessageBodyText->Des().Copy(elemContent);
                        }
                    }
                else if( elemName == KAttachmentId )
                    {
                    // Attachment file id
                    for( TInt amountIndex = 0; amountIndex < fieldAmount; ++amountIndex )
                        {                        
                        if( randomAttr  || elemContent.Length() == 0)
                            {
                            //When type is AMS, attachement will be audio
                            if(param->iMessageType == EAMS)
                                {
                                param->iAttachments->AppendL(CCreatorEngine::EMP3_250kB);
                                }
                            //Otherwise attachement can be any file
                            else
                                {
                                //EJPEG_25kB is first (0) in the enum and LAST_FILE_ID is last in the enum, so real last item id is one before LAST_FILE_ID
                                param->iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) );                                
                                }
                            }
                        else
                            {
                            //When user has been set attechment by it self, we trust user selection (not validating value, e.g. if message is SMS and there is attachement)
                            TInt id = iEngine->GetAttachmentIdL(elemContent);
                            if( id != KUndef )
                                {
                                param->iAttachments->AppendL( id );
                                }
                            }
                        }
                    }
                // Attachment file path handling 
                //E.g. C:\data\others\DOC-20kB.doc
                else if( elemName == KAttachmentPath )
                    {
                    // Attachment file id
                    for( TInt amountIndex = 0; amountIndex < fieldAmount; ++amountIndex )
                        {
                        //Path is random, getting one of the files (not even using path attribute, but id with random)
                        if( randomAttr  || elemContent.Length() == 0)
                            {
                            //EJPEG_25kB is first (0) in the enum and LAST_FILE_ID is last in the enum, so real last item id is one before LAST_FILE_ID
                            param->iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID -1) );
                            }
                        //Otherwise adding attachement path as it is to paths.
                        else
                            {
                            //Adding Attachement file path
                            HBufC* elemData = elemContent.AllocLC();
                            param->iAttachmentPaths.AppendL( elemData );     
                            CleanupStack::Pop(elemData);
                            }
                        }
                    }
                else if ( elemName == KStatus )
                    {
                    if( CompareIgnoreCase( elemContent, KNew ) == 0 )
                        {
                        param->iCreateAsUnread = ETrue;
                        }
                    else if( CompareIgnoreCase( elemContent, KRead ) == 0 )
                        {
                        param->iCreateAsUnread = EFalse;
                        }
                    }
                }
            iEngine->AppendToCommandArrayL(ECmdCreateMessagingEntryMessagesViaScript, param);
            CleanupStack::Pop(); // param
            }
        }
    else
    	{
    	for( TInt i = 0; i < msgAmount; ++i )
    		{
    		TInt randMsg = 0;
    		if( msgType == KSms )
    			randMsg = iEngine->RandomNumber(ECmdCreateRandomEntrySMSInbox, ECmdCreateRandomEntrySMSSent);
    		else if( msgType == KMms )
    			randMsg = iEngine->RandomNumber(ECmdCreateRandomEntryMMSInbox, ECmdCreateRandomEntryMMSSent);
    		else if( msgType == KAms )
    			randMsg = iEngine->RandomNumber(ECmdCreateRandomEntryAMSInbox, ECmdCreateRandomEntryAMSSent);
    		else if( msgType == KEmail )
    			randMsg = iEngine->RandomNumber(ECmdCreateRandomEntryEmailInbox, ECmdCreateRandomEntryEmailSent);
    		else if( msgType == KSmart )
    			randMsg = iEngine->RandomNumber(ECmdCreateRandomEntryBIOInbox, ECmdCreateRandomEntryBIOSent);
    		else if( msgType == KBt )
    			randMsg = iEngine->RandomNumber(ECmdCreateRandomEntryBTInbox, ECmdCreateRandomEntryBTSent);
    		else if( msgType == KIr )
    			randMsg = iEngine->RandomNumber(ECmdCreateRandomEntryIRInbox, ECmdCreateRandomEntryIRSent);
    			
    		if( randMsg > 0 )
    			{
    			iEngine->AppendToCommandArrayL(randMsg, 0, 1);
    			}
    		}
    	}
    }

// End of file
