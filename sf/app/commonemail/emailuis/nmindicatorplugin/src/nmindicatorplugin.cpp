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

#include "nmindicatorplugin.h"
#include "nmindicator.h"
#include "nmsendingindicator.h"
#include "nmsyncindicator.h"
#include "nmunreadindicator.h"

#include <QCoreApplication>
#include <QLocale>
#include <QVariant>
#include <QTimer>

Q_EXPORT_PLUGIN(NmIndicatorPlugin)

const int NmMaxIndicatorCount = 10;
static const QString NmIndicatorName = "com.nokia.nmail.indicatorplugin_%1/1.0";


/*!
    \class NmIndicatorPlugin
    \brief Mail indicator plugin class.
*/

/*!
     Class constructor.
*/
NmIndicatorPlugin::NmIndicatorPlugin()
: mError(0), mTranslator(0)
{
    NM_FUNCTION;
}

/*!
    Class destructor.
*/
NmIndicatorPlugin::~NmIndicatorPlugin()
{
    NM_FUNCTION;
    
	delete mTranslator;
}

/*!
	Return notification types this plugin implements
 */
QStringList NmIndicatorPlugin::indicatorTypes() const
{
    NM_FUNCTION;
    
    QStringList types;
    for (int i=0; i<NmMaxIndicatorCount; i++) {
		QString name = QString(NmIndicatorName).arg(i);
		types << name;
    }
    types << NmSendingIndicatorType;
	types << NmSyncIndicator::IndicatorType;
    types << NmUnreadIndicatorType;
    return types;
}

/*!
    Check if client is allowed to use notification widget
    see HbIndicatorPluginInterface
*/
bool NmIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const QVariantMap &securityInfo) const
{
    NM_FUNCTION;
    
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

/*!
	Creates an indicator of type indicatorType. Ownership is passed to the caller.
 */
HbIndicatorInterface* NmIndicatorPlugin::createIndicator(
        const QString &indicatorType)
{
    NM_FUNCTION;

    if (!mTranslator) {
        mTranslator = new QTranslator();
        QString lang = QLocale::system().name();
        QString appName = "mail_";
        QString path = "z:/resource/qt/translations/";
        mTranslator->load(appName + lang, path);
        QCoreApplication::installTranslator(mTranslator);
    }

    HbIndicatorInterface *indicator;
    if (indicatorType == NmSendingIndicatorType) {
        indicator = new NmSendingIndicator(indicatorType);
    }
    else if (indicatorType == NmUnreadIndicatorType) {
        indicator = new NmUnreadIndicator(indicatorType);
    }
    else if (indicatorType == NmSyncIndicator::IndicatorType) {
        indicator = new NmSyncIndicator();
    }
    else {
        indicator = new NmIndicator(indicatorType);
    }

    return indicator;
}

/*!
	Returns the last error code.
 */
int NmIndicatorPlugin::error() const
{
    NM_FUNCTION;
    
    return mError;
}
