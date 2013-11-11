/** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include <QDebug>
#include <smcmockclassincludes.h>

#include "phoneuiqtviewadapter.h"
#include "phoneuiqtviewif.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamaudioavailability.h"
#include "tphonecmdparamaudiooutput.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamemergencycallheaderdata.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamkeycapture.h"
#include "cphonepubsubproxy.h"

#include <mpeengineinfo.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::PhoneUIQtViewAdapter
// -----------------------------------------------------------------------------
//
PhoneUIQtViewAdapter::PhoneUIQtViewAdapter( 
        PhoneUIQtViewIF & view,
        QObject * parent ) : QObject(parent), m_view(view)
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::~PhoneUIQtViewAdapter
// -----------------------------------------------------------------------------
//
PhoneUIQtViewAdapter::~PhoneUIQtViewAdapter(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::ExecuteCommandL( 
        TPhoneViewCommandId aCmdId )
    {
    SMC_MOCK_METHOD1( void, TPhoneViewCommandId, aCmdId )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::ExecuteCommandL( 
        TPhoneViewCommandId aCmdId,
        TInt aCallId )
    {
    SMC_MOCK_METHOD2( void, TPhoneViewCommandId, aCmdId, 
        TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::ExecuteCommandL( 
        TPhoneViewCommandId aCmdId,
        TPhoneCommandParam * aCommandParam )
    {
    SMC_MOCK_METHOD2( void, TPhoneViewCommandId, aCmdId, 
        TPhoneCommandParam *, aCommandParam )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::ExecuteCommandL( 
        TPhoneViewCommandId aCmdId,
        TInt aCallId,
        TPhoneCommandParam * aCommandParam )
    {
    SMC_MOCK_METHOD3( void, TPhoneViewCommandId, aCmdId, 
        TInt, aCallId, 
        TPhoneCommandParam *, aCommandParam )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::ExecuteCommandL
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::ExecuteCommandL( 
        TPhoneViewCommandId aCmdId,
        TInt aCallId,
        TDesC & aMessage )
    {
    SMC_MOCK_METHOD3( void, TPhoneViewCommandId, aCmdId, 
        TInt, aCallId, 
        TDesC &, aMessage )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::HandleCommandL
// -----------------------------------------------------------------------------
//
TPhoneViewResponseId PhoneUIQtViewAdapter::HandleCommandL( 
        TPhoneViewCommandId aCmdId )
    {
    SMC_MOCK_METHOD1( TPhoneViewResponseId, TPhoneViewCommandId, aCmdId )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::HandleCommandL
// -----------------------------------------------------------------------------
//
TPhoneViewResponseId PhoneUIQtViewAdapter::HandleCommandL( 
        TPhoneViewCommandId aCmdId,
        TPhoneCommandParam * aCommandParam )
    {
    SMC_MOCK_METHOD2( TPhoneViewResponseId, TPhoneViewCommandId, aCmdId, 
        TPhoneCommandParam *, aCommandParam )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::ExecuteCommand
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::ExecuteCommand( 
        TPhoneViewCommandId aCmdId )
    {
    SMC_MOCK_METHOD1( void, TPhoneViewCommandId, aCmdId )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::ExecuteCommand
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::ExecuteCommand( 
        TPhoneViewCommandId aCmdId,
        TPhoneCommandParam * aCommandParam )
    {
    SMC_MOCK_METHOD2( void, TPhoneViewCommandId, aCmdId, 
        TPhoneCommandParam *, aCommandParam )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::FetchContent
// -----------------------------------------------------------------------------
//
const TDesC & PhoneUIQtViewAdapter::FetchContent(  )
    {
    SMC_MOCK_METHOD0( const TDesC & )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::noteController
// -----------------------------------------------------------------------------
//
PhoneNoteController * PhoneUIQtViewAdapter::noteController(  ) const
    {
    SMC_MOCK_METHOD0( PhoneNoteController * )
    }


// -----------------------------------------------------------------------------
// PhoneUIQtViewAdapter::setEngineInfo
// -----------------------------------------------------------------------------
//
void PhoneUIQtViewAdapter::setEngineInfo( 
        MPEEngineInfo * engineInfo )
    {
    SMC_MOCK_METHOD1( void, MPEEngineInfo *, engineInfo )
    }

void PhoneUIQtViewAdapter::dialpadClosed()
{

}

void PhoneUIQtViewAdapter::keyReleased(QKeyEvent *event)
{

}

void PhoneUIQtViewAdapter::handleWindowActivated()
{

}

void PhoneUIQtViewAdapter::handleWindowDeactivated()
{

}

void PhoneUIQtViewAdapter::onFocusLost()
{

}

void PhoneUIQtViewAdapter::onFocusGained()
{

}


