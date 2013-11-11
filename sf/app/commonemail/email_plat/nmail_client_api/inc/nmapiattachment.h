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
 *     Email message attachment public header.
 */

#ifndef NMAPIATTACHMENT_H_
#define NMAPIATTACHMENT_H_

#include <QString>
#include <QSharedDataPointer>
#include <xqsharablefile.h>
#include <nmapimessagecontent.h>
#include <nmapidef.h>

namespace EmailClientApi
{
class NmApiAttachmentPrivate;

class NMAPI_EXPORT NmApiAttachment : public NmApiMessageContent
{
public:
    NmApiAttachment();
    NmApiAttachment(const NmApiAttachment &attachment);
    virtual ~NmApiAttachment();

    NmApiAttachment& operator=(const NmApiAttachment &attachment);    

    QString fileName();
    XQSharableFile file();

    void setFileName(const QString &fileName);

private:
    
    QSharedDataPointer<NmApiAttachmentPrivate> d;
};
}

#endif /* NMAPIATTACHMENT_H_ */
