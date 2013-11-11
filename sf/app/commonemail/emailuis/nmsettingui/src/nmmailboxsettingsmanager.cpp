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
*
*/

#include <QPluginLoader>
#include <QDir>
#include <QScopedPointer>
#include <HbView>
#include <QVariant>
#include <nmdatamanager.h>
#include <nmicons.h>
#include <nmcommon.h>

#include "nmmailboxsettingsmanager.h"
#include "nmsettingsplugininterface.h"


/*!
    \class NmMailboxSettingsManager
    \brief Provides access to mailbox settings through the setting plug-ins.

*/


// ======== HELPER FUNCTIONS ========

/*!
  Creates list of folder paths where plug-ins can be loaded from.
  \return folder path list.
*/
QStringList pluginFolders()
{
    NM_FUNCTION;

    const QString nmSettingsPluginFolderPath("resource/qt/plugins/nmail/settings");
    QStringList pluginDirectories;
    QFileInfoList driveList = QDir::drives();

    foreach(const QFileInfo &driveInfo, driveList) {
        QString pluginDirectory =
            driveInfo.absolutePath() + nmSettingsPluginFolderPath;

        if (QFileInfo(pluginDirectory).exists()) {
            pluginDirectories.append(pluginDirectory);
        }
    }

    return pluginDirectories;
}


// ======== MEMBER FUNCTIONS ========

/*!
    Contructor of NmMailboxSettingsManager.
*/
NmMailboxSettingsManager::NmMailboxSettingsManager()
: mDataManager(NULL)
{
    NM_FUNCTION;

    QScopedPointer<NmDataManager> dataManager(new NmDataManager());
    loadPlugins();
    mDataManager = dataManager.take();
}


/*!
    Destructor of NmMailboxSettingsManager.
*/
NmMailboxSettingsManager::~NmMailboxSettingsManager()
{
    NM_FUNCTION;

    unloadPlugins();
    qDeleteAll(mPluginLoaders);
    mPluginLoaders.clear();
    NmIcons::freeIcons();
    delete mDataManager;
}


/*!
    Populates the given list with available mailboxes.

    \param mailboxList List to be popuplated with mailbox entries.

*/
void NmMailboxSettingsManager::listMailboxes(QList<NmMailbox *> &mailboxList)
{
    NM_FUNCTION;

    mDataManager->listMailboxes(mailboxList);
}


/*!
    Returns a branded icon for the given mailbox id.

    \param mailboxId Id of the mailbox.
    \return Icon for the mailbox.
*/
HbIcon &NmMailboxSettingsManager::mailboxIcon(const NmId &mailboxId) const
{
    NM_FUNCTION;

    Q_UNUSED(mailboxId);
    // TODO: use some branding feature when it is available.
    return NmIcons::getIcon(NmIcons::NmIconDefaultMailbox);
}


/*!
    Populates the given data form model with setting items from the correct
    plug-in.

    \param model Model to be populated with the setting items.
    \param form The form where the setting items are placed.
    \param mailboxId Id of the mailbox which's settings are to be populated to
                     the model.
*/
void NmMailboxSettingsManager::populateModel(HbDataFormModel &model,
    HbDataForm &form, const NmId &mailboxId) const
{
    NM_FUNCTION;

    NmSettingsPluginInterface *plugin = NULL;

    foreach (QPluginLoader *loader, mPluginLoaders) {
        QObject *pluginInstance = loader->instance();
        plugin = qobject_cast<NmSettingsPluginInterface *>(pluginInstance);

        if (plugin && plugin->populateModel(model, form, mailboxId)) {

            // We need to disconnect possible previous connections, because we only have
            // one instance of every plugin. This way we can be sure that signal is send to
            // correct object.
            pluginInstance->disconnect(SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));
            pluginInstance->disconnect(SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));
            pluginInstance->disconnect(SIGNAL(goOnline(const NmId &)));
            pluginInstance->disconnect(SIGNAL(goOffline(const NmId &)));
            pluginInstance->disconnect(this, SIGNAL(aboutToClose()), pluginInstance, SLOT(aboutToClose()));

            connect(pluginInstance, SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                this, SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

            connect(pluginInstance, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                this, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));

            connect(pluginInstance, SIGNAL(goOnline(const NmId &)),
                this, SIGNAL(goOnline(const NmId &)));

            connect(pluginInstance, SIGNAL(goOffline(const NmId &)),
                this, SIGNAL(goOffline(const NmId &)));

            connect(this, SIGNAL(aboutToClose()),
            	pluginInstance, SLOT(aboutToClose()));

            break;
        }
    }
}


/*!
    Loads plug-ins which implements NmSettingsPluginInterface and are registered
    to <code>/resource/qt/plugins/nmail/settings</code> folder.
*/
void NmMailboxSettingsManager::loadPlugins()
{
    NM_FUNCTION;

    QStringList directories(pluginFolders());

    foreach (const QString &directoryPath, directories) {
        QDir directory(directoryPath);
        QFileInfoList fileList(directory.entryInfoList());

        foreach (const QFileInfo &fileInfo, fileList) {
            QScopedPointer<QPluginLoader> loader(
                new QPluginLoader(fileInfo.absoluteFilePath()));
            mPluginLoaders.append(loader.data());
            loader.take();
        }
    }
}


/*!
    Unloads plug-ins to ensure that the root component is deleted.
*/
void NmMailboxSettingsManager::unloadPlugins()
{
    NM_FUNCTION;

    foreach (QPluginLoader *loader, mPluginLoaders) {
        loader->unload();
    }
}

// End of file.
