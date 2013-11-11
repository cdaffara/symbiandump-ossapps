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
#include "sssettingswrapper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SsSettingsWrapper::SsSettingsWrapper
// -----------------------------------------------------------------------------
//
SsSettingsWrapper::SsSettingsWrapper( 
        QObject * parent ) : QObject( parent )
    {

    }

// -----------------------------------------------------------------------------
// SsSettingsWrapper::~SsSettingsWrapper
// -----------------------------------------------------------------------------
//
SsSettingsWrapper::~SsSettingsWrapper(  )
    {

    }

// -----------------------------------------------------------------------------
// SsSettingsWrapper::get
// -----------------------------------------------------------------------------
//
int SsSettingsWrapper::get( 
        SsSettingsWrapperSettings setting,
        int & value )
    {
    static int tmpValue = 0;
    if(tmpValue == 0){
        value = 0;
        tmpValue = 1;
    }else{
        value = 2;
        tmpValue = 0;
    }
    SMC_MOCK_METHOD2( int, SsSettingsWrapperSettings, setting, int&, value )
    }

// -----------------------------------------------------------------------------
// SsSettingsWrapper::set
// -----------------------------------------------------------------------------
//
int SsSettingsWrapper::set( 
        SsSettingsWrapperSettings setting,
        int value )
    {
    SMC_MOCK_METHOD2( int, SsSettingsWrapperSettings, setting, int, value )
    }


