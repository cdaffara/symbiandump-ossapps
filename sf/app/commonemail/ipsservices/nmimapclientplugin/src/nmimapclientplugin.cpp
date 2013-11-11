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

#include "nmimapclientpluginheaders.h"
#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#endif

Q_EXPORT_PLUGIN(NmImapClientPlugin)


/*!
 \class NmImapClientPlugin

 \brief The NmImapClientPlugin plugin provides ui services for IMAP mail protocol.
*/

/*!
    Constructs a new NmImapClientPlugin.
 */
NmImapClientPlugin::NmImapClientPlugin()
: mListOptionsMenuRequest(NULL),
mSettingsViewLauncher(NULL)
{
}

/*!
    Destructor.
 */
NmImapClientPlugin::~NmImapClientPlugin()
{
    delete mSettingsViewLauncher;
}

/*!
    Provides list of supported NmActions.
    Implementation of NmUiExtensionInterface.
    Parameter \a request controls list of request services.
    Parameter \a actionList is updated by supported NmActions.
 */
void NmImapClientPlugin::getActions(
      const NmActionRequest &request,
      QList<NmAction *> &actionList)
{
    NMLOG("NmImapClientPlugin::getActions()-->");

    // Check if this request is for the IMAP protocol.
    quint32 pluginId = this->pluginId();
    if (request.mailboxId().pluginId32() != pluginId
        #ifdef USE_POPIMAP_TESTPLUGIN
        && request.mailboxId().pluginId32() != IMAPTESTPLUGINID
        #endif
    ) {
        NMLOG("NmImapClientPlugin::getActions(): PluginId incorrect.");
        NMLOG(QString("request.mailboxId().pluginId32()=%1").arg(request.mailboxId().pluginId32()));
        return;
    }

    // Get the NmBaseClientPlugin implementation of the UI actions.
    NmBaseClientPlugin::getActions(request, actionList);

    // Append IMAP plugin specific actions
    switch (request.menuType()) {
    case NmActionOptionsMenu:
        {
        if (request.contextView()==NmActionContextViewMessageList){
            mListOptionsMenuRequest = request;
            // Create sub-menu for standard folders selection
            HbMenu *standardFolders = new HbMenu();
            NmAction *inboxAction = new NmAction(0);
            inboxAction->setObjectName("imapclientplugin_folders_inbox");
            inboxAction->setText(hbTrId("txt_mail_opt_folder_sub_inbox"));
            inboxAction->setCheckable(true);
            connect(inboxAction, SIGNAL(triggered()), this, SLOT(switchToStandardFolderInbox()));
            standardFolders->addAction(inboxAction);
            NmAction *outboxAction = new NmAction(0);
            outboxAction->setObjectName("imapclientplugin_folders_outbox");
            outboxAction->setText(hbTrId("txt_mail_opt_folder_sub_outbox"));
            outboxAction->setCheckable(true);
            connect(outboxAction, SIGNAL(triggered()), this, SLOT(switchToStandardFolderOutbox()));
            standardFolders->addAction(outboxAction);
            NmAction *draftsAction = new NmAction(0);
            draftsAction->setObjectName("imapclientplugin_folders_drafts");
            draftsAction->setText(hbTrId("txt_mail_opt_folder_sub_drafts"));
            draftsAction->setCheckable(true);
            connect(draftsAction, SIGNAL(triggered()), this, SLOT(switchToStandardFolderDrafts()));
            standardFolders->addAction(draftsAction);
            NmAction *sentAction = new NmAction(0);
            sentAction->setObjectName("imapclientplugin_folders_sent");
            sentAction->setText(hbTrId("txt_mail_opt_folder_sub_sent"));
            sentAction->setCheckable(true);
            connect(sentAction, SIGNAL(triggered()), this, SLOT(switchToStandardFolderSent()));
            standardFolders->addAction(sentAction);
            // Set active action
            NmFolderType curFolderType(NmFolderOther);
            if (mUiEngine) {
                curFolderType = mUiEngine->folderTypeById(request.mailboxId(),request.folderId());
            }
            switch (curFolderType){
                case NmFolderInbox: {
                    inboxAction->setChecked(true);
                    break;
                }
                case NmFolderOutbox: {
                    outboxAction->setChecked(true);
                    break;
                }
                case NmFolderDrafts: {
                    draftsAction->setChecked(true);
                    break;
                }
                case NmFolderSent: {
                    sentAction->setChecked(true);
                    break;
                }
            }

            // Add folders selection with sub-menu
            NmAction *foldersAction = new NmAction(0);
            foldersAction->setObjectName("baseclientplugin_folders");
            foldersAction->setText(hbTrId("txt_mail_opt_change_folder"));
            foldersAction->setMenu(standardFolders);
            actionList.append(foldersAction);
            }
        }
        break;
    default:
        break;
    }

    NMLOG("<--NmImapClientPlugin::getActions()");
}

/*!
    Slot connected to options menu settings NmAction.
    Opens mailbox settings.
 */
void NmImapClientPlugin::settings()
{
    NMLOG("NmImapClientPlugin::settings()-->");

    const NmId &id = mMenuRequest.mailboxId();
    NmMailboxMetaData *mailbox = mUiEngine->mailboxById(id);

    if (mailbox) {
        if (!mSettingsViewLauncher) {
            mSettingsViewLauncher = new NmSettingsViewLauncher();

            connect(mSettingsViewLauncher,
                SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                this, SLOT(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

            connect(mSettingsViewLauncher,
                SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                this, SLOT(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));

            connect(mSettingsViewLauncher,
                SIGNAL(goOnline(const NmId &)),
                this, SLOT(goOnline(const NmId &)));

            connect(mSettingsViewLauncher,
                SIGNAL(goOffline(const NmId &)),
                this, SLOT(goOffline(const NmId &)));
        }
        handleRequest(NmActionResponseCommandSettings, mMenuRequest);
        mSettingsViewLauncher->launchSettingsView(id, mailbox->name());
    }

    NMLOG("<--NmImapClientPlugin::settings()");
}

/*!
    Private slot to switch message list contents
    to standard folder inbox
*/
void NmImapClientPlugin::switchToStandardFolderInbox()
{
    if (mListOptionsMenuRequest.observer()){
        NmId folderId=mUiEngine->standardFolderId(mListOptionsMenuRequest.mailboxId(),NmFolderInbox);
        NmActionResponse response(NmActionResponseCommandSwitchFolder, NmActionOptionsMenu,
                                  NmActionContextViewNone, mListOptionsMenuRequest.mailboxId(),
                                  folderId);
        mListOptionsMenuRequest.observer()->handleActionCommand(response);
    }
}

/*!
    Private slot to switch message list contents
    to standard folder outbox
*/
void NmImapClientPlugin::switchToStandardFolderOutbox()
{
    if (mListOptionsMenuRequest.observer()){
        NmId folderId=mUiEngine->standardFolderId(mListOptionsMenuRequest.mailboxId(),NmFolderOutbox);
        NmActionResponse response(NmActionResponseCommandSwitchFolder, NmActionOptionsMenu,
                                  NmActionContextViewNone, mListOptionsMenuRequest.mailboxId(),
                                  folderId);
        mListOptionsMenuRequest.observer()->handleActionCommand(response);
    }
}

/*!
    Private slot to switch message list contents
    to standard folder drafts
*/
void NmImapClientPlugin::switchToStandardFolderDrafts()
{
    if (mListOptionsMenuRequest.observer()){
        NmId folderId=mUiEngine->standardFolderId(mListOptionsMenuRequest.mailboxId(),NmFolderDrafts);
        NmActionResponse response(NmActionResponseCommandSwitchFolder, NmActionOptionsMenu,
                                  NmActionContextViewNone, mListOptionsMenuRequest.mailboxId(),
                                  folderId);
        mListOptionsMenuRequest.observer()->handleActionCommand(response);
    }
}

/*!
    Private slot to switch message list contents
    to standard folder sent
*/
void NmImapClientPlugin::switchToStandardFolderSent()
{
    if (mListOptionsMenuRequest.observer()){
        NmId folderId=mUiEngine->standardFolderId(mListOptionsMenuRequest.mailboxId(),NmFolderSent);
        NmActionResponse response(NmActionResponseCommandSwitchFolder, NmActionOptionsMenu,
                                  NmActionContextViewNone, mListOptionsMenuRequest.mailboxId(),
                                  folderId);
        mListOptionsMenuRequest.observer()->handleActionCommand(response);
    }
}


/*!
    Provide id of plugin.
    Implementation of NmUiExtensionInterface.
 */
quint32 NmImapClientPlugin::pluginId()
{
    NMLOG("NmImapClientPlugin::pluginId()-->");
    return IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID;
}
