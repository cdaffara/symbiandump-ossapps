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
* Description:
*       Email message content implementation.
*/

#include "nmapiheaders.h"

namespace EmailClientApi
{

/*!
   Constructor for NmApiMessageContent
 */
NmApiMessageContent::NmApiMessageContent()
{
    NM_FUNCTION;
    d = new NmApiMessageContentPrivate();
}

/*!
    Class copy constructor.
*/
NmApiMessageContent::NmApiMessageContent(const NmApiMessageContent &messagecontent)
{
    NM_FUNCTION;
    d = messagecontent.d;
}

/*!
   Destructor for NmApiMessageContent
 */
NmApiMessageContent::~NmApiMessageContent()
{
    NM_FUNCTION;
}

NmApiMessageContent& NmApiMessageContent::operator=(const NmApiMessageContent &messagecontent)
{
    if (this == &messagecontent)
        return *this;
    
    d = messagecontent.d;

    return *this;
}

/*!
   Getter for content type
 */
QString NmApiMessageContent::contentType() const
{
    NM_FUNCTION;
    return d->contentType();
}

/*!
   Getter for is complete
 */
bool NmApiMessageContent::isComplete()
{
    NM_FUNCTION;
    return d->isComplete();
}

/*!
   Getter for size
 */
quint32 NmApiMessageContent::size() const
{
    NM_FUNCTION;
    return d->size();
}

/*!
   Getter for id
 */
quint64 NmApiMessageContent::id() const
{
     NM_FUNCTION;
     return d->id();
}

/*!
   Setter for content type
 */
void NmApiMessageContent::setContentType(const QString &contentType)
{
    NM_FUNCTION;
    d->setContentType(contentType);
}

/*!
   Setter for is complete
 */
void NmApiMessageContent::setComplete(bool complete)
{
    NM_FUNCTION;
    d->setComplete(complete);
}

/*!
   Setter for size
 */
void NmApiMessageContent::setSize(quint32 size)
{
    NM_FUNCTION;
    d->setSize(size);
}

/*!
   Setter for id
 */
void NmApiMessageContent::setId(quint64 id)
{
    NM_FUNCTION;
    d->setId(id);
}

}
