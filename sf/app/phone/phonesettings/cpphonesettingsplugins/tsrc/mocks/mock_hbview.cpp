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
#include <hbview.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbView::HbView
// -----------------------------------------------------------------------------
//
HbView::HbView( 
        QGraphicsItem * /*parent*/ )
    //:
    //HbWidget( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbView::~HbView
// -----------------------------------------------------------------------------
//
HbView::~HbView(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbView::title
// -----------------------------------------------------------------------------
//
QString HbView::title(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbView::icon
// -----------------------------------------------------------------------------
//
HbIcon HbView::icon(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbView::setIcon
// -----------------------------------------------------------------------------
//
void HbView::setIcon( 
        const HbIcon & )
    {
    }


// -----------------------------------------------------------------------------
// HbView::menu
// -----------------------------------------------------------------------------
//
HbMenu * HbView::menu(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbView::setMenu
// -----------------------------------------------------------------------------
//
void HbView::setMenu( 
        HbMenu * )
    {
    }


// -----------------------------------------------------------------------------
// HbView::takeMenu
// -----------------------------------------------------------------------------
//
HbMenu * HbView::takeMenu(  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::toolBar
// -----------------------------------------------------------------------------
//
HbToolBar * HbView::toolBar(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbView::setToolBar
// -----------------------------------------------------------------------------
//
void HbView::setToolBar( 
        HbToolBar *  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::takeToolBar
// -----------------------------------------------------------------------------
//
HbToolBar * HbView::takeToolBar(  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::dockWidget
// -----------------------------------------------------------------------------
//
HbDockWidget * HbView::dockWidget(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbView::setDockWidget
// -----------------------------------------------------------------------------
//
void HbView::setDockWidget( 
        HbDockWidget *  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::widget
// -----------------------------------------------------------------------------
//
QGraphicsWidget * HbView::widget(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbView::setWidget
// -----------------------------------------------------------------------------
//
void HbView::setWidget( 
        QGraphicsWidget *  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::takeWidget
// -----------------------------------------------------------------------------
//
QGraphicsWidget * HbView::takeWidget(  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::showItems
// -----------------------------------------------------------------------------
//
void HbView::showItems( 
        Hb::SceneItems  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::hideItems
// -----------------------------------------------------------------------------
//
void HbView::hideItems( 
        Hb::SceneItems  )
    {
    }


// -----------------------------------------------------------------------------
// HbView::visibleItems
// -----------------------------------------------------------------------------
//
Hb::SceneItems HbView::visibleItems(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbView::isItemVisible
// -----------------------------------------------------------------------------
//
bool HbView::isItemVisible( 
        Hb::SceneItem  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbView::setItemVisible
// -----------------------------------------------------------------------------
//
void HbView::setItemVisible( 
        Hb::SceneItem item,
        bool visible )
    {
    SMC_MOCK_METHOD2( void, Hb::SceneItem, item, 
        bool, visible )
    }


// -----------------------------------------------------------------------------
// HbView::isContentFullScreen
// -----------------------------------------------------------------------------
//
bool HbView::isContentFullScreen(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbView::viewFlags
// -----------------------------------------------------------------------------
//
HbView::HbViewFlags HbView::viewFlags(  ) const
    {
    // SMC_MOCK_METHOD0( HbViewFlags )
    }


// -----------------------------------------------------------------------------
// HbView::setTitleBarVisible
// -----------------------------------------------------------------------------
//
void HbView::setTitleBarVisible( 
        bool visible )
    {
    SMC_MOCK_METHOD1( void, bool, visible )
    }


// -----------------------------------------------------------------------------
// HbView::setStatusBarVisible
// -----------------------------------------------------------------------------
//
void HbView::setStatusBarVisible( 
        bool visible )
    {
    SMC_MOCK_METHOD1( void, bool, visible )
    }



// -----------------------------------------------------------------------------
// HbView::addAction
// -----------------------------------------------------------------------------
//
void HbView::addAction( 
        HbAction * action,
        ActionContainer preferredActionContainer )
    {
   // SMC_MOCK_METHOD2( void, HbAction *, action, 
     //   ActionContainer, preferredActionContainer )
    }


// -----------------------------------------------------------------------------
// HbView::navigationAction
// -----------------------------------------------------------------------------
//
HbAction * HbView::navigationAction(  ) const
    {
    SMC_MOCK_METHOD0( HbAction * )
    }


// -----------------------------------------------------------------------------
// HbView::setNavigationAction
// -----------------------------------------------------------------------------
//
void HbView::setNavigationAction( 
        HbAction * action )
    {
 //   SMC_MOCK_METHOD1( void, HbAction *, action )
    }


// -----------------------------------------------------------------------------
// HbView::setTitle
// -----------------------------------------------------------------------------
//
void HbView::setTitle( 
        const QString & title )
    {
    SMC_MOCK_METHOD1( void, const QString &, title )
    }


// -----------------------------------------------------------------------------
// HbView::setContentFullScreen
// -----------------------------------------------------------------------------
//
void HbView::setContentFullScreen( 
        bool enable )
    {
    SMC_MOCK_METHOD1( void, bool, enable )
    }


// -----------------------------------------------------------------------------
// HbView::setViewFlags
// -----------------------------------------------------------------------------
//
void HbView::setViewFlags( 
        HbViewFlags flags )
    {
  //  SMC_MOCK_METHOD1( void, HbViewFlags, flags )
    }


// -----------------------------------------------------------------------------
// HbView::titleChanged
// -----------------------------------------------------------------------------
//
void HbView::titleChanged( 
        const QString & title )
    {
    SMC_MOCK_METHOD1( void, const QString &, title )
    }


// -----------------------------------------------------------------------------
// HbView::iconChanged
// -----------------------------------------------------------------------------
//
void HbView::iconChanged( 
        const HbIcon & icon )
    {
  //  SMC_MOCK_METHOD1( void, const HbIcon &, icon )
    }


// -----------------------------------------------------------------------------
// HbView::toolBarChanged
// -----------------------------------------------------------------------------
//
void HbView::toolBarChanged(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbView::visibleItemsChanged
// -----------------------------------------------------------------------------
//
void HbView::visibleItemsChanged(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbView::contentFullScreenChanged
// -----------------------------------------------------------------------------
//
void HbView::contentFullScreenChanged(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbView::dockWidgetChanged
// -----------------------------------------------------------------------------
//
void HbView::dockWidgetChanged(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbView::HbView
// -----------------------------------------------------------------------------
//
HbView::HbView( 
        HbViewPrivate & dd,
        QGraphicsItem * parent )
    //:
    //HbWidget( /*dd, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbView::event
// -----------------------------------------------------------------------------
//
bool HbView::event( 
        QEvent * event )
    {
  //  SMC_MOCK_METHOD1( bool, QEvent *, event )
    }


