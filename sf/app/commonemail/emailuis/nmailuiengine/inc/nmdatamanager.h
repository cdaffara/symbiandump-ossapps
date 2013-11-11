/*
* Copyright (c) 2009-10 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMDATAMANAGER_H_
#define NMDATAMANAGER_H_

#include "nmuienginedef.h"
#include "nmcommon.h"

class NmMailbox;
class NmMessage;
class NmMessagePart;
class NmFolder;
class NmMailboxMetaData;
class NmFolderMetaData;
class NmMessageEnvelope;
class NmDataPluginFactory;


class NMUIENGINE_EXPORT NmDataManager: public QObject
{
    Q_OBJECT
public:
    NmDataManager();
    virtual ~NmDataManager();

    NmMailboxMetaData *mailboxById(const NmId &mailboxId);
    NmFolderMetaData *folderById(const NmId &mailboxId, const NmId &folderId);
    NmMessageEnvelope *envelopeById(const NmId &mailboxId,
                                    const NmId &folderId,
                                    const NmId &messageId);
    NmMailbox *mailbox(const NmId &mailboxId);
    NmFolder *folder(const NmId &mailboxId,
                     const NmId &folderId);
    NmMessage *message(const NmId &mailboxId,
                       const NmId &folderId,
                       const NmId &messageId);
    
    void listMailboxIds(QList<NmId> &mailboxIdList);
    void listMailboxes(QList<NmMailbox*> &mailboxList);
    void listFolders(const NmId mailboxId,
                     QList<NmFolder*> &folderList);
    void listMessages(const NmId &mailboxId,
                      const NmId &folderId,
                      QList<NmMessageEnvelope*> &messageEnvelopeList);

    int contentToMessagePart(const NmId &mailboxId,
                             const NmId &folderId,
                             const NmId &messageId,
                             NmMessagePart &messagePart);
    NmId getStandardFolderId(const NmId &mailboxId,
                             NmFolderType folderType);
    
    NmFolderType folderTypeById(NmId mailboxId, NmId folderId);

private:
    NmDataPluginFactory   *mFactory;
};

#endif /* NMDATAMANAGER_H_ */
