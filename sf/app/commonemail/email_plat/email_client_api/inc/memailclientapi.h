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
* Description: ECom interface for Email Client API
*
*/


#ifndef MEMAILCLIENTAPI_H
#define MEMAILCLIENTAPI_H

#include <emailapidefs.h>

namespace EmailInterface {

class MEmailMailbox;


typedef RPointerArray<MEmailMailbox> RMailboxPtrArray;

/**
 * Email client main interface for accessing mailboxes and launching
 * email editor and viewer.
 * Use CEmailInterfaceFactory::InterfaceL( KEmailClientApiInterface )
 * to get instance. Use Release() (from MEmailInterface) when the instance 
 * is no more needed.
 * 
 */
class MEmailClientApi : public MEmailInterface
{
public:
    
    /** returns mailbox by id, ownership transferred */
    virtual MEmailMailbox* MailboxL( const TMailboxId& aId ) = 0;
    
    /** 
     * Conveninence method for accessing mailbox by own addresss.
     */
    virtual MEmailMailbox* MailboxL( const TPtrC& aAddress ) = 0;
        
    /**
    * Gets all mailboxes in pointer array. It is recommended to free other
    * mailboxes besides needed one to optimise memory usage.
    */    
    virtual TInt GetMailboxesL( RMailboxPtrArray& aMailboxes ) = 0;

    /**
    * Launch policy defines behavior when launching email application
    */
    enum TLaunchPolicy {
        // Launch email application as from menu application by the user
        EDefault,
        // Shows inbox with unseen messages. If there are no unseen messages
        // behavior is same as EDefault. This one is not currently supported.
        EShowLastUnseenMailbox
    };
    /**
     * Launches Email application with defined launch behavior
     * The method follows "fire and forget" pattern, returns immediately.
     * @param aPolicy defines behavior for app launch
     */
    virtual void LaunchEmailL( const TLaunchPolicy aPolicy ) = 0;

};

}  // namespace EmailInterface

#endif // MEMAILCLIENTAPI_H
