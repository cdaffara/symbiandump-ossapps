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


#include "infowidgetsathandler.h"
#include "infowidgetlogging.h"
#include <rsatservice.h>

/*!
  \class InfoWidgetSatHandler
  \brief Handles SIM Application Toolkit specific functionality of 
         Operator info widget
*/

/*!
   InfoWidgetSatHandler::InfoWidgetSatHandler
 */
InfoWidgetSatHandler::InfoWidgetSatHandler(QObject *parent):
    QObject(parent),
    m_satHandlerPrivate(NULL), 
    m_connected(false)
{
    DPRINT << ": IN";
    m_satHandlerPrivate.reset(
            new InfoWidgetSatHandlerPrivate(this, m_satService));
    DPRINT << ": OUT";
}

/*!
   InfoWidgetSatHandler::~InfoWidgetSatHandler
 */
InfoWidgetSatHandler::~InfoWidgetSatHandler()
{
    DPRINT; 
}

/*!
   InfoWidgetSatHandler::connect
 */
void InfoWidgetSatHandler::connect(bool connect)
{
    DPRINT; 
    if (connect && !m_connected) {
        DPRINT << "connect and startObserving() ";
        m_connected = m_satHandlerPrivate->connect();
        if (m_connected) {
            m_satHandlerPrivate->startObserving();
        }
    } else if (!connect && m_connected) {
        DPRINT << "disconnect and stopObserving() ";
        m_satHandlerPrivate->disconnect();
        m_connected = false; 
    }
    DPRINT << ": connected = " << m_connected;
}

/*!
   InfoWidgetSatHandler::satDisplayText
 */
const QString& InfoWidgetSatHandler::satDisplayText() const
{
    DPRINT << ": text: " << m_displayText; 
    return m_displayText;
}

/*!
   InfoWidgetSatHandler::setSatDisplayText
 */
void InfoWidgetSatHandler::setSatDisplayText(
        const QString& displayText)
{
    DPRINT << ": display text: " << displayText;
    m_displayText = displayText;
}

/*!
   InfoWidgetSatHandler::handleIdleModeTxtMessage
 */
void InfoWidgetSatHandler::handleIdleModeTxtMessage(int idleResult)
{
    DPRINT << ": handleIdleModeTxtMessage: " << idleResult;
    if (m_connected) {
        m_satService.SetIdleModeTextResponse(
                static_cast<RSatService::TSATIdleResult>(idleResult));
    } else {
        m_satService.SetIdleModeTextResponse( 
                RSatService::ESATIdleCmdBeyondMeCapabilities );
    }
 
    // Emit the result
    emit handleMessage(idleResult);
}

/*!
   InfoWidgetSatHandler::handleSatError
 */
void InfoWidgetSatHandler::handleSatError(
        int operation, int errorCode)
{
    DWARNING << ": satError : operation: " << 
            operation << ": errorCode: "<<errorCode;
    m_satService.SetIdleModeTextResponse( 
            RSatService::ESATIdleMeUnableToProcessCmd);
    emit handleError(operation, errorCode);
}

// End of File. 

