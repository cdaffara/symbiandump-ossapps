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
 * Description: QT contact resolver interface
 *
 */

// SYSTEM INCLUDES
#include "qtcontacts.h"
#include "qcontactdetailfilter.h"
#include <QList>

// USER INCLUDES
#include "ccscontactsresolver.h"
// ======================== Member Functions ====================

// ----------------------------------------------------------------------------
// CCsContactsResolver::ContactsResolver
// @see contactsresolver.h
// ----------------------------------------------------------------------------
CCsContactsResolver::CCsContactsResolver(QContactManager* manager):
mPhonebookManager(manager)
        {
        // do nothing
        }
// ----------------------------------------------------------------------------
// CCsContactsResolver::~ContactsResolver
// @see contactsresolver.h
// ----------------------------------------------------------------------------
CCsContactsResolver::~CCsContactsResolver()
    {
    // do nothing
    }

// ----------------------------------------------------------------------------
// CCsContactsResolver::resolveContact
// @see contactsresolver.h
// ----------------------------------------------------------------------------
bool CCsContactsResolver::resolveContact(
        const QString &address,
        CCsContactDetail &contactDetail)
    {
    // apply filter on phone number field
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(
            QContactPhoneNumber::DefinitionName,
            QContactPhoneNumber::FieldNumber);

    phoneFilter.setValue(address);
    phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);
    QList<QContact> matchingContacts = mPhonebookManager->contacts(phoneFilter);
    if ( matchingContacts.count() > 0 ) {
        QContact match = matchingContacts.at(0);
        // Fill the contact details
        contactDetail.contactId = match.localId();
        contactDetail.displayName = match.displayLabel();
        return true;
    }

    // apply filter on email address field
    QContactDetailFilter emailFilter;
    emailFilter.setDetailDefinitionName(
            QContactEmailAddress::DefinitionName,
            QContactEmailAddress::FieldEmailAddress);

    emailFilter.setValue(address);
    emailFilter.setMatchFlags(QContactFilter::MatchExactly);
    matchingContacts = mPhonebookManager->contacts(emailFilter);
    if ( matchingContacts.count() > 0 ) {
        QContact match = matchingContacts.at(0);
        // Fill the contact details
        contactDetail.contactId = match.localId();
        contactDetail.displayName = match.displayLabel();
        return true;
    }
    return false;
    }

// ----------------------------------------------------------------------------
// CCsContactsResolver::resolveContactId
// @see contactsresolver.h
// ----------------------------------------------------------------------------
void CCsContactsResolver::resolveContactId(
        const quint32 &contactId,
        CCsContactDetail &contactDetail)
    {
    // Fetch back the persisted contact
    QContact contact = mPhonebookManager->contact(contactId);
    contactDetail.contactId = contact.localId();

    contactDetail.displayName = contact.displayLabel();

    QList<QContactPhoneNumber> numbers = contact.details<QContactPhoneNumber>();
    int numberCount = numbers.count();

    for ( int a=0; a<numberCount ;++a)
        {
        QString phoneNumber= numbers.at(a).number();
        contactDetail.addressList.append(phoneNumber);
        }

    QList<QContactEmailAddress> emailAddresses = contact.details<QContactEmailAddress>();
    int emailCount = emailAddresses.count();

    for ( int a=0; a<emailCount ;++a)
        {
        QString emailAddr= emailAddresses.at(a).emailAddress();
        contactDetail.addressList.append(emailAddr);
        }
    }

// EOF
