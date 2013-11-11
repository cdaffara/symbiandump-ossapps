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

#ifndef NMUIVIEWIDS_H_
#define NMUIVIEWIDS_H_

#include <qalgorithms.h> // For qDeleteAll().
#include <QStringList>
#include "nmcommon.h"
#include "nmaddress.h"


enum NmUiViewId
{
    NmUiViewNone = 0,          // Used to reset view stack
    NmUiViewMailboxList,       // Mailbox list view
    NmUiViewMessageList,       // Message list view of a folder
    NmUiViewMessageSearchList, // Message search list view
    NmUiViewMessageViewer,     // Message view of an email message
    NmUiViewMessageEditor      // Message editor
};

enum NmUiEditorStartMode
{
    NmUiEditorCreateNew = 0,
    NmUiEditorReply,
    NmUiEditorReplyAll,
    NmUiEditorForward,
    NmUiEditorMailto,
    NmUiEditorFromDrafts
};


/*!
	\class NmUiStartParam
	\brief NmUi start parameter class, contains general info about view.

	Use this class to start different views of the application.
    Mailbox list activation: Use default values.
    Message list activation: NmUiViewMessageList + mailboxId + folderId
    Message viewer activation: NmUiViewMessageViewer + mailboxId + folderId + messageId
    Message editor activation: NmUiViewMessageEditor + mailboxId + messageId + editorStartMode

    For new message to mailto addresses startup, use NmUiEditorMailto and set
    address list ownership of mailto address list is transferred to
    NmUiStartParam object
*/
class NmUiStartParam
{
public:

    inline explicit NmUiStartParam(NmUiViewId viewId = NmUiViewMailboxList,
        NmId mailboxId = 0,
        NmId folderId = 0,
        NmId messageId = 0,
        NmUiEditorStartMode editorStartMode = NmUiEditorCreateNew,
        QList<NmAddress*> *mailtoAddresses = NULL,
        QStringList *attachmentList = NULL,
        bool service = false,
        QString *subject = NULL,
        QList<NmAddress*> *ccAddressList = NULL,
        QList<NmAddress*> *bccAddressList = NULL,
        QString *bodyText = NULL,
        QString *bodyTextMimeType = NULL)
    {
        mViewId = viewId;
        mMailboxId = mailboxId;
        mFolderId = folderId;
        mMessageId = messageId;
        mEditorStartMode = editorStartMode;
        mMailtoAddressList = mailtoAddresses;
        mAttachmentList = attachmentList;
        mService = service;
        mSubject = subject;
        mCcAddressList = ccAddressList;
        mBccAddressList = bccAddressList;
        if (bodyText) {
            mBodyText = *bodyText;
		}
		if (bodyTextMimeType) {
		    mBodyTextMimeType = *bodyTextMimeType;
		}
    }

    inline ~NmUiStartParam()
    {
        if (mMailtoAddressList) {
            qDeleteAll(*mMailtoAddressList);
            delete mMailtoAddressList;
        }

        if (mAttachmentList) {
            delete mAttachmentList;
        }

        if (mSubject) {
            delete mSubject;
        }

        if (mCcAddressList) {
            qDeleteAll(*mCcAddressList);
            delete mCcAddressList;
        }

        if (mBccAddressList) {
            qDeleteAll(*mBccAddressList);
            delete mBccAddressList;
        }
    }

    inline NmUiViewId viewId()
    {
        return mViewId;
    }

    inline NmId mailboxId()
    {
        return mMailboxId;
    }

    inline NmId folderId()
    {
        return mFolderId;
    }

    inline NmId messageId()
    {
        return mMessageId;
    }

    inline NmUiEditorStartMode editorStartMode()
    {
        return mEditorStartMode;
    }

    inline QList<NmAddress*> &mailtoAddressList()
    {
        if (!mMailtoAddressList) {
            mMailtoAddressList = new QList<NmAddress*>();
        }

        return *mMailtoAddressList;
    }

    inline QStringList *attachmentList()
    {
        return mAttachmentList;
    }

    inline bool service()
    {
        return mService;
    }

    inline QString *subject()
    {
        return mSubject;
    }

    inline QList<NmAddress*> &ccAddressList()
    {
        if (!mCcAddressList) {
            mCcAddressList = new QList<NmAddress*>();
        }

        return *mCcAddressList;
    }

    inline QList<NmAddress*> &bccAddressList()
    {
        if (!mBccAddressList) {
            mBccAddressList = new QList<NmAddress*>();
        }

        return *mBccAddressList;
    }

    inline void setMailboxId(NmId mailboxId)
    {
        mMailboxId = mailboxId;
    }

    inline void setFolderId(NmId folderId)
    {
        mFolderId = folderId;
    }

    inline void setMessageId(NmId messageId)
    {
        mMessageId = messageId;
    }

    inline const QString &bodyText() const
    {
		return mBodyText;
	}

    inline const QString &bodyMimeType() const
    {
		return mBodyTextMimeType;
	}

private: // Data

    NmUiViewId mViewId;
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;
    NmUiEditorStartMode mEditorStartMode;
    QList<NmAddress*> *mMailtoAddressList; // owned
    QStringList *mAttachmentList; // owned
    bool mService;
    QString *mSubject; // owned
    QList<NmAddress*> *mCcAddressList; // owned
    QList<NmAddress*> *mBccAddressList; // owned
    QString mBodyText;
    QString mBodyTextMimeType;
};


#endif /* NMUIVIEWIDS_H_ */

// End of file.
