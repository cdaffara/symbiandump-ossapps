/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event interface for the contacts manager.
 *
*/


#ifndef M_CS_CONTACTS_MANAGER_OBSERVER_H
#define M_CS_CONTACTS_MANAGER_OBSERVER_H

class CCsContactDetail;

/**
 * This observer interface is the internal callback interface between the
 * contacts manager and conversation server.
 *
 *  @since S60 v10.1
 */
class MCsContactsManagerObserver 
    {

public:

    /**
     * HandleAddContact
     * This asynchronous callback is invoked when the new contact is added.
     */ 
    virtual void HandleAddContact(CCsContactDetail& aDetail) = 0;

    /**
     * HandleContactChange
     * This asynchronous callback is invoked when the contact is changed.
     */
    virtual void HandleContactChange(CCsContactDetail& aDetail) = 0;
    
    /**
     * HandleDeleteContact
     * This asynchronous callback is invoked when the contact is deleted.
     */
    virtual void HandleDeleteContact(CCsContactDetail& aDetail) = 0;

    };


#endif // M_CS_CONTACTS_MANAGER_OBSERVER_H
