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
 *      Email message content private implementation.
 */

#include "nmapiheaders.h"

namespace EmailClientApi
{

/*!
   Constructor for NmApiMessageContentPrivate
 */
NmApiMessageContentPrivate::NmApiMessageContentPrivate()
{
    NM_FUNCTION;
}

/*!
    Class copy constructor.
*/
NmApiMessageContentPrivate::NmApiMessageContentPrivate(const NmApiMessageContentPrivate &messagecontentprivate)
 : QSharedData(messagecontentprivate),
   mContentType(messagecontentprivate.mContentType),
   mComplete(messagecontentprivate.mComplete),
   mSize(messagecontentprivate.mSize),
   mId(messagecontentprivate.mId)
{
    NM_FUNCTION;
}

/*!
   Destructor for NmApiMessageContentPrivate
 */
NmApiMessageContentPrivate::~NmApiMessageContentPrivate()
{
    NM_FUNCTION;
}

/*!
   Getter for content type
 */
QString NmApiMessageContentPrivate::contentType() const
{
    NM_FUNCTION;
    return mContentType;
}

/*!
   Getter for is complete
 */
bool NmApiMessageContentPrivate::isComplete()
{
    NM_FUNCTION;
    return mComplete;
}

/*!
   Getter for size
 */
quint32 NmApiMessageContentPrivate::size() const
{
    NM_FUNCTION;
    return mSize;
}

/*!
   Getter for id
 */
quint64 NmApiMessageContentPrivate::id() const
{
     NM_FUNCTION;
     return mId;
}

/*!
   Setter for content type
 */
void NmApiMessageContentPrivate::setContentType(const QString &contentType)
{
    NM_FUNCTION;
    mContentType = contentType;
}

/*!
   Setter for is complete
 */
void NmApiMessageContentPrivate::setComplete(bool complete)
{
    NM_FUNCTION;
    mComplete = complete;
}

/*!
   Setter for size
 */
void NmApiMessageContentPrivate::setSize(quint32 size)
{
    NM_FUNCTION;
    mSize = size;
}

/*!
   Setter for id
 */
void NmApiMessageContentPrivate::setId(quint64 id)
{
    NM_FUNCTION;
    mId = id;
}

}
