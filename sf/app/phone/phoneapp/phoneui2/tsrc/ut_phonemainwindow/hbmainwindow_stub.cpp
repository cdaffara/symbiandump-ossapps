/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hbmainwindow.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbMainWindow::HbMainWindow
// -----------------------------------------------------------------------------
//
HbMainWindow::HbMainWindow( 
        QWidget * parent,
        Hb::WindowFlags windowFlags )
    : d_ptr(NULL)
    //QGraphicsView( /*parent, windowFlags*/ )
    {
    Q_UNUSED(parent)
    Q_UNUSED(windowFlags)
    }

// -----------------------------------------------------------------------------
// HbMainWindow::~HbMainWindow
// -----------------------------------------------------------------------------
//
HbMainWindow::~HbMainWindow(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbMainWindow::addView
// -----------------------------------------------------------------------------
//
HbView * HbMainWindow::addView( 
        QGraphicsWidget * widget )
    {
    Q_UNUSED(widget)
    }
// -----------------------------------------------------------------------------
// HbMainWindow::insertView
// -----------------------------------------------------------------------------
//
HbView * HbMainWindow::insertView( 
        int index,
        QGraphicsWidget * widget )
    {
	Q_UNUSED(index)
	Q_UNUSED(widget)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::removeView
// -----------------------------------------------------------------------------
//
void HbMainWindow::removeView( 
        QGraphicsWidget * widget )
    {
    Q_UNUSED(widget)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::views
// -----------------------------------------------------------------------------
//
QList <HbView * > HbMainWindow::views(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::currentView
// -----------------------------------------------------------------------------
//
HbView * HbMainWindow::currentView(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::setCurrentView
// -----------------------------------------------------------------------------
//
void HbMainWindow::setCurrentView( 
        HbView * view,
        bool animate,
        Hb::ViewSwitchFlags flags )
    {
    Q_UNUSED(view)
    Q_UNUSED(animate)
    Q_UNUSED(flags)
    }



// -----------------------------------------------------------------------------
// HbMainWindow::orientation
// -----------------------------------------------------------------------------
//
Qt::Orientation HbMainWindow::orientation(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::setOrientation
// -----------------------------------------------------------------------------
//
void HbMainWindow::setOrientation( 
        Qt::Orientation orientation,
        bool animate )
    {
    Q_UNUSED(orientation)
    Q_UNUSED(animate)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::unsetOrientation
// -----------------------------------------------------------------------------
//
void HbMainWindow::unsetOrientation( 
        bool animate )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::nativeBackgroundWindow
// -----------------------------------------------------------------------------
//
WId HbMainWindow::nativeBackgroundWindow(  )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::resetNativeBackgroundWindow
// -----------------------------------------------------------------------------
//
void HbMainWindow::resetNativeBackgroundWindow(  )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::layoutRect
// -----------------------------------------------------------------------------
//
QRectF HbMainWindow::layoutRect(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::setBackgroundImageName
// -----------------------------------------------------------------------------
//
void HbMainWindow::setBackgroundImageName( 
        Qt::Orientation orientation,
        const QString & name )
    {
	Q_UNUSED(orientation)
	Q_UNUSED(name)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::backgroundImageName
// -----------------------------------------------------------------------------
//
QString HbMainWindow::backgroundImageName( 
        Qt::Orientation orientation ) const
    {
	Q_UNUSED(orientation)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::setAutomaticOrientationEffectEnabled
// -----------------------------------------------------------------------------
//
void HbMainWindow::setAutomaticOrientationEffectEnabled( 
        bool enabled )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::automaticOrientationEffectEnabled
// -----------------------------------------------------------------------------
//
bool HbMainWindow::automaticOrientationEffectEnabled(  ) const
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::broadcastEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::broadcastEvent( 
        int eventType )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::viewReady
// -----------------------------------------------------------------------------
//
void HbMainWindow::viewReady(  )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::aboutToChangeView
// -----------------------------------------------------------------------------
//
void HbMainWindow::aboutToChangeView( 
        HbView * oldView,
        HbView * newView )
    {
	Q_UNUSED(oldView)
	Q_UNUSED(newView)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::currentViewChanged
// -----------------------------------------------------------------------------
//
void HbMainWindow::currentViewChanged( 
        HbView * view )
    {
    Q_UNUSED(view)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::aboutToChangeOrientation
// -----------------------------------------------------------------------------
//
void HbMainWindow::aboutToChangeOrientation(  )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::aboutToChangeOrientation
// -----------------------------------------------------------------------------
//
void HbMainWindow::aboutToChangeOrientation( 
        Qt::Orientation newOrientation,
        bool animated )
    {
    Q_UNUSED(newOrientation)
    Q_UNUSED(animated)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::orientationChanged
// -----------------------------------------------------------------------------
//
void HbMainWindow::orientationChanged( 
        Qt::Orientation orientation )
    {
    Q_UNUSED(orientation)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::changeEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::changeEvent( 
        QEvent * event )
    {
    Q_UNUSED(event)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::closeEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::closeEvent( 
        QCloseEvent * event )
    {
    Q_UNUSED(event)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::keyPressEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::keyPressEvent( 
        QKeyEvent * event )
    {
    Q_UNUSED(event)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::keyReleaseEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::keyReleaseEvent( 
        QKeyEvent * event )
    {
    Q_UNUSED(event)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::resizeEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::resizeEvent( 
        QResizeEvent * event )
    {
    Q_UNUSED(event)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::customEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::customEvent( 
        QEvent * event )
    {
    Q_UNUSED(event)
    }


// -----------------------------------------------------------------------------
// HbMainWindow::scrollContentsBy
// -----------------------------------------------------------------------------
//
void HbMainWindow::scrollContentsBy( 
        int dx,
        int dy )
    {
    }


// -----------------------------------------------------------------------------
// HbMainWindow::paintEvent
// -----------------------------------------------------------------------------
//
void HbMainWindow::paintEvent( 
        QPaintEvent * event )
    {
    Q_UNUSED(event)
    }
