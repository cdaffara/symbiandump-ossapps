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
   constructor for NmApiMessageBody
 */
NmApiMessageBody::NmApiMessageBody()
{
    NM_FUNCTION;
    
    d = new NmApiMessageBodyPrivate();
}

/*!
   copying constructor for NmApiMessageBody
 */
NmApiMessageBody::NmApiMessageBody(const NmApiMessageBody &apiMessageBody) : d(apiMessageBody.d)
{
    NM_FUNCTION;
}

/*!
   assignment operator for nmapimessagebody
 */
NmApiMessageBody& NmApiMessageBody::operator=(const NmApiMessageBody &apiMessageBody)
{
    NM_FUNCTION;
    
	if (this != &apiMessageBody) 
	{
		d = apiMessageBody.d;
	}
	return *this;
}

/*!
   Compare data from \a apiMessageBody
 */    
bool NmApiMessageBody::operator==(const NmApiMessageBody &apiMessageBody)
{
    NM_FUNCTION;
    
    bool retVal = false;
    if (this->d == apiMessageBody.d)
    {
        retVal = true;
    }
    return retVal;
}

/*!
   Destructor for NmApiMessageBody
 */
NmApiMessageBody::~NmApiMessageBody()
{
    NM_FUNCTION;
}
/*!
   getter for total size of message body in bytes
 */
quint64 NmApiMessageBody::totalSize() const
{
    NM_FUNCTION;
    
    return d->totalSize;
}

/*!
   getter for bytes available in local mailbox store
 */
quint64 NmApiMessageBody::fetchedSize() const
{
    NM_FUNCTION;
    
    return d->fetchedSize;
}

/*!
   getter for fetched content
 */
QString NmApiMessageBody::content() const
{
    NM_FUNCTION;
    
    return d->content;
}

/*!
   setter for total size of message body in bytes
 */
void NmApiMessageBody::setTotalSize(quint64 size)
{
    NM_FUNCTION;
    
    d->totalSize = size;
}

/*!
   setter for bytes available on local mailbox store
 */
void NmApiMessageBody::setFetchedSize(quint64 size)
{
    NM_FUNCTION;
    
    d->fetchedSize = size;
}

/*!
   setter for content
 */
void NmApiMessageBody::setContent(const QString& content)
{
    NM_FUNCTION;
    
    d->content = content;
}
}

