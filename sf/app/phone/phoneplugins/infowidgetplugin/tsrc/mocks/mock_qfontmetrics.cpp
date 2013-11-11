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
#include <QFontMetrics>
#include <QChar>

// Stub class implementation, needed in QFontMetrics header
class QFontPrivate {
    QFontPrivate(){}
    ~QFontPrivate(){}
}; 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QFontMetrics::QFontMetrics
// -----------------------------------------------------------------------------
//
QFontMetrics::QFontMetrics( 
        const QFont & )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetrics::QFontMetrics
// -----------------------------------------------------------------------------
//
QFontMetrics::QFontMetrics( 
        const QFont &,
        QPaintDevice * pd )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetrics::QFontMetrics
// -----------------------------------------------------------------------------
//
QFontMetrics::QFontMetrics( 
        const QFontMetrics & )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetrics::~QFontMetrics
// -----------------------------------------------------------------------------
//
QFontMetrics::~QFontMetrics(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetrics::ascent
// -----------------------------------------------------------------------------
//
int QFontMetrics::ascent(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::descent
// -----------------------------------------------------------------------------
//
int QFontMetrics::descent(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::height
// -----------------------------------------------------------------------------
//
int QFontMetrics::height(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::leading
// -----------------------------------------------------------------------------
//
int QFontMetrics::leading(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::lineSpacing
// -----------------------------------------------------------------------------
//
int QFontMetrics::lineSpacing(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::minLeftBearing
// -----------------------------------------------------------------------------
//
int QFontMetrics::minLeftBearing(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::minRightBearing
// -----------------------------------------------------------------------------
//
int QFontMetrics::minRightBearing(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::maxWidth
// -----------------------------------------------------------------------------
//
int QFontMetrics::maxWidth(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::xHeight
// -----------------------------------------------------------------------------
//
int QFontMetrics::xHeight(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::averageCharWidth
// -----------------------------------------------------------------------------
//
int QFontMetrics::averageCharWidth(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::inFont
// -----------------------------------------------------------------------------
//
bool QFontMetrics::inFont( QChar ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::leftBearing
// -----------------------------------------------------------------------------
//
int QFontMetrics::leftBearing( QChar ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::rightBearing
// -----------------------------------------------------------------------------
//
int QFontMetrics::rightBearing( QChar ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::width
// -----------------------------------------------------------------------------
//
int QFontMetrics::width( 
        const QString & string,
        int len ) const
    {
    SMC_MOCK_METHOD2( int, const QString &, string, 
        int, len )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::width
// -----------------------------------------------------------------------------
//
int QFontMetrics::width(QChar) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::charWidth
// -----------------------------------------------------------------------------
//
int QFontMetrics::charWidth( 
        const QString & str,
        int pos ) const
    {
    SMC_MOCK_METHOD2( int, const QString &, str, 
        int, pos )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::boundingRect
// -----------------------------------------------------------------------------
//
QRect QFontMetrics::boundingRect( QChar ) const
    {
    SMC_MOCK_METHOD0( QRect )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::boundingRect
// -----------------------------------------------------------------------------
//
QRect QFontMetrics::boundingRect( 
        const QString & text ) const
    {
    SMC_MOCK_METHOD1( QRect, const QString &, text )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::boundingRect
// -----------------------------------------------------------------------------
//
QRect QFontMetrics::boundingRect( 
        const QRect & r,
        int flags,
        const QString & text,
        int tabstops,
        int * tabarray ) const
    {
    SMC_MOCK_METHOD5( QRect, const QRect &, r, 
        int, flags, 
        const QString &, text, 
        int, tabstops, 
        int *, tabarray )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::size
// -----------------------------------------------------------------------------
//
QSize QFontMetrics::size( 
        int flags,
        const QString & str,
        int tabstops,
        int * tabarray ) const
    {
    SMC_MOCK_METHOD4( QSize, int, flags, 
        const QString &, str, 
        int, tabstops, 
        int *, tabarray )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::tightBoundingRect
// -----------------------------------------------------------------------------
//
QRect QFontMetrics::tightBoundingRect( 
        const QString & text ) const
    {
    SMC_MOCK_METHOD1( QRect, const QString &, text )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::elidedText
// -----------------------------------------------------------------------------
//
QString QFontMetrics::elidedText( 
        const QString & text,
        Qt::TextElideMode mode,
        int width,
        int flags ) const
    {
    SMC_MOCK_METHOD4( QString, const QString &, text, 
        Qt::TextElideMode, mode, 
        int, width, 
        int, flags )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::underlinePos
// -----------------------------------------------------------------------------
//
int QFontMetrics::underlinePos(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::overlinePos
// -----------------------------------------------------------------------------
//
int QFontMetrics::overlinePos(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::strikeOutPos
// -----------------------------------------------------------------------------
//
int QFontMetrics::strikeOutPos(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetrics::lineWidth
// -----------------------------------------------------------------------------
//
int QFontMetrics::lineWidth(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::QFontMetricsF
// -----------------------------------------------------------------------------
//
QFontMetricsF::QFontMetricsF( 
        const QFont & )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::QFontMetricsF
// -----------------------------------------------------------------------------
//
QFontMetricsF::QFontMetricsF( 
        const QFont &,
        QPaintDevice * pd )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::QFontMetricsF
// -----------------------------------------------------------------------------
//
QFontMetricsF::QFontMetricsF( 
        const QFontMetrics & )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::QFontMetricsF
// -----------------------------------------------------------------------------
//
QFontMetricsF::QFontMetricsF( 
        const QFontMetricsF & )
    {
    
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::~QFontMetricsF
// -----------------------------------------------------------------------------
//
QFontMetricsF::~QFontMetricsF(  )
    {
    
    }

// -----------------------------------------------------------------------------
// QFontMetricsF::ascent
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::ascent(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::descent
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::descent(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::height
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::height(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::leading
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::leading(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::lineSpacing
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::lineSpacing(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::minLeftBearing
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::minLeftBearing(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::minRightBearing
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::minRightBearing(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::maxWidth
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::maxWidth(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::xHeight
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::xHeight(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::averageCharWidth
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::averageCharWidth(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::inFont
// -----------------------------------------------------------------------------
//
//bool QFontMetricsF::inFont( QChar ) const
//    {
    //SMC_MOCK_METHOD0( bool )
//    }


// -----------------------------------------------------------------------------
// QFontMetricsF::leftBearing
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::leftBearing( QChar ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::rightBearing
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::rightBearing( QChar ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::width
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::width( 
        const QString & string ) const
    {
    SMC_MOCK_METHOD1( qreal, const QString &, string )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::width
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::width( QChar ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::boundingRect
// -----------------------------------------------------------------------------
//
QRectF QFontMetricsF::boundingRect( 
        const QString & string ) const
    {
    SMC_MOCK_METHOD1( QRectF, const QString &, string )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::boundingRect
// -----------------------------------------------------------------------------
//
QRectF QFontMetricsF::boundingRect( QChar ) const
    {
    SMC_MOCK_METHOD0( QRectF )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::boundingRect
// -----------------------------------------------------------------------------
//
QRectF QFontMetricsF::boundingRect( 
        const QRectF & r,
        int flags,
        const QString & string,
        int tabstops,
        int * tabarray ) const
    {
    SMC_MOCK_METHOD5( QRectF, const QRectF &, r, 
        int, flags, 
        const QString &, string, 
        int, tabstops, 
        int *, tabarray )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::size
// -----------------------------------------------------------------------------
//
QSizeF QFontMetricsF::size( 
        int flags,
        const QString & str,
        int tabstops,
        int * tabarray ) const
    {
    SMC_MOCK_METHOD4( QSizeF, int, flags, 
        const QString &, str, 
        int, tabstops, 
        int *, tabarray )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::tightBoundingRect
// -----------------------------------------------------------------------------
//
QRectF QFontMetricsF::tightBoundingRect( 
        const QString & text ) const
    {
    SMC_MOCK_METHOD1( QRectF, const QString &, text )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::elidedText
// -----------------------------------------------------------------------------
//
QString QFontMetricsF::elidedText( 
        const QString & text,
        Qt::TextElideMode mode,
        qreal width,
        int flags ) const
    {
    SMC_MOCK_METHOD4( QString, const QString &, text, 
        Qt::TextElideMode, mode, 
        qreal, width, 
        int, flags )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::underlinePos
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::underlinePos(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::overlinePos
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::overlinePos(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::strikeOutPos
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::strikeOutPos(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QFontMetricsF::lineWidth
// -----------------------------------------------------------------------------
//
qreal QFontMetricsF::lineWidth(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


