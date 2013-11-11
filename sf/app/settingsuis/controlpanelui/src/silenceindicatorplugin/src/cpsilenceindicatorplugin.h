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

#ifndef CPSILENCEINDICATORPLUGIN_H
#define CPSILENCEINDICATORPLUGIN_H

#include <QtCore/QVariant>
#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>

class MProfileEngineExtended2;
class CpSilenceIndicatorPlugin : public HbIndicatorInterface, public HbIndicatorPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(HbIndicatorPluginInterface)
    
public:
    CpSilenceIndicatorPlugin();
    ~CpSilenceIndicatorPlugin();

public:
    //from HbIndicatorPluginInterface
    QStringList indicatorTypes() const;
    bool accessAllowed(const QString &indicatorType,
                       const QVariantMap &securityInfo ) const;
    HbIndicatorInterface* createIndicator(const QString &indicatorType);
    int error() const;
    
public:
    //from HbIndicatorInterface
    bool handleClientRequest (RequestType type, const QVariant &parameter);
    bool handleInteraction (InteractionType type); 
    QVariant indicatorData(int role) const;
    
private:
    
    Q_DISABLE_COPY(CpSilenceIndicatorPlugin)
    QStringList mIndicatorTypes;  // Type of indicator
    int mError;                   // Indicator errors
    MProfileEngineExtended2 *mEngine;        
};
#endif //CPSILENCEINDICATORPLUGIN_H
