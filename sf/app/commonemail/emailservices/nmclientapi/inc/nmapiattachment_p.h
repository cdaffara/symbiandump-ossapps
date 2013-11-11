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
*       Email message attachment private header.
*/

#ifndef NMAPIATTACHMENT_P_H_
#define NMAPIATTACHMENT_P_H_

namespace EmailClientApi
{

class NmApiAttachmentPrivate : public QSharedData
{

public:
    NmApiAttachmentPrivate();
    NmApiAttachmentPrivate(const NmApiAttachmentPrivate &attachmentprivate);
    virtual ~NmApiAttachmentPrivate();

    QString fileName();
    XQSharableFile file();

    void setFileName(const QString &fileName);

private:
    QString mFilename;
};
}

#endif /* NMAPIATTACHMENT_P_H_ */
