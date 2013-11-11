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
* Description: Observer interface for mailbox events.
*
*/

#ifndef MMAILBOXCONTENTOBSERVER_H
#define MMAILBOXCONTENTOBSERVER_H

#include <emailapidefs.h>

namespace EmailInterface {

class MMailboxContentObserver
{
public:
    virtual void NewMessageEventL( const TMailboxId& aMailbox, const REmailMessageIdArray aNewMessages, const TFolderId& aParentFolderId ) = 0; 
    
    virtual void MessageChangedEventL( const TMailboxId& aMailbox, const REmailMessageIdArray aChangedMessages, const TFolderId& aParentFolderId ) = 0;
    
    virtual void MessageDeletedEventL( const TMailboxId& aMailbox, const REmailMessageIdArray aDeletedMessages, const TFolderId& aParentFolderId ) = 0;
};

} // namespace EmailInterface

#endif // MMAILBOXCONTENTOBSERVER_H
