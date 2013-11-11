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
*		Api's email message class.
*/

#ifndef NMAPIMESSAGE_H_
#define NMAPIMESSAGE_H_

#include <nmapitextcontent.h>
#include <nmapiattachment.h>
#include <nmapimessageenvelope.h>
#include <nmapidef.h>
#include <QMetaType>

namespace EmailClientApi
{
class NmApiMessagePrivate;
class NmApiMessageEnvelope;

class NMAPI_EXPORT NmApiMessage
{
public:
    NmApiMessage();
    NmApiMessage(const NmApiMessage &other);
    virtual ~NmApiMessage();

    NmApiMessage& operator=(const NmApiMessage &other);    

    NmApiTextContent plainTextContent();
    NmApiTextContent htmlContent();
    bool isComplete();
    QList<NmApiAttachment> attachments();
    void addAttachment(NmApiAttachment &attachment);
    NmApiMessageEnvelope& envelope();
    void setPlainTextContent(NmApiTextContent &textContent);
    void setHtmlContent(NmApiTextContent &textContent);
    void setComplete(bool complete);
    void setAttachments(QList<NmApiAttachment> &attachments);
    void setEnvelope(NmApiMessageEnvelope &envelope);
    
private:

    QSharedDataPointer<NmApiMessagePrivate> d;
};
} // namespace EmailClientApi

Q_DECLARE_METATYPE(EmailClientApi::NmApiMessage)

#endif /* NMAPIMESSAGE_H_ */
