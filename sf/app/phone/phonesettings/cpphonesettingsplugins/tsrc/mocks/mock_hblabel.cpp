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
#include <hblabel.h>

// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// HbLabel::HbLabel
// -----------------------------------------------------------------------------
//
HbLabel::HbLabel( 
        const QString & displayText,
        QGraphicsItem * parent )
    {

    }


// -----------------------------------------------------------------------------
// HbLabel::~HbLabel
// -----------------------------------------------------------------------------
//
HbLabel::~HbLabel(  )
    {

    }


// -----------------------------------------------------------------------------
// HbLabel::plainText
// -----------------------------------------------------------------------------
//
QString HbLabel::plainText(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbLabel::html
// -----------------------------------------------------------------------------
//
QString HbLabel::html(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbLabel::setElideMode
// -----------------------------------------------------------------------------
//
void HbLabel::setElideMode( 
        Qt::TextElideMode elideMode )
    {
   // SMC_MOCK_METHOD1( void, Qt::TextElideMode, elideMode )
    }


// -----------------------------------------------------------------------------
// HbLabel::elideMode
// -----------------------------------------------------------------------------
//
Qt::TextElideMode HbLabel::elideMode(  ) const
    {
   // SMC_MOCK_METHOD0( Qt::TextElideMode )
    }


// -----------------------------------------------------------------------------
// HbLabel::setTextWrapping
// -----------------------------------------------------------------------------
//
void HbLabel::setTextWrapping( 
        Hb::TextWrapping mode )
    {
  //  SMC_MOCK_METHOD1( void, Hb::TextWrapping, mode )
    }


// -----------------------------------------------------------------------------
// HbLabel::textWrapping
// -----------------------------------------------------------------------------
//
Hb::TextWrapping HbLabel::textWrapping(  ) const
    {
   // SMC_MOCK_METHOD0( Hb::TextWrapping )
    }


// -----------------------------------------------------------------------------
// HbLabel::setIcon
// -----------------------------------------------------------------------------
//
void HbLabel::setIcon( 
        const HbIcon & icon )
    {
  //  SMC_MOCK_METHOD1( void, const HbIcon &, icon )
    }


// -----------------------------------------------------------------------------
// HbLabel::icon
// -----------------------------------------------------------------------------
//
HbIcon HbLabel::icon(  ) const
    {
   // SMC_MOCK_METHOD0( HbIcon )
    }


// -----------------------------------------------------------------------------
// HbLabel::setAspectRatioMode
// -----------------------------------------------------------------------------
//
void HbLabel::setAspectRatioMode( 
        Qt::AspectRatioMode mode )
    {
    //SMC_MOCK_METHOD1( void, Qt::AspectRatioMode, mode )
    }


// -----------------------------------------------------------------------------
// HbLabel::aspectRatioMode
// -----------------------------------------------------------------------------
//
Qt::AspectRatioMode HbLabel::aspectRatioMode(  ) const
    {
   // SMC_MOCK_METHOD0( Qt::AspectRatioMode )
    }


// -----------------------------------------------------------------------------
// HbLabel::setAlignment
// -----------------------------------------------------------------------------
//
void HbLabel::setAlignment( 
        Qt::Alignment alignment )
    {
   // SMC_MOCK_METHOD1( void, Qt::Alignment, alignment )
    }


// -----------------------------------------------------------------------------
// HbLabel::alignment
// -----------------------------------------------------------------------------
//
Qt::Alignment HbLabel::alignment(  ) const
    {
   // SMC_MOCK_METHOD0( Qt::Alignment )
    }


// -----------------------------------------------------------------------------
// HbLabel::setTextColor
// -----------------------------------------------------------------------------
//
void HbLabel::setTextColor( 
        const QColor & textColor )
    {
   // SMC_MOCK_METHOD1( void, const QColor &, textColor )
    }


// -----------------------------------------------------------------------------
// HbLabel::textColor
// -----------------------------------------------------------------------------
//
QColor HbLabel::textColor(  ) const
    {
  //  SMC_MOCK_METHOD0( QColor )
    }


// -----------------------------------------------------------------------------
// HbLabel::isEmpty
// -----------------------------------------------------------------------------
//
bool HbLabel::isEmpty(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbLabel::primitive
// -----------------------------------------------------------------------------
//
QGraphicsItem * HbLabel::primitive( 
        HbStyle::Primitive primitive ) const
    {
 //   SMC_MOCK_METHOD1( QGraphicsItem *, HbStyle::Primitive, primitive )
    }


// -----------------------------------------------------------------------------
// HbLabel::type
// -----------------------------------------------------------------------------
//
int HbLabel::type(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbLabel::setPlainText
// -----------------------------------------------------------------------------
//
void HbLabel::setPlainText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbLabel::setHtml
// -----------------------------------------------------------------------------
//
void HbLabel::setHtml( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbLabel::setNumber
// -----------------------------------------------------------------------------
//
void HbLabel::setNumber( 
        int number )
    {
    SMC_MOCK_METHOD1( void, int, number )
    }


// -----------------------------------------------------------------------------
// HbLabel::setNumber
// -----------------------------------------------------------------------------
//
void HbLabel::setNumber( 
        qreal number )
    {
    SMC_MOCK_METHOD1( void, qreal, number )
    }


// -----------------------------------------------------------------------------
// HbLabel::updatePrimitives
// -----------------------------------------------------------------------------
//
void HbLabel::updatePrimitives(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbLabel::clear
// -----------------------------------------------------------------------------
//
void HbLabel::clear(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbLabel::HbLabel
// -----------------------------------------------------------------------------
//
HbLabel::HbLabel( 
        HbLabelPrivate & dd,
        QGraphicsItem * parent )
    {
    }


// -----------------------------------------------------------------------------
// HbLabel::initStyleOption
// -----------------------------------------------------------------------------
//
void HbLabel::initStyleOption( 
        HbStyleOptionLabel * option ) const
    {
//    SMC_MOCK_METHOD1( void, HbStyleOptionLabel *, option )
    }


