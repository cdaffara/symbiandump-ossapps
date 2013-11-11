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
#ifndef CONTACTS_MANAGER_H
#define CONTACTS_MANAGER_H

// SYSTEM INCLUDES
#include <QObject>
#include <qtcontacts.h>
#include "qtcontactsglobal.h"
// USER INCLUDES
#include "mcscontactsmanagerobserver.h"

QTM_BEGIN_NAMESPACE
// FORWARD DECLARATIONS
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CCsContactsResolver;

// CLASS DECLARATION
/**
 * QT contact manager interface. Creates an instance of virtual
 * phonebook contact manager. Using the QT contacts manager
 * creates resolver objects for use by cache to resolve addresses 
 */
class CCsContactsManager : public QObject
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit CCsContactsManager(QObject* parent = 0);

    /**
     * Destructor
     */
    virtual ~CCsContactsManager();

public:

    /**
     * resolver
     * Provides a reference to the resolver object.
     * 
     * @return The contact resolver interface.
     */
    CCsContactsResolver* resolver() const;

    /**
     * addObserver
     * add observer into the list
     * @param aObserver instance of MCsContactsManagerObserver
     */
    void addObserver(MCsContactsManagerObserver* aObserver);

    /**
     * removeObserver
     * delete observer from the observer list
     * @param aObserver instance of MCsContactsManagerObserver
     */
    void removeObserver(MCsContactsManagerObserver* aObserver);

public slots:

    /**
     * Handle contactsAdded signal from QContactManager
     * Emits phonebookNotification signal
     * @param contactIds unique contact id list
     */
    void handleContactsAdded(const QList<QContactLocalId>& contactIds);

    /**
     * Handle contactsChanged signal from QContactManager
     * Emits phonebookNotification signal
     * @param contactIds unique contact id list
     */
    void handleContactsChanged(const QList<QContactLocalId>& contactIds);

    /**
     * Handle contactsRemoved signal from QContactManager
     * Emits phonebookNotification signal
     * @param id deleted contact id list
     */
    void handleContactsRemoved(const QList<QContactLocalId> &contactIds);

private:

    /**
     * mContactsResolver
     * An instance of the contacts database resolver
     * Own.
     */
    CCsContactsResolver* mContactsResolver;

    /**
     * mPhonebookManager
     * An instance of the QContactManager
     * Own.
     */
    QContactManager* mPhonebookManager;

    /**
     * mObserverList
     * List of Observers
     * Own.
     */
    QList<MCsContactsManagerObserver*> mObserverList;
};

#endif // CONTACTS_MANAGER_H
