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

#ifndef LCVTINDICATORPLUGIN_H
#define LCVTINDICATORPLUGIN_H

#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>

class LcVtIndicatorPlugin : public HbIndicatorInterface, public HbIndicatorPluginInterface
{
Q_OBJECT
Q_INTERFACES(HbIndicatorPluginInterface)
public:

    LcVtIndicatorPlugin();
    ~LcVtIndicatorPlugin();

public: //from HbIndicatorPluginInterface    
    QStringList indicatorTypes() const;
    bool accessAllowed(const QString &indicatorType,
                       const QVariantMap &securityInfo) const;
    HbIndicatorInterface* createIndicator(const QString &indicatorType);
    int error() const;
    
public: //from HbIndicatorInterface
    bool handleInteraction(InteractionType type);
    QVariant indicatorData(int role) const;


protected: //from HbIndicatorInterface
    bool handleClientRequest(RequestType type, const QVariant &parameter);
    
    
private:
    
    bool bringVtToForeground();
    
private:
    Q_DISABLE_COPY(LcVtIndicatorPlugin)
    int mError;
    QStringList mIndicatorTypes;
    QVariant mParameter;
    
    friend class UT_LcVtIndicatorPlugin;
};

#endif // LCVTINDICATORPLUGIN_H
