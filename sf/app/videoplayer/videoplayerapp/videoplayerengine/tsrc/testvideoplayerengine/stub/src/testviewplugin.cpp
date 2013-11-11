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
* Description:   Stub test class
*
*/

// Version : %version:  %

// INCLUDE FILES
#include "testviewplugin.h"
#include "mpxvideo_debug.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TestViewPlugin::TestViewPlugin()
    : mActive(false)
{
    MPX_DEBUG(_L("TestViewPlugin::TestViewPlugin()"));
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
TestViewPlugin::~TestViewPlugin()
{
    MPX_DEBUG(_L("TestViewPlugin::~TestViewPlugin()"));
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::createView()
{
    MPX_DEBUG(_L("TestViewPlugin::createView()"));
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::destroyView()
{
    MPX_DEBUG(_L("TestViewPlugin::destroyView()"));
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::activateView()
{
    MPX_DEBUG(_L("TestViewPlugin::activateView()"));
    mActive = true;
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
bool TestViewPlugin::activated()
{
    MPX_DEBUG(_L("TestViewPlugin::activateView() ret %d"), mActive);
    return mActive;
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::deactivateView()
{
    MPX_DEBUG(_L("TestViewPlugin::deactivateView()"));
    mActive = false;
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* TestViewPlugin::getView()
{
    MPX_DEBUG(_L("TestViewPlugin::getView()"));
    return 0;
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
MpxViewPlugin* TestViewPlugin::viewPlugin()
{
    MPX_DEBUG(_L("TestViewPlugin::viewPlugin()"));
    return this;
}

// ---------------------------------------------------------------------------
// Slot: Orientation change
// ---------------------------------------------------------------------------
//
void TestViewPlugin::orientationChange( Qt::Orientation orientation )
{
    MPX_DEBUG(_L("TestViewPlugin::orientationChange()"));
    Q_UNUSED( orientation );
}

// ---------------------------------------------------------------------------
// Slot: back
// ---------------------------------------------------------------------------
//
void TestViewPlugin::back()
{
    MPX_DEBUG(_L("TestViewPlugin::back()"));
}

// end of file
