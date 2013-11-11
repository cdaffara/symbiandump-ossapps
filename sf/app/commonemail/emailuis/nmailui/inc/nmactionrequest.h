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

#ifndef NMACTIONREQUEST_H_
#define NMACTIONREQUEST_H_

#include "nmcommon.h"

/*!
    \enum NmActionContextMenuType
    \brief This value indicates action type that is requested using
    \action request class NmActionRequest and responded with NmActionResponse
*/
enum NmActionContextMenuType
{
    NmActionOptionsMenu=0,
    NmActionContextMenu,
    NmActionToolbar,
    NmActionVKB
};

/*!
    \enum NmActionContextView
    \brief This value defines context menu selection type
*/
enum NmActionContextView
{
	NmActionContextViewNone=0,
	NmActionContextViewMailboxList,
	NmActionContextViewMessageList,
	NmActionContextViewMessageSearchList,
	NmActionContextViewViewer,
	NmActionContextViewEditor
};

/*!
    \enum NmActionContextDataType
    \brief This value defines data of context item
*/
enum NmActionContextDataType
{
	NmActionContextDataNone=0,
	NmActionContextDataMailbox,
	NmActionContextDataFolder,
	NmActionContextDataMessage
};



class NmActionObserver;

/*!
    \class NmActionRequest
    \brief NmUi calls extension manager to invoke menu. Context
    \information should be encapsulated using this class.
    \QVariant requestData can be used to relay NmActionContextDataType based
    \info such as message envelope, etc
*/
class NmActionRequest
{
public:
    inline NmActionRequest(NmActionObserver *observer,
		NmActionContextMenuType menuType=NmActionOptionsMenu,
		NmActionContextView contextView=NmActionContextViewNone,
		NmActionContextDataType contextDataType=NmActionContextDataNone,
    	NmId mailboxId=0,
   	    NmId folderId=0,
    	NmId messageId=0,
    	QVariant requestData=0
    	)
    : mObserver(observer),
    mMenuType(menuType),
    mContextView(contextView),
    mContextDataType(contextDataType),
    mMailboxId(mailboxId),
    mFolderId(folderId),
    mMessageId(messageId),
    mRequestData(requestData){};

    inline NmActionObserver *observer() const {return mObserver;}
    inline NmActionContextMenuType menuType() const {return mMenuType;}
    inline NmActionContextView contextView() const {return mContextView;}
    inline NmActionContextDataType contextDataType() const {return mContextDataType;}
    inline NmId mailboxId() const {return mMailboxId;}
    inline NmId folderId() const {return mFolderId;}
    inline NmId messageId() const {return mMessageId;}
    inline QVariant requestData() const {return mRequestData;}

private:
    NmActionObserver *mObserver;
    NmActionContextMenuType mMenuType;
    NmActionContextView mContextView;
    NmActionContextDataType mContextDataType;
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;
    QVariant mRequestData;
};

#endif /* NMACTIONREQUEST_H_ */
