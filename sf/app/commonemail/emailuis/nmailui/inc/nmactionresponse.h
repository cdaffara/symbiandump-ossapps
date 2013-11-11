/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMACTIONRESPONSE_H_
#define NMACTIONRESPONSE_H_

#include "nmactionobserver.h"
#include "nmactionrequest.h"

/*!
    \enum NmActionResponseCommand
    \brief This value indicates response command of the action
    \ Add more commands when use cases need those
*/
enum NmActionResponseCommand
{
    NmActionResponseCommandNone = 0,
    NmActionResponseCommandOpen,
    NmActionResponseCommandNewMail,
    NmActionResponseCommandSendMail,
    NmActionResponseCommandPriorityHigh,
    NmActionResponseCommandPriorityLow,
    NmActionResponseCommandForward,
    NmActionResponseCommandAttach,
    NmActionResponseCommandReply,
    NmActionResponseCommandReplyAll,
    NmActionResponseCommandDeleteMail,
    NmActionResponseCommandUpdateMailboxName,
    NmActionResponseCommandMailboxDeleted,
    NmActionResponseCommandRemoveAttachment,
    NmActionResponseCommandOpenAttachment,
    NmActionResponseCommandSearch,
    NmActionResponseCommandSwitchFolder,
    NmActionResponseCommandSettings
};


/*!
    \class NmActionResponse
    \brief Extension calls NmActionObserver to handle NmAction command.
    \Response should encapsulated using this class and relayed to observer.
*/
class NmActionResponse
{
public:
    inline NmActionResponse(NmActionResponseCommand responseCommand,
    	NmActionContextMenuType menuType=NmActionOptionsMenu,
    	NmActionContextView contextView=NmActionContextViewNone,
    	NmId mailboxId=0,
    	NmId folderId=0,
    	NmId messageId=0
    	)
    : mResponseCommand(responseCommand),
    mMenuType(menuType),
    mContextView(contextView),
    mMailboxId(mailboxId),
    mFolderId(folderId),
    mMessageId(messageId) {  };

    inline NmActionResponse(NmActionResponseCommand responseCommand,
                   const NmActionRequest &actionRequest)
    : mResponseCommand(responseCommand),
    mMenuType(actionRequest.menuType()),
    mContextView(actionRequest.contextView()),
    mMailboxId(actionRequest.mailboxId()),
    mFolderId(actionRequest.folderId()),
    mMessageId(actionRequest.messageId()) {  };

    inline NmActionResponseCommand responseCommand() const {return mResponseCommand;}
    inline NmActionContextMenuType menuType() const {return mMenuType;}
    inline NmActionContextView contextView()const {return mContextView;}
    inline NmId mailboxId()const {return mMailboxId;}
    inline NmId folderId() const {return mFolderId;}
    inline NmId messageId() const {return mMessageId;}

private:
    NmActionResponseCommand mResponseCommand;
    NmActionContextMenuType mMenuType;
    NmActionContextView mContextView;
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;
};


#endif /* NMMENURESPONSE_H_ */
