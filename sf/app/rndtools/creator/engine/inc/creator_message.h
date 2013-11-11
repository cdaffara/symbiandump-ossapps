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





#ifndef __CREATORMESSAGES_H__
#define __CREATORMESSAGES_H__

#include "engine.h"
#include "creator_modulebase.h"

#include <e32base.h>
#include <e32std.h>
#include <msvapi.h>
#include <mtclbase.h>
#include <mtclreg.h>
#include <smut.h>
#include <smuthdr.h>
#include <smsclnt.h>
#include <miutset.h>
#include <mmsclient.h>
#include <ircmtm.h>
#include <irmsgtypeuid.h>
#include <btmsgtypeuid.h>
#include <bif.h>
#include <biouids.h>
#include <miuthdr.h>

#include <cmsvmimeheaders.h>
#include <smtcmtm.h>
#include <ircmtm.h>
#include <btcmtm.h>
#include <mmsvattachmentmanager.h>


class CCreatorEngine;
class CMessagesParameters;
class CAsyncWaiter; 

enum TMessageType
    {
    ESMS = 0,
    EMMS,
    EAMS,
    EEmail,
    ESmartMessage,
    EIrMessage,
    EBTMessage
    };

enum TFolderType
    {
    EInbox = 0,
    EDrafts,
    EOutbox,
    ESent,
    EMailbox
    };
    
class CRecipientInfo : public CBase
{
public:
    CRecipientInfo();
    virtual ~CRecipientInfo();
    
    void SetPhoneNumber(HBufC* aPhone);
    void SetEmailAddress(HBufC* aEmail);
    const HBufC* PhoneNumber() const;
    const HBufC* EmailAddress() const;
    
private:
    HBufC* iPhoneNumber;
    HBufC* iEmailAddress;
};

class CCreatorMessages : public CCreatorModuleBase, public MMsvSessionObserver
    {
enum TCreatorMessagesStatus{
    ECreatorMessagesDelete = 0,
    ECreatorMessagesStart, 
    ECreatorMessagesMessageType,
    ECreatorMessagesFolderType,
    ECreatorMessagesMessageStatus, 
    ECreatorMessagesCharsInBody,
    ECreatorMessagesAttachment,
};

public: 
    static CCreatorMessages* NewL(CCreatorEngine* aEngine);
    static CCreatorMessages* NewLC(CCreatorEngine* aEngine);
    ~CCreatorMessages();

private:
    CCreatorMessages();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase

    TInt CreateSMSEntryL(const CMessagesParameters& parameters);
    TInt CreateMMSEntryL(const CMessagesParameters& parameters);
    TInt CreateAMSEntryL(const CMessagesParameters& parameters);
    TInt CreateEmailEntryL(const CMessagesParameters& parameters);
    TInt CreateSmartMessageEntryL(const CMessagesParameters& parameters);
    TInt CreateObexEntryL(TUid aMtm, const CMessagesParameters& parameters);
    void HandleAttachementsL(const CMessagesParameters& parameters, CMsvStore* store, TInt err );
    void HandleAttachementL(CMsvStore* store, TInt err, TFileName& sourceFileName );

    CMsvEntrySelection* DoGetListOfAccountsL(CMsvSession& aSession, TUid aMtm, TBool aAlwaysListHidden);
    TMsvId DefaultServiceForMTML(CMsvSession& aSession, TUid aMtm, TBool aFindFirstServiceIfNoDefault);

    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3); // from MMsvSessionObserver
    
    void AddRecipientsL(CBaseMtm& aMtm, const CMessagesParameters& aParameters, TBool aUseEmailAddress );
    void GetLinkedAddressesL(   RPointerArray<HBufC>& aAddressArray, 
                                const RArray<TLinkIdParam>& aLinkIds, 
                                TBool aUseEmailAddress,
                                TInt aNumOfExistingAddresses );
    
    void GetAllRecipientsL(RPointerArray<HBufC>& aRecipientArray, const CMessagesParameters& aParameters, TBool aUseEmailAddress );
    void SetSenderToEntryDetailsL(TMsvEntry& aMsgEntry, const CMessagesParameters& aParameters, TBool aUseEmailAddress);
    void SetRecipientToEntryDetailsL(TMsvEntry& aMsgEntry, const CMessagesParameters& aParameters, TBool aUseEmailAddress);
    void AddSenderToMtmAddresseeL(CBaseMtm& aMtm, const CMessagesParameters& aParameters, TBool aUseEmailAddress );
    void AddMtmSenderL(CMmsClientMtm& aMtm, const CMessagesParameters& aParameters, TBool aUseEmailAddress );
    void GetSendersL(RPointerArray<HBufC>& aSenderArray, const CMessagesParameters& aParameters, TBool aUseEmailAddress, TInt aMaxNum );
    void DeleteAllMessagesL( TBool aOnlyCreatedWithCreator );
    void DeleteAllFromFolderL( const TMsvId aContext, CMsvSession* aSession, CSmsClientMtm* aClientMtm, TBool aOnlyCreatedWithCreator = EFalse );
    
public:
    TBool AskDataFromUserL( TInt aCommand ); // from MCreatorModuleBase
    void QueryDialogClosedL(TBool aPositiveAction, TInt aUserData); //from MUIObserver
    
    TInt CreateMessageEntryL(CMessagesParameters *aParameters, TBool aTakeUserGivenParameters=EFalse);    
    TInt CreateRandomMessageEntryL(TInt aCommand);
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();

private:
    CMessagesParameters*    iParameters;
    TMessageType            iMessageType;
    TFolderType             iFolderType;
    TInt                    iDefinedMessageLength;
    CArrayFixFlat<TInt>*    iAttachments; 
    //RPointerArray<HBufC>    iAttachmentPaths;
    TBool                   iCreateAsUnread;
    TMsvId                  iUserSelectedMailbox;
    HBufC*                  iTmpPhoneNumber;
    HBufC*                  iTmpEmail;
    RArray<TInt>            iEntryIds; // TMsvId ( == TInt32 ) TMsvEntry::Id()
    CAsyncWaiter* iWaiter;
    RPointerArray<HBufC>    iSenderArray;
    RPointerArray<HBufC>    iRecipientArray;
public:
    };


class CMessagesParameters : public CCreatorModuleBaseParameters
    {
public: 
    TMessageType            iMessageType;
    TFolderType             iFolderType;
    HBufC*                  iSenderAddress;    
    HBufC*                  iRecipientAddress;
    RPointerArray<HBufC>    iRecipientAddressArray;
    HBufC*                  iMessageSubject;
    HBufC*                  iMessageBodyText;
    CArrayFixFlat<TInt>*    iAttachments; 
    RPointerArray<HBufC>    iAttachmentPaths;
    TInt                    iDefinedMessageLength;
    TUid                    iBIOMessageType;
    TBool                   iCreateAsUnread;
    RArray<TLinkIdParam>    iRecipientLinkIds;
    RArray<TLinkIdParam>    iSenderLinkIds;
    TInt                    iNumberOfExistingRecipients;
    TInt                    iNumberOfExistingSenders;

public:
    CMessagesParameters();
    ~CMessagesParameters();
    };



#endif // __CREATORMESSAGES_H__
