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

#include "ut_infowidgetsathandlerprivate.h"
#define private public
#include "infowidgetsathandler_p.h"
#include "infowidgetsathandler.h"
#include "infowidgetlogging.h"
#include "qtestmains60.h"

static struct ActiveObjectControl {
    enum TOperation {
        EUndefined = 0, 
        ECallRunL, 
        ECallDoCancel,
        EExitSequence
    } operation;
    int iStatus; 
    CActive* object; 
} m_activeObjectControl; 

 
void CActive::SetActive(){
    if (m_activeObjectControl.object) 
    {
    switch (m_activeObjectControl.operation) {
        case ActiveObjectControl::ECallRunL: 
            m_activeObjectControl.operation = ActiveObjectControl::EExitSequence;
            m_activeObjectControl.object->iStatus = 
                    m_activeObjectControl.iStatus;
            m_activeObjectControl.object->RunL(); 
            break;
        case ActiveObjectControl::ECallDoCancel: 
            m_activeObjectControl.operation = ActiveObjectControl::EExitSequence;
            m_activeObjectControl.object->iStatus = 
                    m_activeObjectControl.iStatus;
            m_activeObjectControl.object->DoCancel(); 
            break;
            
        default: 
            break; 
        }
    
    // Reset, causes infinite loop otherwise 
    m_activeObjectControl.operation = ActiveObjectControl::EUndefined; 
    m_activeObjectControl.object = NULL; 
    }
} 

void SimulateLeaveL()
{
    User::Leave(KErrGeneral);
}

void FillData( HBufC * aText,
        RSatService::TSatIconQualifier & aIconQualifier,
        TUint8 & aRecordNumber)
{
    _LIT(KText,"Idle Text");
    TPtr p(aText->Des());
    p.Copy(KText);
    aIconQualifier=RSatService::ESatIconNotSelfExplanatory;
    aRecordNumber=1;
}

void FillDataNoIcon( HBufC * aText,
        RSatService::TSatIconQualifier & aIconQualifier,
        TUint8 & aRecordNumber)
{
    Q_UNUSED(aIconQualifier);
    Q_UNUSED(aRecordNumber);
    _LIT(KText,"Idle Text");
    TPtr p(aText->Des());
    p.Copy(KText);
}

void FillDataSelfExplIcon( HBufC * aText,
        RSatService::TSatIconQualifier & aIconQualifier,
        TUint8 & aRecordNumber)
{
    Q_UNUSED(aText);
    aIconQualifier=RSatService::ESatIconSelfExplanatory;
    aRecordNumber=1;
}

/*!
  UT_InfoWidgetSatHandlerPrivate ::UT_InfoWidgetSatHandlerPrivate 
 */
UT_InfoWidgetSatHandlerPrivate::UT_InfoWidgetSatHandlerPrivate () 
{
    DPRINT << "IN";
    
    m_satHandlerPrivate = 0;
    m_iwSatHandler = 0;
    
    DPRINT << "OUT";
}

/*!
  UT_InfoWidgetSatHandlerPrivate ::~UT_InfoWidgetSatHandlerPrivate 
 */
UT_InfoWidgetSatHandlerPrivate::~UT_InfoWidgetSatHandlerPrivate ()
{
    DPRINT << "IN";
    
    delete m_satHandlerPrivate;
    delete m_iwSatHandler;
    
    DPRINT << "OUT";
}

/*!
  UT_InfoWidgetSatHandlerPrivate ::init
 */
void UT_InfoWidgetSatHandlerPrivate::init()
{
    DPRINT << "IN";
    
    initialize();
    m_iwSatHandler = new InfoWidgetSatHandler;
    m_satHandlerPrivate =  new InfoWidgetSatHandlerPrivate( m_iwSatHandler, m_satService);
    
    QVERIFY(verify());

    DPRINT << "OUT";
}

/*!
  UT_InfoWidgetSatHandlerPrivate ::cleanup
 */
void UT_InfoWidgetSatHandlerPrivate::cleanup()
{
    DPRINT << "IN";
    
    reset();
    delete m_satHandlerPrivate;
    delete m_iwSatHandler;

    DPRINT << "OUT";
}

/*!
  UT_InfoWidgetSatHandlerPrivate::t_connect
 */
void UT_InfoWidgetSatHandlerPrivate::t_connect()
{
    DPRINT << "IN";
    
    expect("RSatSession::ConnectL");
    expect("RSatService::OpenL");
    m_satHandlerPrivate->connect();
     
    expect("RSatSession::ConnectL")
        .willOnce(invokeWithoutArguments(SimulateLeaveL));
    m_satHandlerPrivate->connect();
    
    expect("RSatService::OpenL")
        .willOnce(invokeWithoutArguments(SimulateLeaveL));
    m_satHandlerPrivate->connect();

    QVERIFY(verify());

    DPRINT << "OUT";
}

/*!
  UT_InfoWidgetSatHandlerPrivate::t_disconnect
 */
void UT_InfoWidgetSatHandlerPrivate::t_disconnect()
{
    DPRINT << "IN";
    
    m_satHandlerPrivate->disconnect();
    QVERIFY(verify());

    DPRINT << "OUT";
}

/*!
  UT_InfoWidgetSatHandlerPrivate::t_startObserving
 */
void UT_InfoWidgetSatHandlerPrivate::t_startObserving()
{
    DPRINT << "IN";
    
    int i = KErrGeneral;
    expect("RSatService::NotifySetupIdleModeTextChange").returns(i);
    expect("RSatService::GetSetupIdleModeTextL")
        .willOnce(invoke(FillData));
    m_satHandlerPrivate->startObserving();
    
    expect("RSatService::NotifySetupIdleModeTextChange").returns(i);
    expect("RSatService::GetSetupIdleModeTextL")
        .willOnce(invoke(FillDataNoIcon));
    m_satHandlerPrivate->startObserving();
    
    expect("RSatService::NotifySetupIdleModeTextChange").returns(i);
    expect("RSatService::GetSetupIdleModeTextL")
        .willOnce(invoke(FillDataSelfExplIcon));
    m_satHandlerPrivate->startObserving();
    
    expect("RSatService::GetSetupIdleModeTextL")
        .willOnce(invokeWithoutArguments(SimulateLeaveL));
    m_satHandlerPrivate->startObserving();
    
    QVERIFY(verify());

    DPRINT << "OUT";
}

/*!
  UT_InfoWidgetSatHandlerPrivate::t_runL
 */
void UT_InfoWidgetSatHandlerPrivate::t_runL()
{
    DPRINT;
    expect("RSatService::GetSetupIdleModeTextL")
          .willOnce(invoke(FillData));
    expect("RSatService::NotifySetupIdleModeTextChange").returns(KErrNone);
    
    TRequestStatus& status(m_satHandlerPrivate->iStatus);
    expect("RSatService::NotifySetupIdleModeTextChange").with(status);
    
    // Test RunL sequence with iStatus == KErrNone  
    m_activeObjectControl.operation = ActiveObjectControl::ECallRunL; 
    m_activeObjectControl.iStatus = KErrNone; 
    m_activeObjectControl.object = m_satHandlerPrivate; 
    m_satHandlerPrivate->startObserving();
    
    // Test RunL sequence with iStatus == KErrGeneral
    m_activeObjectControl.operation = ActiveObjectControl::ECallRunL; 
    m_activeObjectControl.iStatus = KErrGeneral; 
    m_activeObjectControl.object = m_satHandlerPrivate; 
    m_satHandlerPrivate->startObserving();
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetSatHandlerPrivate::t_doCancel
 */
void UT_InfoWidgetSatHandlerPrivate::t_doCancel()
{    
    DPRINT << "IN";

    m_activeObjectControl.operation = ActiveObjectControl::ECallDoCancel; 
    m_activeObjectControl.iStatus = KErrNone; 
    m_activeObjectControl.object = m_satHandlerPrivate; 
    m_satHandlerPrivate->startObserving();
    
    QVERIFY(verify());

    DPRINT << "OUT";
}

QTEST_MAIN_S60(UT_InfoWidgetSatHandlerPrivate )
