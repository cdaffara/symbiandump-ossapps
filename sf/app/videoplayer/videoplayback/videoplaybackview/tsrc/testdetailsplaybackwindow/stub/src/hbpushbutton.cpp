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
* Description:  Implementation of HbPushButton
*
*/

// Version : %version:  2 %



#include <qgraphicswidget>

#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "hbpushbutton.h"

// -------------------------------------------------------------------------------------------------
// HbPushButton::HbPushButton
// -------------------------------------------------------------------------------------------------
//
HbPushButton::HbPushButton()
    : mVisible( false )
    , mEnabled( false )
    , mSelected( false )
    , mFrameDrawer( NULL )
{
    MPX_ENTER_EXIT(_L("HbPushButton::HbPushButton()"));

    mBackground = new QGraphicsWidget();
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::~HbPushButton
// -------------------------------------------------------------------------------------------------
//
HbPushButton::~HbPushButton()
{
    MPX_DEBUG(_L("HbPushButton::HbPushButton") );

    if ( mBackground )
    {
        delete mBackground;
        mBackground = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::setEnabled
// -------------------------------------------------------------------------------------------------
//
void HbPushButton::setEnabled( bool enabled )
{
    MPX_DEBUG(_L("HbPushButton::setEnabled %d"), enabled );

    mEnabled = enabled;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::isEnabled
// -------------------------------------------------------------------------------------------------
//
bool HbPushButton::isEnabled()
{
    MPX_DEBUG(_L("HbPushButton::isEnabled %d"), mEnabled );

    return mEnabled;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::setVisible
// -------------------------------------------------------------------------------------------------
//
void HbPushButton::setVisible( bool visible )
{
    MPX_DEBUG(_L("HbPushButton::setVisible %d"), visible );

    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::isVisible
// -------------------------------------------------------------------------------------------------
//
bool HbPushButton::isVisible()
{
    MPX_DEBUG(_L("HbPushButton::isVisible %d"), mVisible );

    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::setSelected
// -------------------------------------------------------------------------------------------------
//
void HbPushButton::setSelected( bool selected )
{
    MPX_DEBUG(_L("HbPushButton::setSelected %d"), selected );

    mSelected = selected;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::isSelected
// -------------------------------------------------------------------------------------------------
//
bool HbPushButton::isSelected()
{
    MPX_DEBUG(_L("HbPushButton::isSelected %d"), mSelected );

    return mSelected;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::primitive
// -------------------------------------------------------------------------------------------------
//
QGraphicsItem* HbPushButton::primitive( HbStyle::Primitive primitive ) const
{
    MPX_DEBUG(_L("HbPushButton::primitive") );

    Q_UNUSED( primitive );
    return mBackground;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::press
// -------------------------------------------------------------------------------------------------
//
void HbPushButton::press()
{
    MPX_DEBUG(_L("HbPushButton::press") );

    emit pressed();
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::release
// -------------------------------------------------------------------------------------------------
//
void HbPushButton::release()
{
    MPX_DEBUG(_L("HbPushButton::release") );

    emit released();
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::frameBackground
// -------------------------------------------------------------------------------------------------
//
HbFrameDrawer* HbPushButton::frameBackground()
{
    MPX_DEBUG(_L("HbPushButton::frameBackground") );

    return mFrameDrawer;
}

// -------------------------------------------------------------------------------------------------
// HbPushButton::setFrameBackground
// -------------------------------------------------------------------------------------------------
//
void HbPushButton::setFrameBackground( HbFrameDrawer *drawer )
{
    MPX_DEBUG(_L("HbPushButton::setFrameBackground") );

    mFrameDrawer = drawer;
}

// End of file
