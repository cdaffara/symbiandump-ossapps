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
#include "hbaction.h"
#include <HbToolBarExtension>
#include <HbMenu>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbAction::HbAction
// -----------------------------------------------------------------------------
//
HbAction::HbAction( 
        QObject * parent )
    :QAction( parent ), d_ptr(0)
    {
    
    }


// -----------------------------------------------------------------------------
// HbAction::HbAction
// -----------------------------------------------------------------------------
//
HbAction::HbAction( 
        const QString & text,
        QObject * parent )
    :QAction( text, parent ), d_ptr(0)
    {
    
    }


// -----------------------------------------------------------------------------
// HbAction::HbAction
// -----------------------------------------------------------------------------
//
HbAction::HbAction( 
        const HbIcon & icon,
        const QString & text,
        QObject * parent )
    :QAction( parent ), d_ptr(0)
    {
    
    }


// -----------------------------------------------------------------------------
// HbAction::HbAction
// -----------------------------------------------------------------------------
//
HbAction::HbAction( 
        Hb::NavigationAction action,
        QObject * parent )
    :QAction( parent ), d_ptr(0)
    {
    
    }


// -----------------------------------------------------------------------------
// HbAction::~HbAction
// -----------------------------------------------------------------------------
//
HbAction::~HbAction(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAction::setIcon
// -----------------------------------------------------------------------------
//
void HbAction::setIcon( 
        const HbIcon & icon )
    {
    SMC_MOCK_METHOD1( void, const HbIcon &, icon )
    }


// -----------------------------------------------------------------------------
// HbAction::icon
// -----------------------------------------------------------------------------
//
HbIcon HbAction::icon(  ) const
    {
    SMC_MOCK_METHOD0( HbIcon )
    }


// -----------------------------------------------------------------------------
// HbAction::setMenu
// -----------------------------------------------------------------------------
//
void HbAction::setMenu( 
        HbMenu * menu )
    {
    SMC_MOCK_METHOD1( void, HbMenu *, menu )
    }


// -----------------------------------------------------------------------------
// HbAction::menu
// -----------------------------------------------------------------------------
//
HbMenu * HbAction::menu(  ) const
    {
    SMC_MOCK_METHOD0( HbMenu * )
    }


// -----------------------------------------------------------------------------
// HbAction::setToolTip
// -----------------------------------------------------------------------------
//
void HbAction::setToolTip( 
        const QString & tooltip )
    {
    SMC_MOCK_METHOD1( void, const QString &, tooltip )
    }


// -----------------------------------------------------------------------------
// HbAction::toolTip
// -----------------------------------------------------------------------------
//
QString HbAction::toolTip(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbAction::setToolBarExtension
// -----------------------------------------------------------------------------
//
void HbAction::setToolBarExtension( 
        HbToolBarExtension * extension )
    {
    SMC_MOCK_METHOD1( void, HbToolBarExtension *, extension )
    }


// -----------------------------------------------------------------------------
// HbAction::toolBarExtension
// -----------------------------------------------------------------------------
//
HbToolBarExtension * HbAction::toolBarExtension(  ) const
    {
    SMC_MOCK_METHOD0( HbToolBarExtension * )
    }


// -----------------------------------------------------------------------------
// HbAction::setCommandRole
// -----------------------------------------------------------------------------
//
void HbAction::setCommandRole( 
        CommandRole commandRole )
    {
    SMC_MOCK_METHOD1( void, CommandRole, commandRole )
    }


// -----------------------------------------------------------------------------
// HbAction::commandRole
// -----------------------------------------------------------------------------
//
HbAction::CommandRole HbAction::commandRole(  ) const
    {
    SMC_MOCK_METHOD0( HbAction::CommandRole )
    }


// -----------------------------------------------------------------------------
// HbAction::HbAction
// -----------------------------------------------------------------------------
//
HbAction::HbAction( 
        HbActionPrivate & dd,
        QObject * parent )
    :QAction( parent ), d_ptr(0)
    {
    
    }


