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
* Description:  Screensaver factory.
*
*/

#include <QDebug>
#include <QStringList>
#include <QDir>
#include <QPluginLoader>

#include "screensaverfactory.h"
#include "screensaverfactory_p.h"
#include "screensaver.h"

/*!
    \class ScreensaverPluginUnloader
    \brief Unloads plugin and deletes plugin loader.
    
    Holds plugin loader instance and unloads and destroys plugin
    on it's destructor.
 */

/*!
    Constructs a new SnsrPluginUnloader with \a pluginLoader and \a parent.
 */
ScreensaverPluginUnloader::ScreensaverPluginUnloader(
            QPluginLoader *pluginLoader, QObject *parent) :
    QObject(parent), mPluginLoader(pluginLoader)
{
}

/*!
    Destructs the class.
 */
ScreensaverPluginUnloader::~ScreensaverPluginUnloader()
{
    if (mPluginLoader) {
        mPluginLoader->unload();
        delete mPluginLoader;
    }
}

/*!
    \class ScreensaverFactoryPrivate
    \brief The private implementation of the ScreensaverFactory.
 */

/*!
    Constructs a new ScreensaverFactoryPrivate with \a pluginDirectory and \a screensaverFactoryPublic.
 */
ScreensaverFactoryPrivate::ScreensaverFactoryPrivate(const QString& pluginDirectory,
                                    ScreensaverFactory *screensaverFactoryPublic) :
    m_q(screensaverFactoryPublic), mPluginDirectory(pluginDirectory)
{
}

/*!
    Destructs the class.
 */
ScreensaverFactoryPrivate::~ScreensaverFactoryPrivate()
{
}

/*!
    Creates and returns a screensaver on the given token.
    \param token Identifies the screensaver to be created.
    \return The created state.
 */
Screensaver* ScreensaverFactoryPrivate::createScreensaver(const ScreensaverToken &token)
{
    QStringList pluginPaths;

    // check plugin dirs from root of different drives
    QFileInfoList drives = QDir::drives();
    for(int i=0; i < drives.count(); i++) {
        QFileInfo drive = drives.at(i);
        QString driveLetter = drive.absolutePath();
        QString path = driveLetter + mPluginDirectory;
        if (QDir(path).exists()) {
            pluginPaths << path;
        }
    }

    // check plugin dir relative to current dir
    if (QDir(mPluginDirectory).exists() && 
        !pluginPaths.contains(QDir(mPluginDirectory).absolutePath())) {
        pluginPaths << mPluginDirectory;
    }

    IScreensaverProvider *provider(0);
    QPluginLoader *loader = new QPluginLoader();
    QObject *plugin(0);

    for(int i=0; i < pluginPaths.count(); i++) {
        QString path = pluginPaths.at(i);
        QString fileName = QDir(path).absoluteFilePath(token.mLibrary);

        loader->setFileName(fileName);
        plugin = loader->instance();
        provider = qobject_cast<IScreensaverProvider*>(plugin);
        if (provider) {
            break;
        }
    }

    Screensaver *screensaver(0);

    if (provider) {
        screensaver = provider->createScreensaver(token);
        if (!screensaver) {
            qWarning() << "Screensaver creation failed.";
            qWarning() << token.mLibrary << "cannot provide" << token.mUri;
            loader->unload();
            delete loader;
        } else {
            // unload plugin once screensaver gets deleted
            ScreensaverPluginUnloader *unloader = new ScreensaverPluginUnloader(loader);
            unloader->connect(screensaver, SIGNAL(destroyed()), SLOT(deleteLater()));
        }
    } else {
        qDebug() << "Screensaver creation failed.";
        qWarning() << token.mLibrary << "- provider not found";
        loader->unload();
        delete loader;
    }

    return screensaver;
}

/*!
    \class ScreensaverFactory
    \brief Finds and creates screensavers.

    Screensaver factory creates an instance of a screensaver
    on a screensaver token that is given to it.
 */

/*!
    Constructs a new ScreensaverFactory with \a pluginDirectory and \a parent.
 */
ScreensaverFactory::ScreensaverFactory(const QString& pluginDirectory, 
                                        QObject *parent) :
    QObject(parent), m_d(new ScreensaverFactoryPrivate(pluginDirectory, this))
{
}

/*!
    Destructs the class.
 */
ScreensaverFactory::~ScreensaverFactory()
{
    delete m_d;
}

/*!
    Creates and returns a screensaver on the given token.
    \param token Identifies the screensaver to be created.
    \return The created screensaver.
 */
Screensaver* ScreensaverFactory::createScreensaver(const ScreensaverToken &token)
{
    return m_d->createScreensaver(token);
}
