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

#include "cpnetworkpluginview.h"
#include "cpnetworkpluginform.h"
#include "cppluginlogging.h"

/*!
  Constructor. 
 */
CpNetworkPluginView::CpNetworkPluginView(QGraphicsItem *parent) :
    CpBaseSettingView(NULL, parent),
    m_networkSettingsForm(NULL)
{
    DPRINT;
    m_networkSettingsForm = new CpNetworkPluginForm();
    // Base class takes ownership of the form
    this->setWidget(m_networkSettingsForm);
    }

/*!
  Destructor. 
 */
CpNetworkPluginView::~CpNetworkPluginView()
{
    DPRINT;
}

/*!
  searchAvailableNetworks.    
 */
void CpNetworkPluginView::searchAvailableNetworks()
{
    DPRINT;
    
    m_networkSettingsForm->searchAvailableNetworks();
}
    
// End of File. 
