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




#ifndef CREATORMAILBOXELEMENT_H_
#define CREATORMAILBOXELEMENT_H_

#include "creator_scriptelement.h"

class CMailboxesParameters;

namespace creatormailbox
{
    _LIT(Kmailbox, "mailbox");
    _LIT(Kname, "name");
    _LIT(Kincomingport, "incomingport");
    _LIT(Kincomingsslwrapper, "incomingsslwrapper");
    _LIT(Kincomingsecuresockets, "incomingsecuresockets");
    _LIT(Kincomingloginname, "incomingloginname");
    _LIT(Kincomingpassword, "incomingpassword");
    _LIT(Kincomingservername, "incomingservername");
    _LIT(Kincomingconnectionmethod, "incomingconnectionmethod");
    _LIT(Kacknowledgereceipts, "acknowledgereceipts");
    _LIT(Kattachmentsizelimit, "attachmentsizelimit");
    _LIT(Kautosendonconnect, "autosendonconnect");
    _LIT(Kbodytextsizelimit, "bodytextsizelimit");
    _LIT(Kdeletemailsatdisconnect, "deletemailsatdisconnect");
    _LIT(Kattachmentfetchsize, "attachmentfetchsize");
    _LIT(Kincomingfolderpath, "incomingfolderpath");
    _LIT(Kpathseparator, "pathseparator");
    _LIT(Kgetemailoptions, "getemailoptions");
    _LIT(Kimapidlecommand, "imapidlecommand");
    _LIT(Kimapidletimeout, "imapidletimeout");
    _LIT(Kmaxemailsize, "maxemailsize");
    _LIT(Ksubscribetype, "subscribetype");
    _LIT(Ksyncrate, "syncrate");
    _LIT(Kfoldersynctype, "foldersynctype");
    _LIT(Kmarkseeninsync, "markseeninsync");
    _LIT(Kenableexpungemode, "enableexpungemode");
    _LIT(Kuseapopsecurelogin, "useapopsecurelogin");
    _LIT(Kinboxsynclimit, "inboxsynclimit");
    _LIT(Kmailboxsynclimit, "mailboxsynclimit");
    _LIT(Kdisconnectedusermode, "disconnectedusermode");
    _LIT(Koutgoingport, "outgoingport");
    _LIT(Koutgoingsslwrapper, "outgoingsslwrapper");
    _LIT(Koutgoingsecuresockets, "outgoingsecuresockets");
    _LIT(Koutgoingloginname, "outgoingloginname");
    _LIT(Koutgoingpassword, "outgoingpassword");
    _LIT(Koutgoingservername, "outgoingservername");
    _LIT(Koutgoingconnectionmethod, "outgoingconnectionmethod");
    _LIT(Kincludesignature, "includesignature");
    _LIT(Kaddvcard, "addvcard");
    _LIT(Kownemail, "ownemail");
    _LIT(Kemailalias, "emailalias");
    _LIT(Kreceiptaddress, "receiptaddress");
    _LIT(Kreplytoaddress, "replytoaddress");
    _LIT(Krequestreceipts, "requestreceipts");
    _LIT(Ksmtpauth, "smtpauth");
    _LIT(Kcopytoself, "copytoself");
    _LIT(Ksendoption, "sendoption");
    _LIT(Ktoccincludelimit, "toccincludelimit");
    
    
    // Get email options:
    _LIT(Kgetheaders, "getheaders");
    _LIT(Kgetbodytext, "getbodytext");
    _LIT(Kgetbodytextandattachments, "getbodytextandattachments");
    _LIT(Kgetattachments, "getattachments");
    _LIT(Kgetbodyalternativetext, "getbodyalternativetext");
    
    // Subscribe types:
    _LIT(Kupdateneither, "updateneither");
    _LIT(Kupdatelocal, "updatelocal");
    _LIT(Kupdateremote, "updateremote");
    _LIT(Kupdateboth, "updateboth");
    
    // Folder sync. types:
    _LIT(Kusecombination, "usecombination");
    _LIT(Kuselocal, "uselocal");
    _LIT(Kuseremote, "useremote");
    
    // Send copy to self option:
    _LIT(Kno, "no");
    _LIT(Kto, "to");
    _LIT(Kcc, "cc");
    _LIT(Kbcc, "bcc");
    
    // Send option:
    _LIT(Kimmediately, "immediately");
    _LIT(Konnextconnection, "onnextconnection");
    _LIT(Konrequest, "onrequest");
    
    // Mailbox types:
    _LIT(KMailboxType, "type");
    _LIT(Kpop3, "pop3");
    _LIT(Kimap4, "imap4");
    _LIT(Ksyncml, "syncml");
}

class CCreatorMailboxElement : public CCreatorScriptElement
{
public:
    static CCreatorMailboxElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
    
protected:
    CCreatorMailboxElement(CCreatorEngine* aEngine);
    TBool SetTextParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* aParams, TBool aSetRandom );
    TBool SetBooleanParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* aParams, TBool aSetRandom );
    TBool SetIntegerParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* aParams, TBool aSetRandom );
    TBool SetEnumParamL(const TDesC& aElemName, const TDesC& aElemContent, CMailboxesParameters* aParams, TBool aSetRandom );
    
    TBool GetBooleanValueL(const TDesC& aElemContent, TBool aSetRandom);
};

#endif /*CREATORMAILBOXELEMENT_H_*/
