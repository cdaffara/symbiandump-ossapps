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

#ifndef SCREENSAVERFACTORY_H
#define SCREENSAVERFACTORY_H

#include <QObject>

#include "iscreensaverprovider.h"
#include "screensavermodel_global.h"

class QPluginLoader;

class SREENSAVERMODEL_EXPORT ScreensaverPluginUnloader : public QObject
{
    Q_OBJECT

public:

    ScreensaverPluginUnloader(QPluginLoader *pluginLoader, QObject *parent = 0);
    ~ScreensaverPluginUnloader();

private:

    QPluginLoader *mPluginLoader;

};

class ScreensaverFactoryPrivate;

class SREENSAVERMODEL_EXPORT ScreensaverFactory : public QObject
{
    Q_OBJECT

public:

    ScreensaverFactory(const QString& pluginDirectory, QObject *parent = 0);
    virtual ~ScreensaverFactory();

    Screensaver *createScreensaver(const ScreensaverToken &token);

private:

    Q_DISABLE_COPY(ScreensaverFactory)

private:

    ScreensaverFactoryPrivate *m_d;
    friend class ScreensaverFactoryPrivate;

};

#endif // SCREENSAVERFACTORY_H
