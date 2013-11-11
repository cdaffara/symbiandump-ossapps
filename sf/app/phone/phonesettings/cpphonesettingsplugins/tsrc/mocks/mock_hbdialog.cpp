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
#include <hbdialog.h>
#include <hbaction.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbDialog::HbDialog
// -----------------------------------------------------------------------------
//
HbDialog::HbDialog( 
        QGraphicsItem * parent )
    //:
    //HbPopup( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDialog::~HbDialog
// -----------------------------------------------------------------------------
//
HbDialog::~HbDialog(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDialog::headingWidget
// -----------------------------------------------------------------------------
//
QGraphicsWidget * HbDialog::headingWidget(  ) const
    {
    SMC_MOCK_METHOD0( QGraphicsWidget * )
    }


// -----------------------------------------------------------------------------
// HbDialog::setHeadingWidget
// -----------------------------------------------------------------------------
//
void HbDialog::setHeadingWidget( 
        QGraphicsWidget * headingWidget )
    {
    SMC_MOCK_METHOD1( void, QGraphicsWidget *, headingWidget )
    }


// -----------------------------------------------------------------------------
// HbDialog::contentWidget
// -----------------------------------------------------------------------------
//
QGraphicsWidget * HbDialog::contentWidget(  ) const
    {
    SMC_MOCK_METHOD0( QGraphicsWidget * )
    }


// -----------------------------------------------------------------------------
// HbDialog::setContentWidget
// -----------------------------------------------------------------------------
//
void HbDialog::setContentWidget( 
        QGraphicsWidget * contentWidget )
    {
    SMC_MOCK_METHOD1( void, QGraphicsWidget *, contentWidget )
    }


// -----------------------------------------------------------------------------
// HbDialog::primaryAction
// -----------------------------------------------------------------------------
//
HbAction * HbDialog::primaryAction(  ) const
    {
    SMC_MOCK_METHOD0( HbAction * )
    }


// -----------------------------------------------------------------------------
// HbDialog::setPrimaryAction
// -----------------------------------------------------------------------------
//
void HbDialog::setPrimaryAction( 
        HbAction * action )
    {
    //SMC_MOCK_METHOD1( void, HbAction *, action )
    }


// -----------------------------------------------------------------------------
// HbDialog::secondaryAction
// -----------------------------------------------------------------------------
//
HbAction * HbDialog::secondaryAction(  ) const
    {
 //   SMC_MOCK_METHOD0( HbAction * )
    }


// -----------------------------------------------------------------------------
// HbDialog::setSecondaryAction
// -----------------------------------------------------------------------------
//
void HbDialog::setSecondaryAction( 
        HbAction * action )
    {
  //  SMC_MOCK_METHOD1( void, HbAction *, action )
    }

// -----------------------------------------------------------------------------
// HbDialog::HbDialog
// -----------------------------------------------------------------------------
//
HbDialog::HbDialog( 
        HbDialogPrivate & dd,
        QGraphicsItem * parent )
    //:
    //HbPopup( /*dd, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDialog::focusInEvent
// -----------------------------------------------------------------------------
//
void HbDialog::focusInEvent( 
        QFocusEvent * event )
    {
  //  SMC_MOCK_METHOD1( void, QFocusEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbDialog::closeEvent
// -----------------------------------------------------------------------------
//
void HbDialog::closeEvent( 
        QCloseEvent * event )
    {
 //   SMC_MOCK_METHOD1( void, QCloseEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbDialog::changeEvent
// -----------------------------------------------------------------------------
//
void HbDialog::changeEvent( 
        QEvent * event )
    {
   // SMC_MOCK_METHOD1( void, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbDialog::event
// -----------------------------------------------------------------------------
//
bool HbDialog::event( 
        QEvent * event )
    {
  //  SMC_MOCK_METHOD1( bool, QEvent *, event )
    }

// -----------------------------------------------------------------------------
// HbDialog::open
// -----------------------------------------------------------------------------
//
void HbDialog::open( QObject* receiver, const char* member )
    {
        SMC_MOCK_METHOD2( void, QObject *, receiver, const char*, member )
    }

