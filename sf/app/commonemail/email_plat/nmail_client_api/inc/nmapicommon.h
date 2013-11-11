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

#ifndef NMAPICOMMON_H_
#define NMAPICOMMON_H_

#include <QMetaType>

namespace EmailClientApi
{
    /*!
       Enum to describe mailbox events
     */
    enum NmApiMailboxEvent
    {
        MailboxCreated, MailboxDeleted
    };

    /*!
        Email to message priority
     */
    enum NmApiMessagePriority
    {
        NmApiMessagePriorityLow,
        NmApiMessagePriorityNormal,
        NmApiMessagePriorityHigh
    };

    /*!
        Enum to message flags
     */
    enum NmApiMessageFlag 
    {
        NmApiMessageFlagRead = 1,
        NmApiMessageFlagReadLocally = 2,
        NmApiMessageFlagLow = 4,
        NmApiMessageFlagImportant = 8,
        NmApiMessageFlagFollowUpComplete = 16,
        NmApiMessageFlagFollowUp = 32,
        NmApiMessageFlagAttachments = 64,
        NmApiMessageFlagMultiple = 128,
        NmApiMessageFlagCalendarMsg = 256,
        NmApiMessageFlagAnswered = 512,
        NmApiMessageFlagForwarded = 1024,
        NmApiMessageFlagOnlyToMe = 2048,
        NmApiMessageFlagRemoteDeleted = 4096,
        NmApiMessageFlagHasMsgSender = 8192
    };
    Q_DECLARE_FLAGS(NmApiMessageFlags, NmApiMessageFlag)
    Q_DECLARE_OPERATORS_FOR_FLAGS(NmApiMessageFlags)

    /*!
       Enum to describe message events.
     */
    enum NmApiMessageEvent 
    {
        MessageCreated,
        MessageDeleted,
        MessageChanged
    };
    
    /*!
       Enum to describe type of email folder 
     */
    enum NmApiEmailFolderType
    {
        Inbox, Drafts, Outbox, Sent, Deleted, EOther
    };

    /*!
        Email list sorting options
     */
    enum NmApiMailSortField
        {
        MailDontCare,            // user accepts any sort order
        MailSortByDate,          // sorting is done by date
        MailSortBySender,        // sorting is done by sender
        MailSortByRecipient,     // sorting is done by recipients
        MailSortBySubject,       // sorting is done by subject
        MailSortByPriority,      // sorting is done by priority
        MailSortByFlagStatus,    // follow up and completed   
        MailSortByUnread,        // sorting is based on unread
        MailSortBySize,          // sorting is done by size
        MailSortByAttachment     // sorting is based on if message has attachments
        };

    /*!
        Email list sort order
     */
    enum NmApiMailSortOrder
        {
        MailDescending,
        MailAscending    
        };


    /*!
        List of message related operations status
     */
    enum NmApiOperationStatus 
    {
        NotStarted,
        Pending,
        Succeeded,
        Failed,
        Canceled
    };
    
    /*!
        List of message related operations
     */
    enum NmApiOperationType
    {
        CreateDraftMessage,
        CreateForwardMessage,
        CreateReplyMessage,
        MoveMessages,
        SaveMessage,
        DeleteMessages,
        FetchMessage,
        SendMessage,
        CreateAttachment,
        RemoveAttachment,
        FetchAttachment
    };    
    
    
    /*!
        Sort criteria definion Class for searching email messages
     */
    class NmApiMailSortCriteria 
        {
        public:
            NmApiMailSortField iField;
            NmApiMailSortOrder iOrder;
        };
    
    /*!
        NmApi error codes
    */
    const int NmApiNoError = 0;
    const int NmApiNotFoundError = -1;
    const int NmApiGeneralError = -2;
    const int NmApiCancelError = -3;
    
    /*!
        NmApi predefined constants for ContentTypes
    */
    const QString NmApiContentTypeMultipartMixed = "multipart/mixed";
    const QString NmApiContentTypeMultipartAlternative = "multipart/alternative";
    const QString NmApiContentTypeMultipartDigest = "multipart/digest";
    const QString NmApiContentTypeMultipartParallel = "multipart/parallel";
    const QString NmApiContentTypeMultipartRelated =  "multipart/related";
    const QString NmApiContentTypeTextPlain = "text/plain";
    const QString NmApiContentTypeTextHtml = "text/html";
    const QString NmApiContentTypeTypeMessage = "message/rfc822";
    const QString NmApiContentTypeParamName = " name=";
    const QString NmApiContentTypeParamCharset = " charset=";
}
Q_DECLARE_METATYPE (EmailClientApi::NmApiMailboxEvent)
Q_DECLARE_METATYPE(EmailClientApi::NmApiMessageEvent)
Q_DECLARE_METATYPE (EmailClientApi::NmApiEmailFolderType)

#endif /* NMAPICOMMON_H_ */
