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
 *      Email message attachment implementation.
 */

#include "nmapiheaders.h"

namespace EmailClientApi
{

/*!
    Class constructor.
*/
NmApiAttachment::NmApiAttachment()
{
    NM_FUNCTION;
    d = new NmApiAttachmentPrivate();
}

/*!
    Class copy constructor.
*/
NmApiAttachment::NmApiAttachment(const NmApiAttachment &attachment)
{
    NM_FUNCTION;
    d = attachment.d;
}

/*!
    Class destructor.
*/
NmApiAttachment::~NmApiAttachment()
{
    NM_FUNCTION;
}

NmApiAttachment& NmApiAttachment::operator=(const NmApiAttachment &attachment)
{
    if (this == &attachment)
        return *this;
    
    d = attachment.d;

    return *this;
}

/*!
   Getter for filename
 */
QString NmApiAttachment::fileName()
{
    NM_FUNCTION;
    return d->fileName();
}

/*!
   Getter for file
 */
XQSharableFile NmApiAttachment::file()
{
    NM_FUNCTION;
    return d->file();
}

/*!
   Setter for filename
 */
void NmApiAttachment::setFileName(const QString &fileName)
{
    NM_FUNCTION;
    d->setFileName(fileName);
}

}
