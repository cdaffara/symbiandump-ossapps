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
* Description: Big clock Screensaver plugin.
*
*/

#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "snsrbigclockscreensaverplugin.h"
#include "snsrbigclockscreensaver.h"

const char *gBigClockInterfaceName = "com.nokia.screensaver.ISnsrBigClockScreensaver";

/*!
    \class SnsrBigClockScreensaverPlugin
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Screensaver big clock screensaver plugin collects ready-made screensaver clock.
 */

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    \reimp
 */
QObject *SnsrBigClockScreensaverPlugin::createInstance(
    const QServiceInterfaceDescriptor &descriptor,
    QServiceContext *context,
    QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String(gBigClockInterfaceName)) {
        LoadTranslator();
        return new SnsrBigClockScreensaver();
    }
    return 0;
}


/*!
    Loads a translator.
 */
void SnsrBigClockScreensaverPlugin::LoadTranslator()
{
    QString locale = QLocale::system().name();
    QString filename = QString("snsrbigclockscreensaverplugin_%1").arg(locale);
    QTranslator *translator = new QTranslator(this);
#ifdef Q_OS_SYMBIAN
    bool loaded(false);
    // TRAP is must here, otherwise it crashes
    TRAP_IGNORE(
        loaded = translator->load(filename,
            QString("z:/resource/qt/translations"));
        if (!loaded) {
            loaded = translator->load(filename,
                QString("c:/resource/qt/translations"));
        }
    );
#else
    translator->load(filename, QString("resource/qt/translations") );
#endif //Q_OS_SYMBIAN

    qApp->installTranslator(translator);
}

Q_EXPORT_PLUGIN2(snsrbigclockscreensaverplugin, SnsrBigClockScreensaverPlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
