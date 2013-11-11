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
   copying constructor for nmmessageenvelope
 */
NmApiEmailAddress::NmApiEmailAddress(const NmApiEmailAddress &envelope) :
    d(envelope.d)
{
    NM_FUNCTION;
}

/*!
   Constructor for NmApiEmailAddress class
 */
NmApiEmailAddress::NmApiEmailAddress()
{
    NM_FUNCTION;
    
    d = new NmApiEmailAddressPrivate();
}

/*!
   destructor
 */
NmApiEmailAddress::~NmApiEmailAddress()
{
    NM_FUNCTION;
}

/*!
   Assignment operator
 */
NmApiEmailAddress &NmApiEmailAddress::operator=(const NmApiEmailAddress &addr)
{
    NM_FUNCTION;
    
    if (this != &addr) {
        d = addr.d;
    }
    return *this;
}

/*!
   Compare data from \a addr
 */
bool NmApiEmailAddress::operator==(const NmApiEmailAddress &addr)
{
    NM_FUNCTION;
    
    bool retVal = false;
    if (this->d == addr.d) {
        retVal = true;
    }
    return retVal;
}

/*!
   getter for displayname
 */
QString NmApiEmailAddress::displayName() const
{
    NM_FUNCTION;
    
    return d->displayName;
}

/*!
   getter for address
 */
QString NmApiEmailAddress::address() const
{
    NM_FUNCTION;
    
    return d->address;
}

/*!
   setter for displayname 
 */
void NmApiEmailAddress::setDisplayName(const QString &displayName)
{
    NM_FUNCTION;
    
    d->displayName = displayName;
}

/*!
   setter for address
 */
void NmApiEmailAddress::setAddress(const QString &address)
{
    NM_FUNCTION;
    
    d->address = address;
}
}
