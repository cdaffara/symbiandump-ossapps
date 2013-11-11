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
#include "phoneaction.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PhoneAction::PhoneAction
// -----------------------------------------------------------------------------
//
PhoneAction::PhoneAction(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneAction::~PhoneAction
// -----------------------------------------------------------------------------
//
PhoneAction::~PhoneAction(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneAction::setText
// -----------------------------------------------------------------------------
//
void PhoneAction::setText( 
        QString text )
    {
    SMC_MOCK_METHOD1( void, QString, text )
    }


// -----------------------------------------------------------------------------
// PhoneAction::text
// -----------------------------------------------------------------------------
//
QString PhoneAction::text(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// PhoneAction::setCommand
// -----------------------------------------------------------------------------
//
void PhoneAction::setCommand( 
        int command )
    {
    SMC_MOCK_METHOD1( void, int, command )
    }


// -----------------------------------------------------------------------------
// PhoneAction::command
// -----------------------------------------------------------------------------
//
int PhoneAction::command(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// PhoneAction::icon
// -----------------------------------------------------------------------------
//
HbIcon PhoneAction::icon(  ) const
    {
    SMC_MOCK_METHOD0( HbIcon )
    }


// -----------------------------------------------------------------------------
// PhoneAction::setIcon
// -----------------------------------------------------------------------------
//
void PhoneAction::setIcon( 
        HbIcon icon )
    {
    SMC_MOCK_METHOD1( void, HbIcon, icon )
    }


// -----------------------------------------------------------------------------
// PhoneAction::setDisabled
// -----------------------------------------------------------------------------
//
void PhoneAction::setDisabled( 
        bool disabled )
    {
    SMC_MOCK_METHOD1( void, bool, disabled )
    }


// -----------------------------------------------------------------------------
// PhoneAction::isDisabled
// -----------------------------------------------------------------------------
//
bool PhoneAction::isDisabled(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// PhoneAction::setActionRole
// -----------------------------------------------------------------------------
//
void PhoneAction::setActionRole( 
        ActionRole role )
    {
    SMC_MOCK_METHOD1( void, ActionRole, role )
    }


// -----------------------------------------------------------------------------
// PhoneAction::actionRole
// -----------------------------------------------------------------------------
//
PhoneAction::ActionRole PhoneAction::actionRole(  ) const
    {
    SMC_MOCK_METHOD0( ActionRole )
    }


