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
 *      Email attachment private implementation.
 */

#include "nmapiheaders.h"
#include <xqsharablefile.h>

namespace EmailClientApi
{

/*!
    Class constructor.
*/
NmApiAttachmentPrivate::NmApiAttachmentPrivate()
{
    NM_FUNCTION;
}

/*!
    Class copy constructor.
*/
NmApiAttachmentPrivate::NmApiAttachmentPrivate(const NmApiAttachmentPrivate &attachmentprivate)
 : QSharedData(attachmentprivate),
   mFilename(attachmentprivate.mFilename)
{
    NM_FUNCTION;
}

/*!
    Class destructor.
*/
NmApiAttachmentPrivate::~NmApiAttachmentPrivate()
{
    NM_FUNCTION;
}

/*!
   Getter for filename
 */
QString NmApiAttachmentPrivate::fileName()
{
    NM_FUNCTION;
    return mFilename;
}

/*!
   Getter for file
 */
XQSharableFile NmApiAttachmentPrivate::file()
{
    NM_FUNCTION;
    XQSharableFile *file = new XQSharableFile();
    file->open(mFilename);
    return *file;
}

/*!
   Setter for filename
 */
void NmApiAttachmentPrivate::setFileName(const QString &fileName)
{
    NM_FUNCTION;
    mFilename = fileName;
}

}
