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
NmApiMessagePrivate::NmApiMessagePrivate()
{
    NM_FUNCTION;
}

/*!
    Class copy constructor.
*/
NmApiMessagePrivate::NmApiMessagePrivate(const NmApiMessagePrivate &other)
 : QSharedData(other),
   mPlainTextContent(other.mPlainTextContent),
   mHtmlContent(other.mHtmlContent),
   mComplete(other.mComplete),
   mAttachments(other.mAttachments),
   mEnvelope(other.mEnvelope)
{
    NM_FUNCTION;
}

/*!
    Class destructor.
*/
NmApiMessagePrivate::~NmApiMessagePrivate()
{
    NM_FUNCTION;
}

/*!
 \fn plainTextContent 
 \return Message plain text content.
 
 Returns message plain text content.
 */
NmApiTextContent NmApiMessagePrivate::plainTextContent()
{
    NM_FUNCTION;
    return mPlainTextContent;
}

/*!
 \fn htmlContent 
 \return Message html text content.
 
 Returns message html text content.
 */
NmApiTextContent NmApiMessagePrivate::htmlContent()
{
    NM_FUNCTION;
    return mHtmlContent;
}

/*!
 \fn isComplete 
 \return Message complete state.
 
 Returns message complete state.
 */
bool NmApiMessagePrivate::isComplete()
{
    NM_FUNCTION;
    return mComplete;
}

/*!
 \fn getAttachments 
 \return Message attachment list.
 
 Returns message attachment list.
 */
QList<NmApiAttachment> NmApiMessagePrivate::attachments()
{
    NM_FUNCTION;
    return mAttachments;
}


/*!
 \fn addAttachment 
 \param attachment Attachment.
 
 Add attachment to message.
 */
void NmApiMessagePrivate::addAttachment(NmApiAttachment &attachment)
{
    NM_FUNCTION;
    mAttachments.append(attachment);
}

/*!
 \fn envelope 
 \return Message envelope.
 
 Returns message envelope.
 */
NmApiMessageEnvelope& NmApiMessagePrivate::envelope()
{
    NM_FUNCTION;
    return mEnvelope;
}

/*!
 \fn setPlainTextContent 
 \param textContent Content.
 
 Setter for plain text content.
 */
void NmApiMessagePrivate::setPlainTextContent(NmApiTextContent &textContent)
{
    NM_FUNCTION;
    mPlainTextContent = textContent;
}

/*!
 \fn setHtmlContent 
 \param textContent Content.
 
 Setter for html text content.
 */
void NmApiMessagePrivate::setHtmlContent(NmApiTextContent &textContent)
{
    NM_FUNCTION;
    mHtmlContent = textContent;
}

/*!
 \fn setComplete 
 \param complete Complete state.
 
 Setter for message complete state.
 */
void NmApiMessagePrivate::setComplete(bool complete)
{
    NM_FUNCTION;
    mComplete = complete;
}

/*!
 \fn setAttachments 
 \param attachments Attachments.
 
 Setter for set attachments to message.
 */
void NmApiMessagePrivate::setAttachments(QList<NmApiAttachment> &attachments)
{
    NM_FUNCTION;
    mAttachments = attachments;
    
}

/*!
 \fn setEnvelope 
 \param envelope Envelope.
 
 Setter for message envelope.
 */
void NmApiMessagePrivate::setEnvelope(NmApiMessageEnvelope &envelope)
{
    NM_FUNCTION;
    mEnvelope = envelope;
}

}
