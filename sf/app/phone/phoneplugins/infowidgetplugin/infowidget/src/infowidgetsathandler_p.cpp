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

#include <fbs.h>
#include "infowidgetsathandler_p.h"
#include "infowidgetsathandler.h"
#include "infowidgetlogging.h"

/*!
  \class InfoWidgetSatHandlerPrivate
  \brief Private implementation of InfoWidgetSatHandler  
*/

/*!
   Constructor. 
 */
InfoWidgetSatHandlerPrivate::InfoWidgetSatHandlerPrivate(
        InfoWidgetSatHandler *satHandler, RSatService& satService)
    : CActive(CActive::EPriorityStandard), 
      m_satHandler(satHandler),
      m_satService(satService)
{
    DPRINT;
    CActiveScheduler::Add(this);
}

/*!
   Desctuctor.            
*/
InfoWidgetSatHandlerPrivate::~InfoWidgetSatHandlerPrivate()
{
    DPRINT;
    Cancel();
}

/*!
   Connect RSatSession and open RSatService. 
 */
bool InfoWidgetSatHandlerPrivate::connect()
{
    TInt err( KErrNone );
    bool connected(false); 
    
    TRAP(err, m_satSession.ConnectL());
    if (KErrNone != err) {
        DCRITICAL << ": Exception occured while connecting SatSession:" << err;
    }
    else{
        TRAP(err, m_satService.OpenL(m_satSession));
        if (KErrNone != err) {
            DCRITICAL << ": Exception occured while opening SatService:" << err;
            m_satSession.Close(); 
        } else {
            connected = true; 
        }
    }
    
    return connected;
}

/*! 
   Gets initial content if any and starts 
   listening for changes.     
 */
void InfoWidgetSatHandlerPrivate::startObserving()
{
    // Not observing yet but
    getIdleModeData();
    // in case there were content, there is need to send response to SAT
    if (!m_idleTxt.isEmpty()) {
        m_satHandler->handleIdleModeTxtMessage(m_idleResult);
    }
    // Start observing for changes
    activate();
}

/*!
   Disconnect session(s) and reset idle text data.    
 */
void InfoWidgetSatHandlerPrivate::disconnect()
{
    DPRINT;
    m_idleTxt.clear();
    Cancel();
    m_satService.NotifySetupIdleModeTextChangeCancel(); 
    m_satService.Close();
    m_satSession.Close();
}

/*!
   Get idle mode text.   
 */
void InfoWidgetSatHandlerPrivate::getIdleModeData()
{
    DPRINT << ": IN"; 
    HBufC* string(NULL);
    TUint8 recordId(NULL);
    RSatService::TSatIconQualifier iconQualifier( 
        RSatService::ESatIconNoIcon );
    
    // Get setup idle mode text, icon qualifier and record id.
    TInt result(KErrNotFound);
    TRAP(result, m_satService.GetSetupIdleModeTextL(
            string, iconQualifier, recordId)); 
    
    if (0 != result) {
        DWARNING << ": Exception occured while GetSetupIdleModeTextL :" << result;
        m_idleResult = RSatService::ESATIdleMeUnableToProcessCmd;
    } else{
        if (RSatService::ESatIconSelfExplanatory != iconQualifier) { 
            m_idleTxt = QString((QChar*)string->Des().Ptr(), string->Length());
        }else {
            m_idleTxt.clear();
        }
        //Determine result
        if(RSatService::ESatIconNoIcon != iconQualifier
                && !m_idleTxt.isEmpty()){
            // Icon requested but we don't have icon support
            m_idleResult = RSatService::ESATIdleSuccessRequestedIconNotDisplayed;
        }else if(RSatService::ESatIconNoIcon == iconQualifier
                && !m_idleTxt.isEmpty()){
            m_idleResult = RSatService::ESATIdleSuccess;
        }else{ 
            // Got empty string, could be permanent problem as well?
            m_idleResult = RSatService::ESATIdleMeUnableToProcessCmd;
        }
    }
    delete string;
    
    // Text (m_idleTxt) is empty in case of selfexplanatory icon
    m_satHandler->setSatDisplayText(m_idleTxt); 
    DPRINT << ": OUT"; 
}

/*!
   InfoWidgetSatHandlerPrivate::RunL   
 */
void InfoWidgetSatHandlerPrivate::RunL()
{ 
    DPRINT;
    if (KErrNone == iStatus.Int()) {   
        getIdleModeData();
        m_satHandler->handleIdleModeTxtMessage(m_idleResult);
    } else {
        m_satHandler->handleSatError(1, iStatus.Int());
    }

    if (!IsActive()) {
        activate();
    }       
}

/*!
   Cancel SetupIdleModeTextChange notify.     
 */
void InfoWidgetSatHandlerPrivate::DoCancel()
{
    DPRINT;
    m_satService.NotifySetupIdleModeTextChangeCancel(); 
}

/*!
   Start listening SetupIdleModeTextChange notify.    
 */
void InfoWidgetSatHandlerPrivate::activate()
{
    DPRINT;
    Cancel();   
    TInt error = m_satService.NotifySetupIdleModeTextChange(iStatus);
    
    if (KErrNone == error) {
        SetActive();
        }
}

// End of File. 
    

