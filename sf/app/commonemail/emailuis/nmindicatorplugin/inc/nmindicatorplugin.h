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
* Description: Mail Indicator Plugin Class
*
*/

#ifndef NMINDICATORPLUGIN_H
#define NMINDICATORPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QTranslator>

#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>

class NmIndicator;

class NmIndicatorPlugin : public QObject, public HbIndicatorPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(HbIndicatorPluginInterface)

public:

     NmIndicatorPlugin();
    ~NmIndicatorPlugin();

public: // From HbIndicatorPluginInterface
    QStringList indicatorTypes() const;
    bool accessAllowed(const QString &indicatorType,
        const QVariantMap &securityInfo) const;
        
    HbIndicatorInterface* createIndicator(const QString &indicatorType);
    int error() const;

private:
    
    Q_DISABLE_COPY(NmIndicatorPlugin)

    int mError;
    QStringList mIndicatorTypes;
    QTranslator *mTranslator; // owned
};

#endif // NMINDICATORPLUGIN_H

