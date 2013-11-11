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
*       Api's email message private implementation.
*/

#ifndef NMAPIMESSAGE_P_H_
#define NMAPIMESSAGE_P_H_

namespace EmailClientApi
{

class NmApiMessagePrivate : public QSharedData
{

public:
    NmApiMessagePrivate();
    NmApiMessagePrivate(const NmApiMessagePrivate &other);
    virtual ~NmApiMessagePrivate();

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
    NmApiTextContent mPlainTextContent;
    NmApiTextContent mHtmlContent;
    bool mComplete;
    QList<NmApiAttachment> mAttachments;
    NmApiMessageEnvelope mEnvelope;
};
}

#endif /* NMAPIMESSAGE_P_H_ */
