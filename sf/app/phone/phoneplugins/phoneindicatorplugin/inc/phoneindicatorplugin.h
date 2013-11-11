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

#ifndef PHONEINDICATORPLUGIN_H
#define PHONEINDICATORPLUGIN_H

#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>

class PhoneIndicatorPlugin : public QObject, public HbIndicatorPluginInterface
{
Q_OBJECT
Q_INTERFACES(HbIndicatorPluginInterface)
public:
    PhoneIndicatorPlugin();
    ~PhoneIndicatorPlugin();

    QStringList indicatorTypes() const;
    bool accessAllowed(const QString &indicatorType,
                       const QVariantMap &securityInfo) const;
    HbIndicatorInterface* createIndicator(const QString &indicatorType);
    int error() const;
private:
    int typeIndex(const QString &indicatorType) const;

private:
    Q_DISABLE_COPY(PhoneIndicatorPlugin)
    int m_error;
    QStringList m_indicatorTypes;
};

#endif // PHONEINDICATORPLUGIN_H
