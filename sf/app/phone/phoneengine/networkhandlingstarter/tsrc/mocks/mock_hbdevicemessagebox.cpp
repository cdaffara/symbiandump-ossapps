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
#include <hbdevicemessagebox.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbDeviceMessageBox::HbDeviceMessageBox
// -----------------------------------------------------------------------------
//
HbDeviceMessageBox::HbDeviceMessageBox( 
        HbMessageBox::MessageBoxType type,
        QObject * parent )
    //:
    //QObject( /*type, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::HbDeviceMessageBox
// -----------------------------------------------------------------------------
//
HbDeviceMessageBox::HbDeviceMessageBox( 
        const QString & text,
        HbMessageBox::MessageBoxType type,
        QObject * parent )
    //:
    //QObject( /*text, type, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::~HbDeviceMessageBox
// -----------------------------------------------------------------------------
//
HbDeviceMessageBox::~HbDeviceMessageBox(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::question
// -----------------------------------------------------------------------------
//
bool HbDeviceMessageBox::question( 
        const QString & text,
        const QString & acceptButtonText,
        const QString & rejectButtonText )
    {
    SMC_MOCK_METHOD3( bool, const QString &, text, 
        const QString &, acceptButtonText, 
        const QString &, rejectButtonText )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::information
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::information( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::warning
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::warning( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::show
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::show(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::update
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::update(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::close
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::close(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::exec
// -----------------------------------------------------------------------------
//
const QAction * HbDeviceMessageBox::exec(  )
    {
    SMC_MOCK_METHOD0( const QAction * )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::triggeredAction
// -----------------------------------------------------------------------------
//
const QAction * HbDeviceMessageBox::triggeredAction(  ) const
    {
    SMC_MOCK_METHOD0( const QAction * )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::isAcceptAction
// -----------------------------------------------------------------------------
//
bool HbDeviceMessageBox::isAcceptAction( 
        const QAction * action ) const
    {
    //SMC_MOCK_METHOD1( bool, const QAction *, action )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setMessageBoxType
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setMessageBoxType( 
        HbMessageBox::MessageBoxType type )
    {
    SMC_MOCK_METHOD1( void, HbMessageBox::MessageBoxType, type )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::messageBoxType
// -----------------------------------------------------------------------------
//
HbMessageBox::MessageBoxType HbDeviceMessageBox::messageBoxType(  ) const
    {
    SMC_MOCK_METHOD0( HbMessageBox::MessageBoxType )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setText
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::text
// -----------------------------------------------------------------------------
//
QString HbDeviceMessageBox::text(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setIconName
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setIconName( 
        const QString & iconName )
    {
    SMC_MOCK_METHOD1( void, const QString &, iconName )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::iconName
// -----------------------------------------------------------------------------
//
QString HbDeviceMessageBox::iconName(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setIconVisible
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setIconVisible( 
        bool visible )
    {
    SMC_MOCK_METHOD1( void, bool, visible )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::iconVisible
// -----------------------------------------------------------------------------
//
bool HbDeviceMessageBox::iconVisible(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setAnimationDefinition
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setAnimationDefinition( 
        QString & animationDefinition )
    {
    SMC_MOCK_METHOD1( void, QString &, animationDefinition )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::animationDefinition
// -----------------------------------------------------------------------------
//
QString HbDeviceMessageBox::animationDefinition(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setTimeout
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setTimeout( 
        int timeout )
    {
    SMC_MOCK_METHOD1( void, int, timeout )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setTimeout
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setTimeout( 
        HbPopup::DefaultTimeout timeout )
    {
    SMC_MOCK_METHOD1( void, HbPopup::DefaultTimeout, timeout )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::timeout
// -----------------------------------------------------------------------------
//
int HbDeviceMessageBox::timeout(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setDismissPolicy
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setDismissPolicy( 
        HbPopup::DismissPolicy dismissPolicy )
    {
    SMC_MOCK_METHOD1( void, HbPopup::DismissPolicy, dismissPolicy )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::dismissPolicy
// -----------------------------------------------------------------------------
//
HbPopup::DismissPolicy HbDeviceMessageBox::dismissPolicy(  ) const
    {
    SMC_MOCK_METHOD0( HbPopup::DismissPolicy )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::setAction
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::setAction( 
        QAction * action,
        ActionRole role )
    {
    //SMC_MOCK_METHOD2( void, QAction *, action, 
    //    ActionRole, role )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::action
// -----------------------------------------------------------------------------
//
QAction * HbDeviceMessageBox::action( 
        ActionRole role ) const
    {
    //SMC_MOCK_METHOD1( QAction *, ActionRole, role )
    }


// -----------------------------------------------------------------------------
// HbDeviceMessageBox::aboutToClose
// -----------------------------------------------------------------------------
//
void HbDeviceMessageBox::aboutToClose(  )
    {
    SMC_MOCK_METHOD0( void )
    }


