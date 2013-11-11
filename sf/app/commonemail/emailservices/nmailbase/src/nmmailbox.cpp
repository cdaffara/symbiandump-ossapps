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

#include "emailtrace.h"

#include <QString>

#include "nmmailbox.h"

NmMailboxPrivate::NmMailboxPrivate()
{
    NM_FUNCTION;
}

NmMailboxPrivate::~NmMailboxPrivate()
{
    NM_FUNCTION;
}

/*!
    \class NmMailbox
    \brief Data model for mailbox spesific data
 */

/*!
    Constructs NmMailbox object with empty data
 */
NmMailbox::NmMailbox()
{
    NM_FUNCTION;
    
    d = new NmMailboxPrivate();
}

/*!
    Copy constructor
 */
NmMailbox::NmMailbox(const NmMailbox &mailbox)
{
    NM_FUNCTION;
    
    d = mailbox.d;
}

/*!
    Constructs NmMailbox object from \a mailbox data
 */
NmMailbox::NmMailbox(QExplicitlySharedDataPointer<NmMailboxPrivate> mailboxprivate)
{
    NM_FUNCTION;
    
    d = mailboxprivate;
}

/*!
    Assign data from \a mailbox
 */
NmMailbox &NmMailbox::operator=(const NmMailbox &mailbox)
{
    NM_FUNCTION;
    
    if (this != &mailbox) {
        d = mailbox.d;
    }
    return *this;
}

/*!
    Destructor
 */
NmMailbox::~NmMailbox()
{
    NM_FUNCTION;
}

/*!
    Returns mailbox id
 */
NmId NmMailbox::id() const
{
    NM_FUNCTION;
    
    return d->mId;
}

/*!
    Sets mailbox id from \a id
 */
void NmMailbox::setId(const NmId& id)
{
    NM_FUNCTION;
    
    d->mId = id;
}

/*!
    Returns mailbox name string
 */
QString NmMailbox::name() const
{
    NM_FUNCTION;
    
    return d->mName;
}

/*!
    Sets mailbox name from \a name
 */
void NmMailbox::setName(const QString &name)
{
    NM_FUNCTION;
    
    d->mName = name;
}

/*!
    Equal operator returns true if both NmMailbox objects have same name and id
 */
bool NmMailbox::operator==(const NmMailbox &mailbox) const
{
    NM_FUNCTION;
    
    bool ret = true;

    if (this->name().compare(mailbox.name()) != 0) {
        ret = false;
    }
    if (this->id() != mailbox.id()) {
        ret = false;
    }
    return ret;
}


NmAddress NmMailbox::address() const
{
    NM_FUNCTION;
    
    return d->mAddress;
}


void NmMailbox::setAddress(const NmAddress& address)
{
    NM_FUNCTION;
    
    d->mAddress = address;
}

