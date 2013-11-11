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
* Description:  Screensaver factory private implementation.
*
*/

#ifndef SCREENSAVERFACTORY_P_H
#define SCREENSAVERFACTORY_P_H

#include "iscreensaverprovider.h"

class ScreensaverFactory;
class IScreensaverProvider;

class ScreensaverFactoryPrivate
{

public:

    ScreensaverFactoryPrivate(const QString& pluginDirectory, 
                                ScreensaverFactory* screensaverFactoryPublic);
    ~ScreensaverFactoryPrivate();

    Screensaver *createScreensaver(const ScreensaverToken &token);

private:

    Q_DISABLE_COPY(ScreensaverFactoryPrivate)

public:

    ScreensaverFactory *m_q;
    QString mPluginDirectory;

};

#endif // SCREENSAVERFACTORY_P_H
