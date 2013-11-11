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

#include "infowidgetengine.h"
#include "infowidgetlogging.h"

/*!
  Stubbed class implementation for testing  
  Widget on windows/other environment.  
*/


/*!
   InfoWidgetEngine::InfoWidgetEngine
 */
InfoWidgetEngine::InfoWidgetEngine(QObject *parent): 
    QObject(parent),
    m_networkHandler(NULL),
    m_satHandler(NULL)
{
    DPRINT << ": STUBBED";
}

/*!
   InfoWidgetEngine::~InfoWidgetEngine
 */
InfoWidgetEngine::~InfoWidgetEngine()
{
    DPRINT << ": STUBBED";
}    

/*!
   InfoWidgetEngine::modelData
 */
const InfoWidgetEngine::ModelData& InfoWidgetEngine::modelData() const
{
    DPRINT << ": STUBBED";
    return m_modelData; 
}

/*!
   InfoWidgetEngine::updateNetworkDataToModel() 
 */
void InfoWidgetEngine::updateNetworkDataToModel()
{
    DPRINT << ": STUBBED"; 
}

/*!
   InfoWidgetEngine::updateSatDataToModel
 */
void InfoWidgetEngine::updateSatDataToModel()
{
    DPRINT << ": STUBBED";
}

/*!
   InfoWidgetEngine::updateLineDataToModel
 */
void InfoWidgetEngine::updateLineDataToModel()
{
    DPRINT << ": STUBBED";
}

/*!
   InfoWidgetEngine::handleNetworkError
 */
void InfoWidgetEngine::handleNetworkError(
        int operation, int errorCode)
{
    DPRINT << ": STUBBED"; 
}

/*!
   InfoWidgetEngine::handleSatError
 */
void InfoWidgetEngine::handleSatError(
        int operation, int errorCode)
{
    DPRINT << ": STUBBED"; 
}

/*!
   InfoWidgetEngine::handleLineError
 */
void InfoWidgetEngine::handleLineError(
        int operation, int errorCode)
{
    DPRINT << ": STUBBED"; 
}

/*!
   InfoWidgetEngine::handlePreferencesChanged
 */
void InfoWidgetEngine::handlePreferencesChanged(
        InfoWidgetPreferences::Options options)
{
    DPRINT << ": STUBBED";
}

/*!
   InfoWidgetEngine::suspend
 */
void InfoWidgetEngine::suspend() 
{
    DPRINT << ": STUBBED";
}

/*!
   InfoWidgetEngine::preferenceChanged
 */
void InfoWidgetEngine::resume()
{
    DPRINT << ": STUBBED"; 
}


// End of File. 

