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
* Description:  Screensaver provider interface.
*
*/

#ifndef ISCREENSAVERPROVIDER_H
#define ISCREENSAVERPROVIDER_H

#include <QtPlugin>
#include <QVariant>

/*!
    \class ScreensaverToken
    \brief Represents a screensaver that can be created.

    Screensavers are located in screensaver providers. Each provider
    exposes its contained screensavers as screensaver tokens. A token has
    all the needed information for screensaver selection and creation.
*/
class ScreensaverToken
{

public:

    /*!
        The name of the library/plugin that contains the screensaver.
    */
    QString mLibrary;

    /*!
        Uniquely identifies the screensaver.
    */
    QString mUri;

};

class Screensaver;

/*!
    \class IScreensaverProvider
    \brief Defines a mechanism for retrieving a screensaver.

    Interface that defines a mechanism for retrieving a screensaver.
    Each screensaver provider implements this interface. 
*/
class IScreensaverProvider
{

public:

    /*!
        Destructs the class.
    */
    virtual ~IScreensaverProvider() {}

public:

    /*!
        Returns contained screensavers as a list of screensaver tokens.
        \return Contained screensavers as tokens.
    */
    virtual QList<ScreensaverToken> screensavers() = 0;

    /*!
        Creates and returns a screensaver based on the given token.
        \param token Identifies the screensaver to be created.
        \return The created screensaver or null in failure cases.
    */
    virtual Screensaver *createScreensaver(const ScreensaverToken &token) = 0;

};

Q_DECLARE_METATYPE(ScreensaverToken)
Q_DECLARE_INTERFACE(IScreensaverProvider,
    "com.nokia.screensaver.iscreensaverprovider/1.0")

#endif // ISCREENSAVERPROVIDER_H
