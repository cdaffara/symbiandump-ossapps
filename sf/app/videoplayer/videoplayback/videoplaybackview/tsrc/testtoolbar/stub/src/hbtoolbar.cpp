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
* Description:  Implementation of HbToolBar
*
*/

// Version : %version:  1 %


#include <QGraphicsLinearLayout>

#include "mpxvideo_debug.h"
#include "hbtoolbar.h"
#include "hbtoolbutton.h"

// -------------------------------------------------------------------------------------------------
// HbToolBar::HbToolBar
// -------------------------------------------------------------------------------------------------
//
HbToolBar::HbToolBar()
    : mVisible( false )
    , mEnabled( false )
{
    MPX_ENTER_EXIT(_L("HbToolBar::HbToolBar()"));

    mLayout = new QGraphicsLinearLayout();
    setLayout( mLayout );
}

// -------------------------------------------------------------------------------------------------
// HbToolBar::~HbToolBar
// -------------------------------------------------------------------------------------------------
//
HbToolBar::~HbToolBar()
{
    MPX_DEBUG(_L("HbToolBar::HbToolBar") );
}

// -------------------------------------------------------------------------------------------------
// HbToolBar::setEnabled
// -------------------------------------------------------------------------------------------------
//
void HbToolBar::setEnabled( bool enabled )
{
    MPX_DEBUG(_L("HbToolBar::setEnabled %d"), enabled );

    mEnabled = enabled;
}

// -------------------------------------------------------------------------------------------------
// HbToolBar::setVisible
// -------------------------------------------------------------------------------------------------
//
void HbToolBar::setVisible( bool visible )
{
    MPX_DEBUG(_L("HbToolBar::setVisible %d"), visible );

    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// HbToolBar::setOrientation
// -------------------------------------------------------------------------------------------------
//
void HbToolBar::setOrientation( int orientation )
{
    MPX_DEBUG(_L("HbToolBar::setOrientation") );
    Q_UNUSED( orientation );
}

// -------------------------------------------------------------------------------------------------
// HbToolBar::clearActions
// -------------------------------------------------------------------------------------------------
//
void HbToolBar::clearActions()
{
    MPX_DEBUG(_L("HbToolBar::clearActions") );
}

// -------------------------------------------------------------------------------------------------
// HbToolBar::addAction
// -------------------------------------------------------------------------------------------------
//
void HbToolBar::addAction( HbAction *action )
{
    MPX_DEBUG(_L("HbToolBar::addAction") );
    Q_UNUSED( action );

    HbToolButton *button = new HbToolButton( action );
    mLayout->addItem( button );
}

// -------------------------------------------------------------------------------------------------
// HbToolBar::addAction
// -------------------------------------------------------------------------------------------------
//
QGraphicsLayout* HbToolBar::layout()
{
    MPX_DEBUG(_L("HbToolBar::layout") );

    return mLayout;
}

// End of file
