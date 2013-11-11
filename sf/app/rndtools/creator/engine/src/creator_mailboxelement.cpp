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



#include "creator_mailboxelement.h"
#include "creator_traces.h"
#include "creator_mailbox.h"

using namespace creatormailbox;

/*
 * 
 */
CCreatorMailboxElement* CCreatorMailboxElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorMailboxElement* self = new (ELeave) CCreatorMailboxElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }
/*
 * 
 */
CCreatorMailboxElement::CCreatorMailboxElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;
    }

TBool CCreatorMailboxElement::SetIntegerParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* param, TBool aSetRandom )
    {    
    _LIT( KAll, "all");
    
    if( aElemName == Kincomingport )
        {
        if( aSetRandom )
            param->iIncomingPort = iEngine->RandomNumber(25, 500);
        else        
            param->iIncomingPort = ConvertStrToIntL(aElemContent);
        }
    else if( aElemName == Kattachmentsizelimit )
        {
        if( aSetRandom )
            param->iAttachmentSizeLimit = iEngine->RandomNumber(1, KMaxTInt);
        else
            param->iAttachmentSizeLimit = ConvertStrToIntL(aElemContent);
        }        
    else if( aElemName == Kbodytextsizelimit )
        {
        if( aSetRandom )
            param->iBodyTextSizeLimit = iEngine->RandomNumber(1, KMaxTInt);
        else
            param->iBodyTextSizeLimit = ConvertStrToIntL(aElemContent);
        }     
    else if( aElemName == Kattachmentfetchsize )
        {
        if( aSetRandom )
            param->iAttachmentFetchSize = iEngine->RandomNumber(1, 40960);
        else            
            param->iAttachmentFetchSize = ConvertStrToIntL(aElemContent);
        }
    else if( aElemName == Kimapidletimeout )
        {
        if( aSetRandom )
            param->iImapIdleTimeout = iEngine->RandomNumber(10, 2000);
        else
            param->iImapIdleTimeout = ConvertStrToIntL(aElemContent);
        }
    else if( aElemName == Kmaxemailsize )
        {
        if( aSetRandom )
            param->iMaxEmailSize =  iEngine->RandomNumber(1024, KMaxTInt);
        else
            param->iMaxEmailSize = ConvertStrToIntL(aElemContent);
        }
    else if( aElemName == Ksyncrate )
        {
        if( aSetRandom )
            param->iSyncRate = iEngine->RandomNumber(10, 500);
        else
            param->iSyncRate = ConvertStrToIntL(aElemContent);
        }
    else if( aElemName == Kinboxsynclimit )
        {
        if( aSetRandom )
            param->iInboxSyncLimit = iEngine->RandomNumber(-1, 256);
        else if( CompareIgnoreCase(aElemContent, KAll) == 0 )
            param->iInboxSyncLimit = -1;
        else
            param->iInboxSyncLimit = ConvertStrToIntL(aElemContent);
        }
    else if( aElemName == Kmailboxsynclimit )
        {
        if( aSetRandom )
            param->iMailboxSyncLimit = iEngine->RandomNumber(-1, 256);
        else if( CompareIgnoreCase(aElemContent, KAll) == 0 )
            param->iMailboxSyncLimit = -1;
        else
            param->iMailboxSyncLimit = ConvertStrToIntL(aElemContent);
        }        
    else if( aElemName == Koutgoingport )
        {
        if( aSetRandom )
            param->iOutgoingPort = KDefaultSmtpPort;
        else
            param->iOutgoingPort = ConvertStrToIntL(aElemContent);
        }
    else if( aElemName == Ktoccincludelimit )
        {
        if( aSetRandom )
            param->iToCCIncludeLimit = iEngine->RandomNumber(0, 30);
        else
            param->iToCCIncludeLimit = ConvertStrToIntL(aElemContent);
        }
    else
        return EFalse;
    
    return ETrue;
    }

TBool CCreatorMailboxElement::GetBooleanValueL(const TDesC& aElemContent, TBool aSetRandom)
    {
    if( aSetRandom )
        return iEngine->RandomNumber(0,1) == 1;
    
    return ConvertStrToBooleanL(aElemContent);
    }

TBool CCreatorMailboxElement::SetBooleanParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* param, TBool aSetRandom )
    {  
    TBool val = GetBooleanValueL(aElemContent, aSetRandom);
    if( aElemName == Kincomingsslwrapper )
        param->iIncomingSSLWrapper = val;
    else if( aElemName == Kincomingsecuresockets )
        param->iIncomingSecureSockets = val;
    else if( aElemName == Kacknowledgereceipts )
        param->iAcknowledgeReceipts = val;
    else if( aElemName == Kautosendonconnect )
        param->iAutoSendOnConnect = val;
    else if( aElemName == Kdeletemailsatdisconnect )
        param->iDeleteEmailsAtDisconnect = val;
    else if( aElemName == Kimapidlecommand )
        param->iImapIdleCommand = val;
    else if( aElemName == Kmarkseeninsync )
        param->iMarkSeenInSync = val;
    else if( aElemName == Kenableexpungemode )
        param->iEnableExpungeMode = val;
    else if( aElemName == Kuseapopsecurelogin )
        param->iUseApop = val;
    else if( aElemName == Kdisconnectedusermode )
        param->iDisconnectedUserMode = val;
    else if( aElemName == Koutgoingsslwrapper )
        param->iOutgoingSSLWrapper = val;
    else if( aElemName == Koutgoingsecuresockets )
        param->iOutgoingSecureSockets = val;
    else if( aElemName == Kincludesignature )
        param->iIncludeSignature = val;
    else if( aElemName == Kaddvcard )
        param->iAddVCard = val;
    else if( aElemName == Krequestreceipts )
        param->iRequestReceipts = val;
    else if( aElemName == Ksmtpauth )
        param->iSmtpAuth = val;
    else
        return EFalse;

    return ETrue;
    }

TBool CCreatorMailboxElement::SetTextParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* param, TBool aSetRandom )
    {   
    _LIT(KIncomingSrvPrefix, "mail.");
    _LIT(KOutgoingSrvPrefix, "smtp.");
    _LIT(KSrvPostfix, ".com");
    _LIT(KDefaultConnMethod, "internet");
    
    if( aElemName == Kname )
        {
        if( aSetRandom )
            {
            TPtrC company(iEngine->RandomString(CCreatorEngine::ECompany));
            SetContentToTextParamL(param->iMailboxName, company);
            }
        else
            SetContentToTextParamL(param->iMailboxName, aElemContent);
        }
    else if( aElemName == Kincomingloginname )
        {
        if( aSetRandom )
            {
            TPtrC random(iEngine->RandomString(CCreatorEngine::EFirstName));
            SetContentToTextParamL(param->iIncomingLoginName, random);
            param->iIncomingLoginName->Des().LowerCase();
            }
        else
            SetContentToTextParamL(param->iIncomingLoginName, aElemContent);
        }
    else if( aElemName == Kincomingpassword )
        {
        if( aSetRandom )
            {
            TPtrC random(iEngine->RandomString(CCreatorEngine::EFirstName));
            SetContentToTextParamL(param->iIncomingPassword, random);
            param->iIncomingPassword->Des().LowerCase();
            }
        else
            SetContentToTextParamL(param->iIncomingPassword, aElemContent);
        }
    else if( aElemName == Kincomingservername )
        {
        if( aSetRandom )
            {
            TPtrC random(iEngine->RandomString(CCreatorEngine::ECompany));
            HBufC* srvName = HBufC::NewLC(KIncomingSrvPrefix().Length()+random.Length()+KSrvPostfix().Length());
            srvName->Des().Copy(KIncomingSrvPrefix);
            srvName->Des().Append(random);
            srvName->Des().Append(KSrvPostfix);
            SetContentToTextParamL(param->iIncomingServerName, srvName->Des());
            CleanupStack::PopAndDestroy(); // srvName
            }
        else
            SetContentToTextParamL(param->iIncomingServerName, aElemContent);
        }
    else if( aElemName == Kincomingconnectionmethod )
        {
        if( aSetRandom )                        
            SetContentToTextParamL(param->iIncomingConnectionMethod, KDefaultConnMethod);            
        else
            SetContentToTextParamL(param->iIncomingConnectionMethod, aElemContent);
        }
    else if( aElemName == Kincomingfolderpath )
        SetContentToTextParamL(param->iIncomingFolderPath, aElemContent);
    else if( aElemName == Kpathseparator )
        SetContentToTextParamL(param->iPathSeparator, aElemContent);
    else if( aElemName == Koutgoingloginname )
        {
        if( aSetRandom )
            {
            TPtrC random(iEngine->RandomString(CCreatorEngine::EFirstName));
            SetContentToTextParamL(param->iOutgoingLoginName, random);
            param->iOutgoingLoginName->Des().LowerCase();
            }
        else
            SetContentToTextParamL(param->iOutgoingLoginName, aElemContent);
        }
    else if( aElemName == Koutgoingpassword )
        {
        if( aSetRandom )
            {
            TPtrC random(iEngine->RandomString(CCreatorEngine::EFirstName));
            SetContentToTextParamL(param->iOutgoingPassword, random);
            param->iOutgoingPassword->Des().LowerCase();
            }
        else
            SetContentToTextParamL(param->iOutgoingPassword, aElemContent);
        }
    else if( aElemName == Koutgoingservername )
        {
        if( aSetRandom )
            {
            TPtrC random(iEngine->RandomString(CCreatorEngine::ECompany));
            HBufC* srvName = HBufC::NewLC(KOutgoingSrvPrefix().Length()+random.Length()+KSrvPostfix().Length());
            srvName->Des().Copy(KOutgoingSrvPrefix);
            srvName->Des().Append(random);
            srvName->Des().Append(KSrvPostfix);
            SetContentToTextParamL(param->iOutgoingServerName, srvName->Des());
            CleanupStack::PopAndDestroy(); // srvName
            }
        else
            SetContentToTextParamL(param->iOutgoingServerName, aElemContent);
        }
    else if( aElemName == Koutgoingconnectionmethod )
        {
        if( aSetRandom )            
            SetContentToTextParamL(param->iOutgoingConnectionMethod, KDefaultConnMethod);            
        else
            SetContentToTextParamL(param->iOutgoingConnectionMethod, aElemContent);
        }
    else if( aElemName == Kownemail )
        {
        if( aSetRandom )
            {
            TDesC* email = iEngine->CreateEmailAddressLC();
            SetContentToTextParamL(param->iOwnEmailAddress, *email);
            CleanupStack::PopAndDestroy(); // email
            }
        else
            SetContentToTextParamL(param->iOwnEmailAddress, aElemContent);
        }
    else if( aElemName == Kemailalias )
        {
        if( aSetRandom )
            {
            _LIT(KSpace, " ");
            TPtrC firstName(iEngine->RandomString(CCreatorEngine::EFirstName));
            TPtrC lastName(iEngine->RandomString(CCreatorEngine::ESurname));
            HBufC* alias = HBufC::NewLC(firstName.Length() + lastName.Length() + KSpace().Length());
            alias->Des().Copy(firstName);
            alias->Des().Append(KSpace);
            alias->Des().Append(lastName);
            SetContentToTextParamL(param->iOwnEmailAlias, alias->Des());
            CleanupStack::PopAndDestroy(); // alias
            }
        else        
            SetContentToTextParamL(param->iOwnEmailAlias, aElemContent);
        }
    else if( aElemName == Kreceiptaddress )
        {
        if( aSetRandom )
            {
            TDesC* email = iEngine->CreateEmailAddressLC();
            SetContentToTextParamL(param->iReceiptAddress, *email);
            CleanupStack::PopAndDestroy(); // email
            }
        else
            SetContentToTextParamL(param->iReceiptAddress, aElemContent);
        }
    else if( aElemName == Kreplytoaddress )
        {
        if( aSetRandom )
            {
            TDesC* email = iEngine->CreateEmailAddressLC();
            SetContentToTextParamL(param->iReceiptAddress, *email);
            CleanupStack::PopAndDestroy(); // email
            }
        else
            SetContentToTextParamL(param->iReplyToAddress, aElemContent);
        }
    else
        return EFalse;
    
    return ETrue;
    }

TBool CCreatorMailboxElement::SetEnumParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* param, TBool aSetRandom )
    {
    // Get email options:
    if( aElemName == Kgetemailoptions )
        {
        if( aSetRandom )
            param->iGetEmailOptions = (CMailboxesParameters::TGetEmailOption) iEngine->RandomNumber(
                                                    CMailboxesParameters::EGetHeaders, 
                                                    CMailboxesParameters::EGetEmailOptionLast-1);
        else if( CompareIgnoreCase(aElemContent, Kgetheaders ) == 0 )
            param->iGetEmailOptions = CMailboxesParameters::EGetHeaders;
        else if( CompareIgnoreCase(aElemContent, Kgetbodytext) == 0 )
            param->iGetEmailOptions = CMailboxesParameters::EGetBodyText;
        else if( CompareIgnoreCase(aElemContent, Kgetbodytextandattachments) == 0 )
            param->iGetEmailOptions = CMailboxesParameters::EGetBodyTextAndAttachments;
        else if( CompareIgnoreCase(aElemContent, Kgetattachments) == 0 )
            param->iGetEmailOptions = CMailboxesParameters::EGetAttachments;
        else if( CompareIgnoreCase(aElemContent, Kgetbodyalternativetext) == 0 )
            param->iGetEmailOptions = CMailboxesParameters::EGetBodyAlternativeText;
        else 
            param->iGetEmailOptions = CMailboxesParameters::EUndef;
        }
    
    // Subscribe type:
    else if( aElemName == Ksubscribetype )
        {
        if( aSetRandom )
            param->iSubscribeType = (TFolderSubscribeType) iEngine->RandomNumber( EUpdateNeither, EUpdateBoth );
        else if( CompareIgnoreCase( aElemContent, Kupdatelocal) == 0 )
            param->iSubscribeType = EUpdateLocal;            
        else if( CompareIgnoreCase( aElemContent, Kupdateremote) == 0 )
            param->iSubscribeType = EUpdateRemote;
        else if( CompareIgnoreCase( aElemContent, Kupdateboth) == 0 )
            param->iSubscribeType = EUpdateBoth;
        else
            param->iSubscribeType = EUpdateNeither;
        }
    
    // Folder synchronization type:
    else if( aElemName == Kfoldersynctype)
        {
        if( aSetRandom )
            param->iSyncType = (TFolderSyncType) iEngine->RandomNumber(EUseCombination, EUseRemote);
        else if( CompareIgnoreCase( aElemContent, Kuselocal) == 0 )
            param->iSyncType = EUseLocal;
        else if( CompareIgnoreCase( aElemContent, Kuseremote) == 0 )
            param->iSyncType = EUseRemote;
        else
            param->iSyncType = EUseCombination;
        }
     
    // Send copy to self option:
    else if( aElemName == Kcopytoself )
        {
        if( aSetRandom )
            param->iSendCopyToSelf = (TImSMTPSendCopyToSelf) iEngine->RandomNumber(ESendNoCopy, ESendCopyAsBccRecipient);
        else if( CompareIgnoreCase( aElemContent, Kto) == 0 )
            param->iSendCopyToSelf = ESendCopyAsToRecipient;
        else if( CompareIgnoreCase( aElemContent, Kcc) == 0 )
            param->iSendCopyToSelf = ESendCopyAsCcRecipient;
        else if( CompareIgnoreCase( aElemContent, Kbcc) == 0 )
            param->iSendCopyToSelf = ESendCopyAsBccRecipient;
        else
            param->iSendCopyToSelf = ESendNoCopy;
        }
    
    // Send option:
    else if( aElemName == Ksendoption )
        {
        if( aSetRandom )
            param->iSendOption = (TImSMTPSendMessageOption) iEngine->RandomNumber(ESendMessageImmediately, ESendMessageOnRequest);
        else if( CompareIgnoreCase( aElemContent, Konnextconnection) == 0 )
            param->iSendOption = ESendMessageOnNextConnection;
        else if( CompareIgnoreCase( aElemContent, Konrequest) == 0 )
            param->iSendOption = ESendMessageOnRequest;
        else
            param->iSendOption = ESendMessageImmediately;
        }
    else
        {
        return EFalse;
        }
    return ETrue;
    }

void CCreatorMailboxElement::ExecuteCommandL()
    {
    const CCreatorScriptAttribute* amountAttr = FindAttributeByName(KAmount);
    const CCreatorScriptAttribute* typeAttr = FindAttributeByName(KMailboxType);
    TInt amount = 1;    
    if( amountAttr )
        {
        amount = ConvertStrToIntL(amountAttr->Value());
        }
    
    TMailboxType mailboxType = EMailboxPOP3;
    
    if( typeAttr && CompareIgnoreCase(typeAttr->Value(), Kimap4) == 0 )
        {
        mailboxType = EMailboxIMAP4;
        }
    
    // Get 'fields' element 
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    if( fieldsElement && fieldsElement->SubElements().Count() > 0)
        {
        // Get sub-elements
        const RPointerArray<CCreatorScriptElement>& fields = fieldsElement->SubElements();        
        // Create mailbox entries, the amount of entries is defined by amount:
        for( TInt cI = 0; cI < amount; ++cI )
            {            
            CMailboxesParameters* param = new (ELeave) CMailboxesParameters;
            CleanupStack::PushL(param);
            
            param->iMailboxType = mailboxType;
            
            for( TInt i = 0; i < fields.Count(); ++i )
                {
                CCreatorScriptElement* field = fields[i];
                TPtrC elemName = field->Name();
                TPtrC elemContent = field->Content();
                const CCreatorScriptAttribute* randomAttr = fields[i]->FindAttributeByName(KRandomLength);
                TBool useRandom = EFalse;
                if( randomAttr || elemContent.Length() == 0 )
                    {
                    useRandom = ETrue;
                    }
                
                if( SetTextParamL(elemName, elemContent, param, useRandom ) == EFalse &&
                    SetBooleanParamL(elemName, elemContent, param, useRandom ) == EFalse &&
                    SetIntegerParamL(elemName, elemContent, param, useRandom ) == EFalse &&
                    SetEnumParamL(elemName, elemContent, param, useRandom ) )
                    {
                    LOGSTRING2("CCreatorMailboxElement::ExecuteCommandL: Unknown mailbox field: %S", &elemName);
                    }
                }
            iEngine->AppendToCommandArrayL(ECmdCreateMessagingEntryMailboxes, param);
            CleanupStack::Pop(); // param
            }
        }
    else
    	{
    	iEngine->AppendToCommandArrayL(ECmdCreateMessagingEntryMailboxes, 0, amount);
    	}
    }
