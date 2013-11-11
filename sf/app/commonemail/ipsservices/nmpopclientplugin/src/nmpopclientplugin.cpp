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

#include "nmpopclientpluginheaders.h"
#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#endif

Q_EXPORT_PLUGIN(NmPopClientPlugin)


/*!
 \class NmPopClientPlugin

 \brief The NmPopClientPlugin plugin provides ui services for POP mail protocol.
*/

/*!
    Constructs a new NmPopClientPlugin.
 */
NmPopClientPlugin::NmPopClientPlugin()
: mListOptionsMenuRequest(NULL),
mSettingsViewLauncher(NULL)
{
    NMLOG("NmPopClientPlugin::NmPopClientPlugin()-->");
    NMLOG("<--NmPopClientPlugin::NmPopClientPlugin()");
}

/*!
    Destructor.
 */
NmPopClientPlugin::~NmPopClientPlugin()
{
    NMLOG("NmPopClientPlugin::~NmPopClientPlugin()-->");
    delete mSettingsViewLauncher;
    NMLOG("<--NmPopClientPlugin::~NmPopClientPlugin()");
}


/*!
    Provides list of supported NmActions.
    Implementation of NmUiExtensionInterface.
    Parameter \a request controls list of request services.
    Parameter \a actionList is updated by supported NmActions.

 */
void NmPopClientPlugin::getActions(
      const NmActionRequest &request,
      QList<NmAction*> &actionList)
{
    NMLOG("NmPopClientPlugin::getActions()-->");
    // check if this request is for POP protocol.
    quint32 pluginId = this->pluginId();
    if (request.mailboxId().pluginId32() != pluginId
#ifdef USE_POPIMAP_TESTPLUGIN
        && request.mailboxId().pluginId32() != POPTESTPLUGINID
#endif
    ) {
        NMLOG("NmPopClientPlugin::getActions(): PluginId incorrect.");
        NMLOG(QString("request.mailboxId().pluginId32()=%1").arg(request.mailboxId().pluginId32()));
        return;
    }

    // Get the NmBaseClientPlugin implementation of the UI actions.
    NmBaseClientPlugin::getActions(request, actionList);

    // Append POP plugin specific actions
    switch (request.menuType()) {
    case NmActionOptionsMenu:
        {
        if (request.contextView()==NmActionContextViewMessageList){
            mListOptionsMenuRequest = request;
            // Create sub-menu for standard folders selection
            HbMenu *standardFolders = new HbMenu();
            NmAction *inboxAction = new NmAction(0);
            inboxAction->setObjectName("popclientplugin_folders_inbox");
            inboxAction->setText(hbTrId("txt_mail_opt_folder_sub_inbox"));
            inboxAction->setCheckable(true);
            connect(inboxAction, SIGNAL(triggered()), this, SLOT(switchToStandardFolderInbox()));
            standardFolders->addAction(inboxAction);
            NmAction *outboxAction = new NmAction(0);
            outboxAction->setObjectName("popclientplugin_folders_outbox");
            outboxAction->setText(hbTrId("txt_mail_opt_folder_sub_outbox"));
            outboxAction->setCheckable(true);
            connect(outboxAction, SIGNAL(triggered()), this, SLOT(switchToStandardFolderOutbox()));
            standardFolders->addAction(outboxAction);
            NmAction *draftsAction = new NmAction(0);
            draftsAction->setObjectName("popclientplugin_folders_drafts");
            draftsAction->setText(hbTrId("txt_mail_opt_folder_sub_drafts"));
            draftsAction->setCheckable(true);
            connect(draftsAction, SIGNAL(triggered()), this, SLOT(switchToStandardFolderDrafts()));
            standardFolders->addAction(draftsAction);
            NmAction *sentAction = new NmAction(0);
            sentAction->setObjectName("popclientplugin_folders_sent");
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

    NMLOG("<--NmPopClientPlugin::getActions()");
}

/*!
    Slot connected to options menu settings NmAction.
    Opens mailbox settings.
 */
void NmPopClientPlugin::settings()
{
    NMLOG("NmPopClientPlugin::settings()-->");

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

    NMLOG("<--NmPopClientPlugin::settings()");
}

/*!
    Private slot to switch message list contents
    to standard folder inbox
*/
void NmPopClientPlugin::switchToStandardFolderInbox()
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
void NmPopClientPlugin::switchToStandardFolderOutbox()
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
void NmPopClientPlugin::switchToStandardFolderDrafts()
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
void NmPopClientPlugin::switchToStandardFolderSent()
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
quint32 NmPopClientPlugin::pluginId()
{
    NMLOG("NmPopClientPlugin::pluginId()-->");
    return IPSSOSPOP3PLUGIN_IMPLEMENTATION_UID;
}
