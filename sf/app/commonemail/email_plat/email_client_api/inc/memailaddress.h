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
* Description: Email address in Email Client API
*
*/

#ifndef MEMAILADDRESS_H
#define MEMAILADDRESS_H

#include <emailapidefs.h>

namespace EmailInterface {

/**
 * Email address that is used as recipient or sender in MEmailMessage and
 * mailbox address in MEmailMailbox.
 * @since S60 v5.0
 */
class MEmailAddress : public MEmailInterface
{ 
public:
    /**
     * Association role of the address in message/mailbox.
     */
    enum TRole { 
        // role is undefined
        EUndefined, 
        // reply-to address in a message
        EReplyTo, 
        // sender of a message
        ESender, 
        // to-recipient of a message
        ETo, 
        // cc-recipient of a message
        ECc, 
        // bcc-recipient of a message
        EBcc };
    
    /**
     * Sets email address
     * @param email address 
     */
    virtual void SetAddressL( const TDesC& aAddress ) = 0;
    
    /**
     * Returns email address
     * @return email address
     */
    virtual TPtrC Address() const = 0;
    
    /**
     * Sets display name for the address
     * @param display name
     */
    virtual void SetDisplayNameL( const TDesC& aDisplayName ) = 0;
    
    /**
     * Returns display name for the address
     * @return display name
     */
    virtual TPtrC DisplayName() const = 0;
    
    /**
     * Returns role associated with the address
     * @return role
     */
    virtual TRole Role() const = 0;
    
    /**
     * Sets role
     * @param role
     */
    virtual void SetRole( const TRole aRole ) = 0;
};

typedef RPointerArray<MEmailAddress>    REmailAddressArray;

} // namespace EmailInterface {

#endif // MEMAILADDRESS_H