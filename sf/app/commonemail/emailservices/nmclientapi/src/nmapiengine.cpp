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

NmApiEngine *NmApiEngine::mInstance = NULL;
quint32 NmApiEngine::mReferenceCount = 0;

/*!
   \class NmApiEngine
   \brief The NmEngine class provides access to data for
   mailboxes, folders and messages.
 */

/*!
   Private constructor
 */
NmApiEngine::NmApiEngine() :
    mFactory(NULL)
{
    NM_FUNCTION;
    
    mFactory = NmApiDataPluginFactory::instance();
}

/*!
  Destructor
 */
NmApiEngine::~NmApiEngine()
{
    NM_FUNCTION;
    
    NmApiDataPluginFactory::releaseInstance(mFactory);
    
    if (mMailPlugins.Count() > 0) {
        CFSMailPlugin* plugin = NULL;
        for (int i = 0; i < mMailPlugins.Count(); ++i) {
            plugin = mMailPlugins[i];
            
            if (plugin) {
                delete plugin;
                plugin = NULL;
            }
            
            CFSMailPlugin::Close();
        }
    }
}

/*!
   Creates instance of NmApiEngine and provide pointer to it.
   
   If engine exist, it increase references count and provide pointer to it.
   
   \return Instance of engine
 */
NmApiEngine *NmApiEngine::instance()
{
    NM_FUNCTION;
    
    if (!mInstance) {
        mInstance = new NmApiEngine();
    }
    mReferenceCount++;
    return mInstance;
}

/*!
   Creates connections for events from email store.
 */
void NmApiEngine::startCollectingEvents()
{
    NM_FUNCTION;
    
    QObject *plugin = mFactory->plugin();
    if(plugin){
        connect(plugin, SIGNAL(messageEvent(NmMessageEvent, NmId, QList<NmId> , NmId)), this,
            SLOT(messageChangedArrived(NmMessageEvent, NmId, QList<NmId> , NmId)), Qt::UniqueConnection);

        connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, QList<NmId> )), this, SLOT(
            mailboxChangedArrived(NmMailboxEvent, QList<NmId> )), Qt::UniqueConnection);
    }
}

/*!
   It process mailbox ids from email store for given event.
   On end it emit \sa emailStoreEvent
   
   \arg Event of mailbox change
   \arg List of mailbox ids for that event 
 */
void NmApiEngine::mailboxChangedArrived(NmMailboxEvent mailboxEvent, const QList<NmId> &mailboxIds)
{
    NM_FUNCTION;
    
    NmApiEvent event;
    event.objectType = EMailbox;
    switch (mailboxEvent) {
        case NmMailboxCreated: {
            event.action = ENew;
            // subscribe all events also for these new mailboxes
            for(int i=0; i<mailboxIds.count(); i++) {
                mFactory->interfaceInstance()->subscribeMailboxEvents(mailboxIds[i]);
            }
        }
            break;
        case NmMailboxDeleted: {
            event.action = EDeleted;
            // unsubscribe all events from deleted mailboxes
            for(int i=0; i<mailboxIds.count(); i++) {
                mFactory->interfaceInstance()->unsubscribeMailboxEvents(mailboxIds[i]);
            }
        }
            break;
        case NmMailboxChanged: {
            event.action = EChange;
        }
            break;
        default:
            break;
    }

    event.folderId = 0;
    event.mailboxId = 0;

    for (int i = 0; i < mailboxIds.count(); i++) {
        event.objectIds.append(mailboxIds.at(i).id());
    }

    emit emailStoreEvent(event);
}

/*!
   It process message ids from email store for given event.
   On end it emit \sa emailStoreEvent
   
   \param messageEvent Event of message change
   \param folderId Id of folder from where are messages
   \param messageIds List of message ids for that event 
   \param mailboxId Id of mailbox from where are messages
 */
void NmApiEngine::messageChangedArrived(
    NmMessageEvent messageEvent,
    const NmId &folderId,
    const QList<NmId> &messageIds,
    const NmId &mailboxId)
{
    NM_FUNCTION;
    
    NmApiEvent event;
    event.objectType = EMessage;
    switch (messageEvent) {
        case NmMessageCreated: {
            event.action = ENew;
        }
            break;
        case NmMessageDeleted: {
            event.action = EDeleted;
        }
            break;
        case NmMessageChanged: {
            event.action = EChange;
        }
            break;
        default:
            break;
    }

    event.folderId = folderId.id();
    event.mailboxId = mailboxId.id();

    for (int i = 0; i < messageIds.count(); i++) {
        event.objectIds.append(messageIds.at(i).id());
    }

    emit emailStoreEvent(event);
}

/*!
   Release instance of engine if there are not referenced objects. 
   If there are refenced objects it only decrease refenced count.
   
   \param instance Instance of engine
 */
void NmApiEngine::releaseInstance(NmApiEngine *&instance)
{
    NM_FUNCTION;
    
    //can't have passed out instances if we don't have any
    if (mInstance) {
        if (instance == mInstance) {
            instance = NULL;
            mReferenceCount--;
        }
        if (0 >= mReferenceCount) {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

/*!
      It get all mailboxes from email store
      
      \sa EmailClientApi::NmMailbox
      \param mailboxList List of mailboxes to be filled.
 */
void NmApiEngine::listMailboxes(QList<EmailClientApi::NmApiMailbox> &mailboxList)
{
    NM_FUNCTION;
    
    QList<NmMailbox*> mailboxFromPlugin;

    NmDataPluginInterface *instance =  mFactory->interfaceInstance();
    if (instance) {
        instance->listMailboxes(mailboxFromPlugin);
        
        while (mailboxFromPlugin.isEmpty() == false) {
            NmMailbox* tempNmMailbox = mailboxFromPlugin.takeLast();
    
            // subscribe all events also for these new mailboxes
            instance->subscribeMailboxEvents(tempNmMailbox->id());
    
            // construct mailboxlist to platform api
            mailboxList << NmToApiConverter::NmMailbox2NmApiMailbox(*tempNmMailbox);
    
            delete tempNmMailbox;
        }
    }
}

/*!
      It get all folders from email store for given mailbox
      
      \sa EmailClientApi::NmApiFolder
      \param mailboxId Mailbox id from where folders should be returned
      \param folderList  of folders to be filled.
 */
void NmApiEngine::listFolders(const quint64 mailboxId, QList<EmailClientApi::NmApiFolder> &folderList)
{
    NM_FUNCTION;
    
    QList<NmFolder*> folderFromPlugin;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        instance->listFolders(mailboxId, folderFromPlugin);
    }

    while (folderFromPlugin.isEmpty() == false) {
        NmFolder* tempNmFolder = folderFromPlugin.takeLast();
        folderList << NmToApiConverter::NmFolder2NmApiFolder(*tempNmFolder);
        delete tempNmFolder;
    }
}

/*!
      It get all envelopes from email store for given mailbox and folder
      
      \sa EmailClientApi::NmApiMessageEnvelope
      \param mailboxId Mailbox id from where envelope should be returned
      \param folderId Folder id from where envelope should be returned
      \param messageEnvelopeList List of envelopes to be filled.
 */
void NmApiEngine::listEnvelopes(const quint64 mailboxId, const quint64 folderId, 
                    QList<EmailClientApi::NmApiMessageEnvelope> &messageEnvelopeList)
{
    NM_FUNCTION;
    
    QList<NmMessage*> messages;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        instance->listMessages(mailboxId, folderId, messages);
    }

    while (!messages.isEmpty()) {
        NmMessage* message = messages.takeFirst();
            
        EmailClientApi::NmApiMessageEnvelope nmEnvelope =
            NmToApiConverter::NmMessageEnvelope2NmApiMessageEnvelope(message->envelope());
        
        NmMessagePart *plainTextPart = message->plainTextBodyPart();
        
        QString plainText = QString();
        if (plainTextPart) {
            plainText = plainTextPart->textContent();
        }

        nmEnvelope.setPlainText(plainText);
        nmEnvelope.setFetchedSize(message->fetchedSize());
        nmEnvelope.setTotalSize(message->size());

        messageEnvelopeList << nmEnvelope;
        delete message;
    }
}

/*!
   Return envelope given by mailbox, folder and envelope id.
   
   \param mailboxId Mailbox id from where envlope should come
   \param folderId Folder id from where envlope should come
   \param messageId Id of envelope which should be returned
   \param envelope Envelope to fill.
   
   \return Return true if it will find any envelope
 */
bool NmApiEngine::getEnvelopeById(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 envelopeId,
    EmailClientApi::NmApiMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    //flag indicating that envelope with given id was found
    bool found = false;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        NmMessage *message(NULL);
        instance->getMessageById(mailboxId, folderId, envelopeId, message);
        if(message){
            NmMessageEnvelope env = message->envelope();

            envelope = NmToApiConverter::NmMessageEnvelope2NmApiMessageEnvelope(env);
            QString plainText = QString();
            
            NmMessagePart *plainTextPart = message->plainTextBodyPart();
            if (plainTextPart) {
                instance->contentToMessagePart(mailboxId, folderId, envelopeId, *plainTextPart);
                plainText = plainTextPart->textContent();
            }

            envelope.setPlainText(plainText);
            envelope.setFetchedSize(message->fetchedSize());
            envelope.setTotalSize(message->size());

            found = true;
        }
        delete message;
    }
    return found;
}

/*!
   Return folder given by mailbox and folder id.
   
   \param mailboxId Id of Mailbox which should be returned
   \param folderId Folder id id of folder to be fetched
   \param mailboxFolder reference to copy reference of NmApiFolder
   
   \return Return true if it will find folder
 */
bool NmApiEngine::getFolderById(
    const quint64 mailboxId,
    const quint64 folderId,
    EmailClientApi::NmApiFolder &mailboxFolder)
{
    NM_FUNCTION;

    bool found = false;

    listMailPlugins();

    CFSMailPlugin *plugin = NULL;
    CFSMailFolder *fsFolder = NULL;
    TFSMailMsgId fsMailBoxId = TFSMailMsgId(mailboxId);
    TFSMailMsgId fsFolderId = TFSMailMsgId(folderId);

    for (int i = 0; i < mMailPlugins.Count() && !fsFolder; i++){
        plugin = mMailPlugins[i];
        if (plugin && (plugin->Id() == fsMailBoxId.PluginId())) {
            TRAPD(err, fsFolder = plugin->GetFolderByUidL(fsMailBoxId, fsFolderId));
            Q_UNUSED(err);
        }
    }
    if (fsFolder) {
        NmFolder *nmFolder = fsFolder->GetNmFolder();
        EmailClientApi::NmApiFolder apiFolder = NmToApiConverter::NmFolder2NmApiFolder(*nmFolder);
        mailboxFolder = apiFolder;
        found = true;
        delete nmFolder;
        delete fsFolder;
    }
    return found;
}

/*!
   Return message given by mailbox, folder and message id.
   
   \param mailboxId Mailbox id from where message should come
   \param folderId Folder id from where message should come
   \param messageId Id of message which should be returned
   \param message Envelope to fill.
   
   \return Return true if it will find any envelope
 */
bool NmApiEngine::getMessageById(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 messageId,
    EmailClientApi::NmApiMessage &message)
{
    NM_FUNCTION;
    
    bool found = false;
    
    listMailPlugins();

    CFSMailPlugin *plugin = NULL;
    CFSMailMessage *fsMessage = NULL;
    TFSMailMsgId fsMailBoxId = TFSMailMsgId(mailboxId);
    TFSMailMsgId fsFolderId = TFSMailMsgId(folderId);
    TFSMailMsgId fsMessageId = TFSMailMsgId(messageId);
    
    for (int i = 0; i < mMailPlugins.Count() && !fsMessage; i++){
        plugin = mMailPlugins[i];
        if (plugin && (plugin->Id() == fsMailBoxId.PluginId())) {
            TRAPD(err, fsMessage = plugin->GetMessageByUidL(fsMailBoxId,
                    fsFolderId,
                    fsMessageId,
                    EFSMsgDataStructure));
            Q_UNUSED(err);
        }
    }
    if (fsMessage) {
        NmMessage *nmMessage = fsMessage->GetNmMessage();
        EmailClientApi::NmApiMessage apiMessage = NmToApiConverter::NmMessage2NmApiMessage(*nmMessage);
        message = apiMessage;
        found = true;
        delete nmMessage;
        delete fsMessage;
    }
    return found;
}


/*!
   Return mailbox given by mailbox id.
   
   \param mailboxId Id of Mailbox which should be returned
   \param mailbox Mailbox to fill.
   
   \return Return true if it will find any envelope
 */
bool NmApiEngine::getMailboxById(const quint64 mailboxId, EmailClientApi::NmApiMailbox &mailbox)
{
    NM_FUNCTION;
    
    //flag indicating that mailbox with given id was found
    bool found = false;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();

    if (instance) {    
        NmMailbox *nmmailbox = NULL;
        int err =instance->getMailboxById(NmId(mailboxId), nmmailbox);
    
        if (err == NmNoError && nmmailbox) {
            mailbox = NmToApiConverter::NmMailbox2NmApiMailbox(*nmmailbox);
            found = true;            
        }
        delete nmmailbox;
    }
    
    return found;
}

/*!
 \fn listMailPlugins 
 \param mailPlugins List of mail plugins to be listed.
 \return True if operation completed succesfully, otherwise false.
 
 Lists all mail plugins.  
 */
bool NmApiEngine::listMailPlugins(RPointerArray<CFSMailPlugin> &mailPlugins)
{
    NM_FUNCTION;
    bool ret = false;
    if (mMailPlugins.Count() == 0) {
        TRAPD(err, listMailPluginsL());
        if (err == KErrNone){
            ret = true;
        }
    } else {
        ret = true;
    }
    mailPlugins = mMailPlugins;
    
    return ret;
}

/*!
 \fn listMailPlugins 
 
 Lists all mail plugins.  
 */
void NmApiEngine::listMailPlugins()
{
    NM_FUNCTION;
    if (mMailPlugins.Count() == 0) {
        TRAP_IGNORE(listMailPluginsL());
    }
}

/*!
 \fn ListMailPluginsL
 
 Leaving version of listMailPlugins.
 */
void NmApiEngine::listMailPluginsL()
{
    NM_FUNCTION;

    RPointerArray<CImplementationInformation> implInfo;
    CFSMailPlugin::ListImplementationsL(implInfo);
    CleanupClosePushL(implInfo);    
    CFSMailPlugin* plugin = NULL;
    for (int i = 0; i < implInfo.Count(); ++i) {
        TUid id = implInfo[i]->ImplementationUid();
        plugin = CFSMailPlugin::NewL(id);
        
        if (plugin) {
            mMailPlugins.AppendL(plugin);
        }
    }
    CleanupStack::Pop(&implInfo);
}

