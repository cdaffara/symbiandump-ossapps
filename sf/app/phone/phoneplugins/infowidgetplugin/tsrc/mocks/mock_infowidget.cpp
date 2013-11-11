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
#include <hbanchorlayout.h>
#include <hbwidget.h>
#include <hbiconitem.h>
#include <hbmarqueeitem.h>
#include <hbfontspec.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbcheckbox.h>
#include <hbpushbutton.h>
#include <QPainter>
#include <QPainterPath>
#include <QBrush>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include "infowidget.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// InfoWidget::InfoWidget
// -----------------------------------------------------------------------------
//
InfoWidget::InfoWidget( 
        QGraphicsItem * parent,
        Qt::WindowFlags flags )
    : HbWidget(parent, flags)
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidget::~InfoWidget
// -----------------------------------------------------------------------------
//
InfoWidget::~InfoWidget(  )
    {
    
    }


// -----------------------------------------------------------------------------
// InfoWidget::boundingRect
// -----------------------------------------------------------------------------
//
QRectF InfoWidget::boundingRect(  ) const
    {
    SMC_MOCK_METHOD0( QRectF )
    }


// -----------------------------------------------------------------------------
// InfoWidget::sizeHint
// -----------------------------------------------------------------------------
//
QSizeF InfoWidget::sizeHint( 
        Qt::SizeHint which,
        const QSizeF & constraint ) const
    {
    SMC_MOCK_METHOD2( QSizeF, Qt::SizeHint, which, 
        const QSizeF &, constraint )
    }


// -----------------------------------------------------------------------------
// InfoWidget::sizePolicy
// -----------------------------------------------------------------------------
//
QSizePolicy InfoWidget::sizePolicy(  ) const
    {
    SMC_MOCK_METHOD0( QSizePolicy )
    }


// -----------------------------------------------------------------------------
// InfoWidget::homeZoneDisplay
// -----------------------------------------------------------------------------
//
QString InfoWidget::homeZoneDisplay()
    {
    SMC_MOCK_METHOD0( QString );
    }


// -----------------------------------------------------------------------------
// InfoWidget::setHomeZoneDisplay
// -----------------------------------------------------------------------------
//
void InfoWidget::setHomeZoneDisplay( 
        QString value )
    {
    SMC_MOCK_METHOD1( void, QString, value )
    }


// -----------------------------------------------------------------------------
// InfoWidget::mcnDisplay
// -----------------------------------------------------------------------------
//
QString InfoWidget::mcnDisplay(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// InfoWidget::setMcnDisplay
// -----------------------------------------------------------------------------
//
void InfoWidget::setMcnDisplay( 
        QString value )
    {
    SMC_MOCK_METHOD1( void, QString, value )
    }


// -----------------------------------------------------------------------------
// InfoWidget::activeLineDisplay
// -----------------------------------------------------------------------------
//
QString InfoWidget::activeLineDisplay(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// InfoWidget::setActiveLineDisplay
// -----------------------------------------------------------------------------
//
void InfoWidget::setActiveLineDisplay( 
        QString value )
    {
    SMC_MOCK_METHOD1( void, QString, value )
    }


// -----------------------------------------------------------------------------
// InfoWidget::satDisplay
// -----------------------------------------------------------------------------
//
QString InfoWidget::satDisplay(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// InfoWidget::setSatDisplay
// -----------------------------------------------------------------------------
//
void InfoWidget::setSatDisplay( 
        QString value )
    {
    SMC_MOCK_METHOD1( void, QString, value )
    }


// -----------------------------------------------------------------------------
// InfoWidget::spnDisplay
// -----------------------------------------------------------------------------
//
QString InfoWidget::spnDisplay(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// InfoWidget::setSpnDisplay
// -----------------------------------------------------------------------------
//
void InfoWidget::setSpnDisplay( 
        QString value )
    {
    SMC_MOCK_METHOD1( void, QString, value )
    }


// -----------------------------------------------------------------------------
// InfoWidget::onInitialize
// -----------------------------------------------------------------------------
//
void InfoWidget::onInitialize(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::onUninitialize
// -----------------------------------------------------------------------------
//
void InfoWidget::onUninitialize(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::onShow
// -----------------------------------------------------------------------------
//
void InfoWidget::onShow(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::onHide
// -----------------------------------------------------------------------------
//
void InfoWidget::onHide(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::readModel
// -----------------------------------------------------------------------------
//
void InfoWidget::readModel(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::handleModelError
// -----------------------------------------------------------------------------
//
void InfoWidget::handleModelError( 
        int operation,
        int errorCode )
    {
    SMC_MOCK_METHOD2( void, int, operation, 
        int, errorCode )
    }


// -----------------------------------------------------------------------------
// InfoWidget::spnDisplaySettingChanged
// -----------------------------------------------------------------------------
//
void InfoWidget::spnDisplaySettingChanged( 
        int state )
    {
    SMC_MOCK_METHOD1( void, int, state )
    }


// -----------------------------------------------------------------------------
// InfoWidget::mcnDisplaySettingChanged
// -----------------------------------------------------------------------------
//
void InfoWidget::mcnDisplaySettingChanged( 
        int state )
    {
    SMC_MOCK_METHOD1( void, int, state )
    }


// -----------------------------------------------------------------------------
// InfoWidget::satDisplaySettingChanged
// -----------------------------------------------------------------------------
//
void InfoWidget::satDisplaySettingChanged( 
        int state )
    {
    SMC_MOCK_METHOD1( void, int, state )
    }


// -----------------------------------------------------------------------------
// InfoWidget::settingsEditingFinished
// -----------------------------------------------------------------------------
//
void InfoWidget::settingsEditingFinished(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::settingsEditingCancelled
// -----------------------------------------------------------------------------
//
void InfoWidget::settingsEditingCancelled(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::settingsValidationFailed
// -----------------------------------------------------------------------------
//
void InfoWidget::settingsValidationFailed(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::settingsDialogClosed
// -----------------------------------------------------------------------------
//
void InfoWidget::settingsDialogClosed( 
        HbAction *sss )
    {
    SMC_MOCK_METHOD1( void, HbAction *, sss )
    }


// -----------------------------------------------------------------------------
// InfoWidget::startMarquees
// -----------------------------------------------------------------------------
//
bool InfoWidget::startMarquees()
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// InfoWidget::stopMarquees
// -----------------------------------------------------------------------------
//
void InfoWidget::stopMarquees(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::marqueeNext
// -----------------------------------------------------------------------------
//
void InfoWidget::marqueeNext(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::gestureEvent
// -----------------------------------------------------------------------------
//
void InfoWidget::gestureEvent(QGestureEvent *event)
{
    SMC_MOCK_METHOD1( void, QGestureEvent *, event )
}


// -----------------------------------------------------------------------------
// InfoWidget::changeEvent
// -----------------------------------------------------------------------------
//
void InfoWidget::changeEvent( 
        QEvent * event )
    {
    SMC_MOCK_METHOD1( void, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// InfoWidget::timerEvent
// -----------------------------------------------------------------------------
//
void InfoWidget::timerEvent( 
        QTimerEvent * event )
    {
    SMC_MOCK_METHOD1( void, QTimerEvent *, event )
    }


// -----------------------------------------------------------------------------
// InfoWidget::updateInfoDisplay
// -----------------------------------------------------------------------------
//
void InfoWidget::updateInfoDisplay(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// InfoWidget::readPersistentPreferences
// -----------------------------------------------------------------------------
//
bool InfoWidget::readPersistentPreferences(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// InfoWidget::initializeCheckBoxStates
// -----------------------------------------------------------------------------
//
void InfoWidget::initializeCheckBoxStates(  )
    {
    SMC_MOCK_METHOD0( void )
    }

/*
// -----------------------------------------------------------------------------
// InfoWidget::installTranslator
// -----------------------------------------------------------------------------
//
bool InfoWidget::installTranslator( 
        QString translationFile )
    {
    SMC_MOCK_METHOD1( bool, QString, translationFile )
    }


// -----------------------------------------------------------------------------
// InfoWidget::removeTranslators
// -----------------------------------------------------------------------------
//
void InfoWidget::removeTranslators(  )
    {
    SMC_MOCK_METHOD0( void )
    }
*/

