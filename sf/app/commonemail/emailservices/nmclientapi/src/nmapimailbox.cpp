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

#include "nmapiheaders.h"

namespace EmailClientApi
{

/*!
    constructor for NmApiMailbox 
 */
NmApiMailbox::NmApiMailbox()
{
    NM_FUNCTION;
    
    d = new NmApiMailboxPrivate();
}

/*!
    Copy constructor for NmApiMailbox 
 */
NmApiMailbox::NmApiMailbox(const NmApiMailbox &nmApiMailbox)
: d(nmApiMailbox.d)
{
    NM_FUNCTION;
}

/*!
   destructor for NmApiMailbox
 */
NmApiMailbox::~NmApiMailbox()
{
    NM_FUNCTION;
}

/*!
   Assign data from \a mailbox
 */
NmApiMailbox &NmApiMailbox::operator=(const NmApiMailbox &mailbox)
{
    NM_FUNCTION;
    
    if (this != &mailbox) {
        d = mailbox.d;
    }
    return *this;
}

/*!
   Compare data from \a mailbox
 */
bool NmApiMailbox::operator==(const NmApiMailbox &mailbox)
{
    NM_FUNCTION;
    
    bool returnValue = false;
    if (d == mailbox.d) {
        returnValue = true;
    }
    return returnValue;
}

/*!
   getter for id
 */
quint64 NmApiMailbox::id() const
{
    NM_FUNCTION;
    
    return d->id;
}

/*!
   getter for address
 */
QString NmApiMailbox::address() const
{
    NM_FUNCTION;
    
    return d->address;
}

/*!
   getter for name
 */
QString NmApiMailbox::name() const
{
    NM_FUNCTION;
    
    return d->name;
}

/*!
   setter for id
 */
void NmApiMailbox::setId(quint64 id)
{
    NM_FUNCTION;
    
    d->id = id;
}

/*!
   setter for address
 */
void NmApiMailbox::setAddress(const QString &address)
{
    NM_FUNCTION;
    
    d->address = address;
}

/*!
   setter for name
 */
void NmApiMailbox::setName(const QString &name)
{
    NM_FUNCTION;
    
    d->name = name;
}

}

