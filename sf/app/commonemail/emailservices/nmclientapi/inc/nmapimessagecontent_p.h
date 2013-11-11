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
*       Email message content private implementation.
*/

#ifndef NMAPIMESSAGECONTENT_P_H_
#define NMAPIMESSAGECONTENT_P_H_

namespace EmailClientApi
{

class NmApiMessageContentPrivate : public QSharedData
{
public:
    NmApiMessageContentPrivate();
    NmApiMessageContentPrivate(const NmApiMessageContentPrivate &messagecontentprivate);
    virtual ~NmApiMessageContentPrivate();

    QString contentType() const;
    bool isComplete();
    quint32 size() const;
    quint64 id() const;
    
    void setContentType(const QString &contentType);
    void setComplete(bool complete);
    void setSize(quint32 size);
    void setId(quint64 id);

private:
    
    QString mContentType;
    bool mComplete;
    quint32 mSize;
    quint64 mId;
};
}

#endif /* NMAPIMESSAGEENVELOPE_P_H_ */
