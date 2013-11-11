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
#include <QDebug>
#include <smcmockclassincludes.h>
#include "cpplugincommon.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SettingsWrapper::SettingsWrapper
// -----------------------------------------------------------------------------
//
SettingsWrapper::SettingsWrapper( 
        QObject * parent ) : QObject( parent )
    {
    
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::~SettingsWrapper
// -----------------------------------------------------------------------------
//
SettingsWrapper::~SettingsWrapper(  )
    {
    
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::showCallDuration
// -----------------------------------------------------------------------------
//
bool SettingsWrapper::showCallDuration(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::setShowCallDuration
// -----------------------------------------------------------------------------
//
int SettingsWrapper::setShowCallDuration( 
        bool value )
    {
    SMC_MOCK_METHOD1( int, bool, value )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::readVtVideoSending
// -----------------------------------------------------------------------------
//
int SettingsWrapper::readVtVideoSending(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::writeVtVideoSending
// -----------------------------------------------------------------------------
//
int SettingsWrapper::writeVtVideoSending( 
        int value )
    {
    SMC_MOCK_METHOD1( int, int, value )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::readSoftRejectText
// -----------------------------------------------------------------------------
//
void SettingsWrapper::readSoftRejectText( 
        QString & text, bool & userDefined )
    {
    //To improve coverage
    static int i=0;
    if(i==0){
        text = "Soittelen myöhemmin...";
        userDefined = true;
        i=1;
    }
    SMC_MOCK_METHOD2( void, QString &, text,
        bool &, userDefined )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::writeSoftRejectText
// -----------------------------------------------------------------------------
//
int SettingsWrapper::writeSoftRejectText( 
        const QString & text,
        bool userDefined )
    {
    SMC_MOCK_METHOD2( int, const QString &, text, 
        bool, userDefined )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::numberGroupingSupported
// -----------------------------------------------------------------------------
//
bool SettingsWrapper::numberGroupingSupported(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::isFeatureCallWaitingDistiquishNotProvisionedEnabled
// -----------------------------------------------------------------------------
//
bool SettingsWrapper::isFeatureCallWaitingDistiquishNotProvisionedEnabled(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::isPhoneOffline
// -----------------------------------------------------------------------------
//
bool SettingsWrapper::isPhoneOffline(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::isOngoingCall
// -----------------------------------------------------------------------------
//
bool SettingsWrapper::isOngoingCall(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// SettingsWrapper::forbiddenIconSupported
// -----------------------------------------------------------------------------
//
bool SettingsWrapper::forbiddenIconSupported(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// Tools::voipSupported
// -----------------------------------------------------------------------------
//
bool Tools::voipSupported(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// Tools::videoSupported
// -----------------------------------------------------------------------------
//
bool Tools::videoSupported(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


