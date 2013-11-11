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
*       Email message content public header.
*/

#ifndef NMAPIMESSAGECONTENT_H_
#define NMAPIMESSAGECONTENT_H_

#include <nmapidef.h>
#include <QSharedDataPointer>

namespace EmailClientApi
{
class NmApiMessageContentPrivate;

class NMAPI_EXPORT NmApiMessageContent
{
public:
    NmApiMessageContent();
    NmApiMessageContent(const NmApiMessageContent &messagecontent);
    virtual ~NmApiMessageContent();

    NmApiMessageContent& operator=(const NmApiMessageContent &messagecontent);    

    QString contentType() const;
    bool isComplete();
    quint32 size() const;
    quint64 id() const;
    
    void setContentType(const QString &contentType);
    void setComplete(bool complete);
    void setSize(quint32 size);
    void setId(quint64 id);

private:
    
    QSharedDataPointer<NmApiMessageContentPrivate> d;
};
}
#endif /*NMAPIMESSAGECONTENT_H_ */
