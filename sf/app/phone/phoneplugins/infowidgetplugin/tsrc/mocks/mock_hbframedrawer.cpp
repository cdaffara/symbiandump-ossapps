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
#include <QBitmap>
#include <QPainter>
#include <QGraphicsItem>
#include <smcmockclassincludes.h>
#include <hbframedrawer.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbFrameDrawer::HbFrameDrawer
// -----------------------------------------------------------------------------
//
HbFrameDrawer::HbFrameDrawer( 
        bool cacheFlag )
    {
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::HbFrameDrawer
// -----------------------------------------------------------------------------
//
HbFrameDrawer::HbFrameDrawer( 
        const QString & frameGraphicsName,
        FrameType type,
        bool cacheFlag )
    {
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::~HbFrameDrawer
// -----------------------------------------------------------------------------
//
HbFrameDrawer::~HbFrameDrawer(  )
    {
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::isNull
// -----------------------------------------------------------------------------
//
bool HbFrameDrawer::isNull(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::frameGraphicsName
// -----------------------------------------------------------------------------
//
QString HbFrameDrawer::frameGraphicsName(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setFrameGraphicsName
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setFrameGraphicsName( 
        const QString & frameGraphicsName )
    {
    SMC_MOCK_METHOD1( void, const QString &, frameGraphicsName )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::frameType
// -----------------------------------------------------------------------------
//
HbFrameDrawer::FrameType HbFrameDrawer::frameType() const
    {
    SMC_MOCK_METHOD0( FrameType )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setFrameType
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setFrameType( 
        HbFrameDrawer::FrameType type )
    {
    SMC_MOCK_METHOD1( void, FrameType, type )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::borderWidths
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::borderWidths( 
        qreal & left,
        qreal & top,
        qreal & right,
        qreal & bottom ) const
    {
    SMC_MOCK_METHOD4( void, qreal &, left, 
        qreal &, top, 
        qreal &, right, 
        qreal &, bottom )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setBorderWidths
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setBorderWidths( 
        const qreal left,
        const qreal top,
        const qreal right,
        const qreal bottom )
    {
    SMC_MOCK_METHOD4( void, const qreal, left, 
        const qreal, top, 
        const qreal, right, 
        const qreal, bottom )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setBorderWidths
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setBorderWidths( 
        const qreal horizontal,
        const qreal vertical )
    {
    SMC_MOCK_METHOD2( void, const qreal, horizontal, 
        const qreal, vertical )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setBorderWidth
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setBorderWidth( 
        const qreal width )
    {
    SMC_MOCK_METHOD1( void, const qreal, width )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::fillWholeRect
// -----------------------------------------------------------------------------
//
bool HbFrameDrawer::fillWholeRect(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setFillWholeRect
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setFillWholeRect( 
        bool fill )
    {
    SMC_MOCK_METHOD1( void, bool, fill )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::mirroringMode
// -----------------------------------------------------------------------------
//
HbIcon::MirroringMode HbFrameDrawer::mirroringMode(  ) const
    {
    SMC_MOCK_METHOD0( HbIcon::MirroringMode )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setMirroringMode
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setMirroringMode( 
        HbIcon::MirroringMode mode )
    {
    SMC_MOCK_METHOD1( void, HbIcon::MirroringMode, mode )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::fileNameSuffixList
// -----------------------------------------------------------------------------
//
QStringList HbFrameDrawer::fileNameSuffixList(  ) const
    {
    SMC_MOCK_METHOD0( QStringList )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setFileNameSuffixList
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setFileNameSuffixList( 
        const QStringList & list )
    {
    SMC_MOCK_METHOD1( void, const QStringList &, list )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setMask
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setMask( 
        const QPixmap & mask )
    {
    SMC_MOCK_METHOD1( void, const QPixmap &, mask )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setMask
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setMask( 
        const QBitmap & mask )
    {
    SMC_MOCK_METHOD1( void, const QBitmap &, mask )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::mask
// -----------------------------------------------------------------------------
//
QPixmap HbFrameDrawer::mask(  ) const
    {
    SMC_MOCK_METHOD0( QPixmap )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::maskBitmap
// -----------------------------------------------------------------------------
//
QBitmap HbFrameDrawer::maskBitmap(  ) const
    {
    SMC_MOCK_METHOD0( QBitmap )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::frameSize
// -----------------------------------------------------------------------------
//
QSize HbFrameDrawer::frameSize(  ) const
    {
    SMC_MOCK_METHOD0( QSize )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::themeChanged
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::themeChanged(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setLayoutDirection
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setLayoutDirection( 
        Qt::LayoutDirection direction )
    {
    SMC_MOCK_METHOD1( void, Qt::LayoutDirection, direction )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setGraphicsItem
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setGraphicsItem( 
        QGraphicsItem * item )
    {
    SMC_MOCK_METHOD1( void, QGraphicsItem *, item )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::paint
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::paint( 
        QPainter * painter,
        const QRectF & rect ) const
    {
    SMC_MOCK_METHOD2( void, QPainter *, painter, 
        const QRectF &, rect )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::rect
// -----------------------------------------------------------------------------
//
QRectF HbFrameDrawer::rect(  ) const
    {
    SMC_MOCK_METHOD0( QRectF )
    }


// -----------------------------------------------------------------------------
// HbFrameDrawer::setRect
// -----------------------------------------------------------------------------
//
void HbFrameDrawer::setRect( 
        const QRectF & rect )
    {
    SMC_MOCK_METHOD1( void, const QRectF &, rect )
    }


