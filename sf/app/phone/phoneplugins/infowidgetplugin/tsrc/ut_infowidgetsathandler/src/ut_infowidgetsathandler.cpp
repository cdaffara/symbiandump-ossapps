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
#include "ut_infowidgetsathandler.h"
#define private public
#include "infowidgetsathandler.h"
#include "qtestmains60.h"

/*!
  UT_InfoWidgetSatHandler::UT_InfoWidgetSatHandler
 */
UT_InfoWidgetSatHandler::UT_InfoWidgetSatHandler() 
    : 
    m_satHandler(NULL)
{
    
}


/*!
  UT_InfoWidgetSatHandler::~UT_InfoWidgetSatHandler
 */
UT_InfoWidgetSatHandler::~UT_InfoWidgetSatHandler()
{
    delete m_satHandler;
}

/*!
  UT_InfoWidgetSatHandler::init
 */
void UT_InfoWidgetSatHandler::init()
{
    initialize();
    
    m_satHandler =  new InfoWidgetSatHandler();
    
    QVERIFY(verify());
}


/*!
  UT_InfoWidgetSatHandler::cleanup
 */
void UT_InfoWidgetSatHandler::cleanup()
{
    reset();
    
    delete m_satHandler;
    m_satHandler = NULL;
}


/*!
  UT_InfoWidgetSatHandler::t_satDisplayText
 */
void UT_InfoWidgetSatHandler::t_satDisplayText()
{
    QString satDisplayText("satDisplayText");
    m_satHandler->setSatDisplayText(satDisplayText);
    QVERIFY(satDisplayText == m_satHandler->satDisplayText());
}


/*!
  UT_InfoWidgetSatHandler::t_setSatDisplayText
 */
void UT_InfoWidgetSatHandler::t_setSatDisplayText()
{
    QString satDisplayText("satDisplayText");
    m_satHandler->setSatDisplayText(satDisplayText);
    
    QVERIFY(satDisplayText == m_satHandler->satDisplayText());
}


void UT_InfoWidgetSatHandler::t_connect()
{
    //}else if (!connect && m_connected){
    m_satHandler->m_connected = true;
    m_satHandler->connect(false);
    
        
    m_satHandler->m_connected = true;
    m_satHandler->connect(true);
    
    //if(connect && !m_connected){
    m_satHandler->m_connected = false;
    m_satHandler->connect(false);
    
    QVERIFY(verify());
}


void UT_InfoWidgetSatHandler::t_handleIdleModeTxtMessage()
{
    m_satHandler->m_connected = false;
    m_satHandler->handleIdleModeTxtMessage(0);
    m_satHandler->m_connected = true;
    m_satHandler->handleIdleModeTxtMessage(0);
    
    QVERIFY(verify());
}


void UT_InfoWidgetSatHandler::t_handleSatError()
{
    m_satHandler->handleSatError(1,1);
    
    QVERIFY(verify());
}
    

QTEST_MAIN_S60(UT_InfoWidgetSatHandler)
