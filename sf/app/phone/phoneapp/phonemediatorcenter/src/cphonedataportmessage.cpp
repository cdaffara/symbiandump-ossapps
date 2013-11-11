/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneDataPortMessage class.
*
*/


// INCLUDE FILES
#include <videotelcontrolmediatorapi.h>
#include <MediatorDomainUIDs.h>
#include "phonelogger.h"
#include "cphonedataportmessage.h"
#include "cphonemediatorsender.h"
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneDataPortMessage::CPhoneDataPortMessage
// ---------------------------------------------------------
//
CPhoneDataPortMessage::CPhoneDataPortMessage( MPEEngineInfo& aEngineInfo ) :
    iEngineInfo( aEngineInfo )
    {
    }

// ---------------------------------------------------------
// CPhoneDataPortMessage::ConstructL
// ---------------------------------------------------------
//
void CPhoneDataPortMessage::ConstructL()
    {
    }

// ---------------------------------------------------------
// CPhoneDataPortMessage::NewL
// ---------------------------------------------------------
//
CPhoneDataPortMessage* CPhoneDataPortMessage::NewL( MPEEngineInfo& aEngineInfo )
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneDataPortMessage::NewL( ) ");
    CPhoneDataPortMessage* self = new( ELeave ) CPhoneDataPortMessage( aEngineInfo );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CPhoneDataPortMessage::~CPhoneDataPortMessage
// ---------------------------------------------------------
//
CPhoneDataPortMessage::~CPhoneDataPortMessage()
    {
    }

// ---------------------------------------------------------
// CPhoneDataPortMessage::ExecuteL
// ---------------------------------------------------------
//
void CPhoneDataPortMessage::ExecuteL()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneDataPortMessage::ExecuteL( ) ");
    const TName& dataPortName = iEngineInfo.DataPortName();
    TDataPortPackage dataPortPackage( dataPortName );
    CPhoneMediatorSender::Instance()->IssueCommand( KMediatorVideoTelephonyDomain,
                                                    KCatPhoneToVideotelCommands,
                                                    EVtCmdUseDataport,
                                                    TVersion( KPhoneToVideotelCmdVersionMajor,
                                                              KPhoneToVideotelCmdVersionMinor,
                                                              KPhoneToVideotelCmdVersionBuild ),
                                                    dataPortPackage );
    }

//  End of File  
