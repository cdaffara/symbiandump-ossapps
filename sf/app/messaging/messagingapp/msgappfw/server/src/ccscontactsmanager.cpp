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
 * Description:QT contact manager interface
 *
 */

// System includes
#include <QList>
// User includes
#include "ccscontactsmanager.h"
#include "ccscontactsresolver.h"

// ========================== Member Functions ===============================

// ----------------------------------------------------------------------------
// ContactsManager::ContactsManager
// @see contactsmanager.h
// ----------------------------------------------------------------------------
CCsContactsManager::CCsContactsManager(QObject* parent):QObject(parent)
{
    mPhonebookManager = new QContactManager("symbian");

    // Connect to QContactManager signals
    connect(mPhonebookManager, 
            SIGNAL(contactsAdded(const QList<QContactLocalId>&)), 
            this, 
            SLOT(handleContactsAdded(const QList<QContactLocalId>&)));

    connect(mPhonebookManager, 
            SIGNAL(contactsChanged(const QList<QContactLocalId>&)), 
            this, 
            SLOT(handleContactsChanged(const QList<QContactLocalId>&)));

    connect(mPhonebookManager, 
            SIGNAL(contactsRemoved(QList<QContactLocalId>)), 
            this, 
            SLOT(handleContactsRemoved(QList<QContactLocalId>)));

    mContactsResolver = new CCsContactsResolver(mPhonebookManager);
}
// ----------------------------------------------------------------------------
// ContactsManager::~ContactsManager
// @see contactsmanager.h
// ----------------------------------------------------------------------------
CCsContactsManager::~CCsContactsManager()
{
    delete mContactsResolver;
    mContactsResolver = NULL;
    delete mPhonebookManager;
    mPhonebookManager = NULL;
    mObserverList.clear();
}
// ----------------------------------------------------------------------------
// ContactsManager::resolver
// @see contactsmanager.h
// ----------------------------------------------------------------------------
CCsContactsResolver* CCsContactsManager::resolver() const
{
    return mContactsResolver;
}

// ----------------------------------------------------------------------------
// ContactsManager::handleContactsAdded
// @see contactsmanager.h
// ----------------------------------------------------------------------------
void CCsContactsManager::handleContactsAdded(
                                          const QList<QContactLocalId>& contactIds)
{
    int contactsCount = contactIds.count();
    for (int i = 0; i < contactsCount; ++i)
    {
        CCsContactDetail detail;        
        mContactsResolver->resolveContactId(contactIds.at(i), detail);
                           
        //notify observers
        int count = mObserverList.count();
        for (int observerCount=0; observerCount < count; observerCount++)
        {
            MCsContactsManagerObserver* observer = mObserverList.at(observerCount);
            observer->HandleAddContact(detail);
        }            
    }
}

// ----------------------------------------------------------------------------
// ContactsManager::handleContactsChanged
// @see contactsmanager.h
// ----------------------------------------------------------------------------
void CCsContactsManager::handleContactsChanged(
                                            const QList<QContactLocalId>& contactIds)
{
    int contactsCount = contactIds.count();
    for (int i = 0; i < contactsCount; ++i)
    {
        CCsContactDetail detail;
        mContactsResolver->resolveContactId(contactIds.at(i), detail);
        
        //notify observers
        int count = mObserverList.count();
        for (int observerCount=0; observerCount < count; observerCount++)
        {
            MCsContactsManagerObserver* observer = mObserverList.at(observerCount);
            observer->HandleContactChange(detail);
        }    
    }
}

// ----------------------------------------------------------------------------
// ContactsManager::handleContactsRemoved
// @see contactsmanager.h
// ----------------------------------------------------------------------------
void CCsContactsManager::handleContactsRemoved(const QList<QContactLocalId> &contactIds)
{
    CCsContactDetail detail;
    int idCount = contactIds.count();
    for (int i = 0; i < idCount; i++)
    {
        detail.contactId = contactIds.value(i);

        //notify observers
        int count = mObserverList.count();
        for (int observerCount=0; observerCount < count; observerCount++)
        {
            MCsContactsManagerObserver* observer = mObserverList.at(observerCount);
            observer->HandleDeleteContact(detail);
        } 
    }

}

// ---------------------------------------------------------------------------
// CCsContactsManager::addObserver
// Register for events.
// ---------------------------------------------------------------------------
void CCsContactsManager::addObserver(MCsContactsManagerObserver* aObserver)
{
    mObserverList.append(aObserver);
}

// ---------------------------------------------------------------------------
// CCsContactsManager::removeObserver
// DeRegister for events.
// ---------------------------------------------------------------------------
void CCsContactsManager::removeObserver(MCsContactsManagerObserver* aObserver)
{
    int index = mObserverList.indexOf(aObserver);
    if ( index >= 0)
    {
        mObserverList.removeAt(index);
    }
}

// EOF
