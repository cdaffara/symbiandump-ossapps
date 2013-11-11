/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMMESSAGELISTMODEL_H_
#define NMMESSAGELISTMODEL_H_

#include <QStandardItemModel>

#include "nmcommon.h"
#include "nmmessagelistmodelitem.h"
#include "nmuienginedef.h"
#include "nmuiviewids.h"

class NmMessage;
class NmDataManager;
class QModelIndex;
class NmMessageEnvelope;


class NMUIENGINE_EXPORT NmMessageListModel : public QStandardItemModel
{
    Q_OBJECT

public:

    NmMessageListModel(NmDataManager &dataManager,
                       QObject *parent = 0);

    virtual ~NmMessageListModel();

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;

    void refresh(const NmId mailboxId,
                 const NmId folderId,
                 const QList<NmMessageEnvelope*> &messageEnvelopeList);

    bool dividersActive();

    // This function is temporary, to be removed when divider state can be
    // modified with settings.
    void setDividers(bool active);

    void setEnvelopeProperties(NmEnvelopeProperties property,
                               const QList<NmId> &messageIds);

    NmId currentMailboxId();

    void setIgnoreFolderIds(bool ignore);

    void handleFolderDeletedEvent(
                            const NmId &folderId,
                            const NmId &mailboxId);   


public slots:

    void handleMessageEvent(NmMessageEvent event,
                            const NmId &folderId,
                            const QList<NmId> &messageIds,
                            const NmId &mailboxId);


private:

    bool messagesBelongUnderSameDivider(
        const NmMessageEnvelope *message1,
        const NmMessageEnvelope *message2) const;

    void insertDividerIntoModel(
        NmMessageEnvelope *messageForDivider,
        int parentRow);

    void insertMessageIntoModel(
        NmMessageEnvelope *messageEnvelope,
        int childRow,
        bool emitSignal);

    void insertNewMessageIntoModel(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &msgId);

    int getInsertionIndex(const NmMessageEnvelope &envelope) const;

    int dividerInsertionIndex(int messageIndex);

    NmMessageListModelItem *createTitleDividerItem(NmMessageEnvelope *messageForDivider);

    NmMessageListModelItem *createMessageItem(NmMessageEnvelope *message);

    void removeMessageFromModel(const NmId &msgId);

    void removeItem(int row, NmMessageListModelItem &item);

    NmMessageListModelItem *itemFromModel(const NmId &messageId);

    bool changed(const NmMessageEnvelope &first,
                 const NmMessageEnvelope &second);

    void updateMessageEnvelope(const NmId &mailboxId,
                               const NmId &folderId,
                               const NmId &msgId);

    void updateEnvelope(NmEnvelopeProperties property, const NmId &msgId);


signals:

    void setNewParam(NmUiStartParam *startParam);


private: // Data

    NmDataManager &mDataManager;
    bool mDividersActive;
    NmMessageListModelItem *mParentPtr; // Not owned
    NmId mCurrentMailboxId;
    NmId mCurrentFolderId;
    NmFolderType mCurrentFolderType;
    bool mIgnoreFolderIds;
};

#endif /* NMMESSAGELISTMODEL_H_*/
