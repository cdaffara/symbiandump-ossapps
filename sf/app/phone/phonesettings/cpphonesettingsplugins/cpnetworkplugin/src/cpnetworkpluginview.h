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

#ifndef CPNETWORKPLUGINVIEW_H
#define CPNETWORKPLUGINVIEW_H

#include <cpbasesettingview.h>

class CpNetworkPluginForm;

class CpNetworkPluginView : public CpBaseSettingView
{
    Q_OBJECT
    
public:
    explicit CpNetworkPluginView(QGraphicsItem *parent = 0);
    virtual ~CpNetworkPluginView();
    void searchAvailableNetworks();

private:
    CpNetworkPluginForm* m_networkSettingsForm;

};

#endif  // CPNETWORKPLUGINVIEW_H
