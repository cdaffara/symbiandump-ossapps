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

#include "ut_cpnetworkpluginview.h"
#include "qtestmains60ui.h"
#define private public
#include "cpnetworkpluginview.h"

/*!
  ut_cpnetworkpluginview::ut_cpnetworkpluginview
 */
ut_cpnetworkpluginview::ut_cpnetworkpluginview() 
    : m_cpnetworkpluginview(NULL)
{
}


/*!
  ut_cpnetworkpluginview::~ut_cpnetworkpluginview
 */
ut_cpnetworkpluginview::~ut_cpnetworkpluginview()
{

}


/*!
  ut_cpnetworkpluginview::init
 */
void ut_cpnetworkpluginview::init()
{
    initialize();

    m_cpnetworkpluginview = new CpNetworkPluginView();
}


/*!
  ut_cpnetworkpluginview::cleanup
 */
void ut_cpnetworkpluginview::cleanup()
{
    reset();
    
    delete m_cpnetworkpluginview;
    m_cpnetworkpluginview = NULL;
}

/*!
  ut_cpnetworkpluginview::t_memleak
 */
void ut_cpnetworkpluginview::t_memleak()
{
    
}

QTEST_MAIN_S60UI(ut_cpnetworkpluginview)
