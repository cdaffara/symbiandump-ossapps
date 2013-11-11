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

#include "hbtapgesture.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbTapGesture::HbTapGesture
// -----------------------------------------------------------------------------
//
HbTapGesture::HbTapGesture( 
        QObject * parent )
    :d_ptr(0)
    //QTapGesture( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbTapGesture::~HbTapGesture
// -----------------------------------------------------------------------------
//
HbTapGesture::~HbTapGesture(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbTapGesture::startPos
// -----------------------------------------------------------------------------
//
QPointF HbTapGesture::startPos(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// HbTapGesture::setStartPos
// -----------------------------------------------------------------------------
//
void HbTapGesture::setStartPos( 
        const QPointF & startPos )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, startPos )
    }


// -----------------------------------------------------------------------------
// HbTapGesture::sceneStartPos
// -----------------------------------------------------------------------------
//
QPointF HbTapGesture::sceneStartPos(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// HbTapGesture::setSceneStartPos
// -----------------------------------------------------------------------------
//
void HbTapGesture::setSceneStartPos( 
        const QPointF & startPos )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, startPos )
    }


// -----------------------------------------------------------------------------
// HbTapGesture::scenePosition
// -----------------------------------------------------------------------------
//
QPointF HbTapGesture::scenePosition(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// HbTapGesture::setScenePosition
// -----------------------------------------------------------------------------
//
void HbTapGesture::setScenePosition( 
        const QPointF & pos )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, pos )
    }


// -----------------------------------------------------------------------------
// HbTapGesture::tapStyleHint
// -----------------------------------------------------------------------------
//
HbTapGesture::TapStyleHint HbTapGesture::tapStyleHint(  ) const
    {
    SMC_MOCK_METHOD0( HbTapGesture::TapStyleHint )
    }


// -----------------------------------------------------------------------------
// HbTapGesture::HbTapGesture
// -----------------------------------------------------------------------------
//
HbTapGesture::HbTapGesture( 
        HbTapGesturePrivate & dd,
        QObject * parent )
    :d_ptr(0)
    //QTapGesture( /*dd, parent*/ )
    {
    
    }

