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
 *      Api's email message implementation.
 */

#include "nmapiheaders.h"

namespace EmailClientApi
{

/*!
    Class constructor.
*/
NmApiMessage::NmApiMessage()
{
    NM_FUNCTION;
    d = new NmApiMessagePrivate();
}

/*!
    Class copy constructor.
*/
NmApiMessage::NmApiMessage(const NmApiMessage &other)
{
    NM_FUNCTION;
    d = other.d;
}

/*!
    Class destructor.
*/
NmApiMessage::~NmApiMessage()
{
    NM_FUNCTION;
}

NmApiMessage& NmApiMessage::operator=(const NmApiMessage &other)
{
    if (this == &other)
        return *this;
    
    d = other.d;

    return *this;
}

/*!
 \fn plainTextContent 
 \return Message plain text content.
 
 Returns message plain text content.
 */
NmApiTextContent NmApiMessage::plainTextContent()
{
    NM_FUNCTION;
    return d->plainTextContent();
}

/*!
 \fn htmlContent 
 \return Message html text content.
 
 Returns message html text content.
 */
NmApiTextContent NmApiMessage::htmlContent()
{
    NM_FUNCTION;
    return d->htmlContent();
}

/*!
 \fn isComplete 
 \return Message complete state.
 
 Returns message complete state.
 */
bool NmApiMessage::isComplete()
{
    NM_FUNCTION;
    return d->isComplete();
}

/*!
 \fn getAttachments 
 \return Message attachment list.
 
 Returns message attachment list.
 */
QList<NmApiAttachment> NmApiMessage::attachments()
{
    NM_FUNCTION;
    return d->attachments();
}


/*!
 \fn addAttachment 
 \param attachment Attachment.
 
 Add attachment to message.
 */
void NmApiMessage::addAttachment(NmApiAttachment &attachment)
{
    NM_FUNCTION;
    d->addAttachment(attachment);
}

/*!
 \fn envelope 
 \return Message envelope.
 
 Returns message envelope.
 */
NmApiMessageEnvelope& NmApiMessage::envelope()
{
    NM_FUNCTION;
    return d->envelope();
}

/*!
 \fn setPlainTextContent 
 \param textContent Content.
 
 Setter for plain text content.
 */
void NmApiMessage::setPlainTextContent(NmApiTextContent &textContent)
{
    NM_FUNCTION;
    d->setPlainTextContent(textContent);
}

/*!
 \fn setHtmlContent 
 \param textContent Content.
 
 Setter for html text content.
 */
void NmApiMessage::setHtmlContent(NmApiTextContent &textContent)
{
    NM_FUNCTION;
    d->setHtmlContent(textContent);
}

/*!
 \fn setComplete 
 \param complete Complete state.
 
 Setter for message complete state.
 */
void NmApiMessage::setComplete(bool complete)
{
    NM_FUNCTION;
    d->setComplete(complete);
}

/*!
 \fn setAttachments 
 \param attachments Attachments.
 
 Setter for set attachments to message.
 */
void NmApiMessage::setAttachments(QList<NmApiAttachment> &attachments)
{
    NM_FUNCTION;
    d->setAttachments(attachments);
}

/*!
 \fn setEnvelope 
 \param envelope Envelope.
 
 Setter for message envelope.
 */
void NmApiMessage::setEnvelope(NmApiMessageEnvelope &envelope)
{
    NM_FUNCTION;
    d->setEnvelope(envelope);
}

}
