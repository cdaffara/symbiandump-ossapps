/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */

#include "nmapiheaders.h"

/*!
   converts nmmailbox to client api NmApiMailbox
 */
EmailClientApi::NmApiMailbox NmToApiConverter::NmMailbox2NmApiMailbox(const NmMailbox &mailbox)
{
    NM_FUNCTION;
    
    EmailClientApi::NmApiMailbox api_mailbox;
    api_mailbox.setId(mailbox.id().id());
    api_mailbox.setName(mailbox.name());
    NmMailbox tmp(mailbox);
    api_mailbox.setAddress(tmp.address().address());
    return api_mailbox;
}

/*!
   converts NmFolder to client api NmFolder
 */
EmailClientApi::NmApiFolder NmToApiConverter::NmFolder2NmApiFolder(const NmFolder &folder)
{
    NM_FUNCTION;
    
    EmailClientApi::NmApiFolder api_folder;

    api_folder.setParentFolderId(folder.parentId().id());
    api_folder.setId(folder.folderId().id());
    api_folder.setName(folder.name());
    api_folder.setFolderType((EmailClientApi::NmApiEmailFolderType) folder.folderType());
    return api_folder;
}

/*!
   converts NmMessageEnvelope to client api NmApiMessageEnvelope
 */
EmailClientApi::NmApiMessageEnvelope NmToApiConverter::NmMessageEnvelope2NmApiMessageEnvelope(
    const NmMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    EmailClientApi::NmApiMessageEnvelope api_env;
    
    QList<NmAddress> to = envelope.toRecipients();
    QList<NmAddress> cc = envelope.ccRecipients();
    QList<NmAddress> bcc = envelope.bccRecipients();
    QList<EmailClientApi::NmApiEmailAddress> to_api = NmAddress2QString(to);
    QList<EmailClientApi::NmApiEmailAddress> cc_api = NmAddress2QString(cc);
    QList<EmailClientApi::NmApiEmailAddress> bcc_api = NmAddress2QString(bcc);

    api_env.setToRecipients(to_api);
    api_env.setCcRecipients(cc_api);
    api_env.setBccRecipients(bcc_api);

    api_env.setHasAttachments(envelope.hasAttachments());
    api_env.setId(envelope.messageId().id());
    api_env.setIsForwarded(envelope.isForwarded());
    api_env.setIsRead(envelope.isRead());
    api_env.setIsReplied(envelope.isReplied());
    api_env.setParentFolder(envelope.folderId().id());

    api_env.setSender(envelope.sender().address());
    api_env.setSentTime(envelope.sentTime());
    api_env.setSubject(envelope.subject());
    NmId mailboxId(envelope.mailboxId());
    api_env.setMailboxId(mailboxId.id());
    api_env.setFlags(static_cast<EmailClientApi::NmApiMessageFlags>(static_cast<int>(envelope.flags())), true);

    return api_env;
}

/*!
   converts QList of NmAddresses to Qlist of email client apis NmApiEmailAddresses
 */
QList<EmailClientApi::NmApiEmailAddress> NmToApiConverter::NmAddress2QString(
    const QList<NmAddress> &addresses)
{
    NM_FUNCTION;
    
    QList<EmailClientApi::NmApiEmailAddress> nmAddresses;
    for (int i = 0; i < addresses.count(); i++) {
        EmailClientApi::NmApiEmailAddress addr;
        addr.setAddress(addresses[i].address());
        addr.setDisplayName(addresses[i].displayName());

        nmAddresses << addr;
    }

    return nmAddresses;
}

/*!
   converts nmmessage to client api NmApiMessage
 */
EmailClientApi::NmApiMessage NmToApiConverter::NmMessage2NmApiMessage(NmMessage &message)
{
    NM_FUNCTION;
    
    EmailClientApi::NmApiMessage api_message;
    QList<NmMessagePart*> attachments;
    message.attachmentList(attachments);

    if (attachments.size() > 0){
        QList<EmailClientApi::NmApiAttachment> api_attachments; 
        for (int i = 0; i < attachments.size(); i++){
            NmMessagePart *part = attachments.at(i);
            if (part->contentType().compare(EmailClientApi::NmApiContentTypeTextPlain) != 0 && 
                part->contentType().compare(EmailClientApi::NmApiContentTypeTextHtml) != 0) {
                EmailClientApi::NmApiAttachment api_attachment;
                api_attachment.setFileName(part->attachmentName());
                api_attachment.setId(part->partId().id());
                api_attachment.setContentType(part->contentType());
                api_attachment.setSize(part->size());
                api_attachments.append(api_attachment);
            }
        }
        api_message.setAttachments(api_attachments);
    }
    
    EmailClientApi::NmApiMessageEnvelope api_envelope = NmMessageEnvelope2NmApiMessageEnvelope(message.envelope());
    api_message.setEnvelope(api_envelope);
    const NmMessagePart *plaintext = message.plainTextBodyPart();
    if (plaintext) {
        EmailClientApi::NmApiTextContent api_plaintext;
        api_plaintext.setContent(plaintext->textContent());
        api_plaintext.setId(plaintext->partId().id());
        api_plaintext.setContentType(plaintext->contentType());
        api_plaintext.setSize(plaintext->size());
        api_message.setPlainTextContent(api_plaintext);
    }
    
    const NmMessagePart *htmlcontent = message.htmlBodyPart();
    if (htmlcontent) {
        EmailClientApi::NmApiTextContent api_htmlcontent;
        api_htmlcontent.setContent(htmlcontent->textContent());
        api_htmlcontent.setId(htmlcontent->partId().id());
        api_htmlcontent.setContentType(htmlcontent->contentType());
        api_htmlcontent.setSize(htmlcontent->size());
        api_message.setHtmlContent(api_htmlcontent);
    }
    
    return api_message;
}
