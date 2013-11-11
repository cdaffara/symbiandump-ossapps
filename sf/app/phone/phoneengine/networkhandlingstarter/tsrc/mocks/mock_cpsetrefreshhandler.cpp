/** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mock_cpsetrefreshhandler.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPSetRefreshHandlerMock::NewL
// -----------------------------------------------------------------------------
//
CPSetRefreshHandlerMock * CPSetRefreshHandlerMock::NewL(  )
    {
    SMC_MOCK_NEWL0( CPSetRefreshHandlerMock * )

    CPSetRefreshHandlerMock* self = new( ELeave ) CPSetRefreshHandlerMock;
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPSetRefreshHandlerMock::NotifyFileChangeL
// -----------------------------------------------------------------------------
//
void CPSetRefreshHandlerMock::NotifyFileChangeL( 
        MSSSettingsRefreshObserver & aObserver,
        TSatElementaryFiles aObservedFile,
        TSatRefreshType aObservedRefreshType )
    {
    SMC_MOCK_METHOD3( void, MSSSettingsRefreshObserver &, aObserver, 
        TSatElementaryFiles, aObservedFile, 
        TSatRefreshType, aObservedRefreshType )
    }


// -----------------------------------------------------------------------------
// CPSetRefreshHandlerMock::CancelNotify
// -----------------------------------------------------------------------------
//
void CPSetRefreshHandlerMock::CancelNotify(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPSetRefreshHandlerMock::ChangedCspSettings
// -----------------------------------------------------------------------------
//
TInt CPSetRefreshHandlerMock::ChangedCspSettings( 
        TPSetChangedCspSetting & aChangedCspSettings,
        TPSetChangedCspSetting & aNewValues )
    {
    SMC_MOCK_METHOD2( TInt, TPSetChangedCspSetting &, aChangedCspSettings, 
        TPSetChangedCspSetting &, aNewValues )
    }
// -----------------------------------------------------------------------------
// CPSetRefreshHandlerMock::ChangedCspSettings
// -----------------------------------------------------------------------------
//
CPSetRefreshHandlerMock::~CPSetRefreshHandlerMock()
    {
    }

