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



#ifndef CREATORMESSAGEELEMENT_H_
#define CREATORMESSAGEELEMENT_H_

#include "creator_scriptelement.h"
#include "creator_message.h"

namespace creatormsg
{
    _LIT(KMessage, "message");
    _LIT(KType, "type");
    _LIT(KTo, "to");
    _LIT(KFrom, "from");
    _LIT(KFolder, "folder");
    _LIT(KSubject, "subject");
    _LIT(KText, "text");
    _LIT(KAttachmentPath, "attachmentpath");
    _LIT(KAttachmentId, "attachmentid");
    _LIT(KStatus, "status");
    _LIT(KSms, "sms");
    _LIT(KMms, "mms");
    _LIT(KAms, "ams");
    _LIT(KEmail, "email");
    _LIT(KSmart, "smart");
    _LIT(KBt, "bt");
    _LIT(KIr, "ir");
    _LIT(KSent, "sent");
    _LIT(KInbox, "inbox");
    _LIT(KDraft, "draft");
    _LIT(KOutbox, "outbox");
    _LIT(KMailbox, "mailbox");
    _LIT(KRead, "read");
    _LIT(KNew, "new");
}

/*
 * Builds message parameters from the parsed script and adds a command to the engine 
 */
class CCreatorMessageElement : public CCreatorScriptElement
{
public:
    static CCreatorMessageElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
protected:    
    CCreatorMessageElement(CCreatorEngine* aEngine);
    /*
     * Creates message address. The address type is based on the message type.
     * @return Phone number string for SMS, MMS, AMS and Smart messages. Email address for other types.
     * @param msgType Message type.
     */
    HBufC* CreateMessageAddressLC(const TDesC& msgType);
    /*
     * Sets message type to parameters
     * @param aParameters Message parameters
     * @param aMsgTypeStr Message type string
     */
    void SetMessageTypeL(CMessagesParameters& aParameters, const TDesC& aMsgTypeStr ) const;
    /*
     * Gets length of the random data
     * @return Length of the random data
     * @param aRandomLenStr String defining the random length (from the script)
     * @param aMsgType Message type string
     */
    TInt GetRandomBodyLengthL(const TDesC& aRandomLenStr, const TDesC& aMsgType ) const;
    /*
     * Gets maximum length of the body text
     * @return Maximum length of the body text
     * @param aMsgType Message type string
     */
    TInt GetMaxBodyLength( const TDesC& aMsgType ) const;

};
    

#endif /*CREATORMESSAGEELEMENT_H_*/
