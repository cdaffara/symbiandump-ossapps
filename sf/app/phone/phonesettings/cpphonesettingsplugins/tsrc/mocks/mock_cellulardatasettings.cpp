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
#include <cellulardatasettings.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CellularDataSettings::CellularDataSettings
// -----------------------------------------------------------------------------
//
CellularDataSettings::CellularDataSettings(  )
    //:
    //QObject( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// CellularDataSettings::~CellularDataSettings
// -----------------------------------------------------------------------------
//
CellularDataSettings::~CellularDataSettings(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CellularDataSettings::dataUseHome
// -----------------------------------------------------------------------------
//
int CellularDataSettings::dataUseHome(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// CellularDataSettings::setDataUseHome
// -----------------------------------------------------------------------------
//
void CellularDataSettings::setDataUseHome( 
        int value )
    {
    SMC_MOCK_METHOD1( void, int, value )
    }


// -----------------------------------------------------------------------------
// CellularDataSettings::dataUseRoam
// -----------------------------------------------------------------------------
//
int CellularDataSettings::dataUseRoam(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// CellularDataSettings::setDataUseRoam
// -----------------------------------------------------------------------------
//
void CellularDataSettings::setDataUseRoam( 
        int value )
    {
    SMC_MOCK_METHOD1( void, int, value )
    }


