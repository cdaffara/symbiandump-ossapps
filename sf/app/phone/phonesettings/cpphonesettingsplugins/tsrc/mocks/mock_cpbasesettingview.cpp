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
#include "cpbasesettingview.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpBaseSettingView::CpBaseSettingView
// -----------------------------------------------------------------------------
//
CpBaseSettingView::CpBaseSettingView( 
        QGraphicsWidget * ,
        QGraphicsItem *  )
    //:
    //HbView( /*settingForm, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// CpBaseSettingView::~CpBaseSettingView
// -----------------------------------------------------------------------------
//
CpBaseSettingView::~CpBaseSettingView(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CpBaseSettingView::close
// -----------------------------------------------------------------------------
//
void CpBaseSettingView::close(  )
    {
    SMC_MOCK_METHOD0( void )
    }


