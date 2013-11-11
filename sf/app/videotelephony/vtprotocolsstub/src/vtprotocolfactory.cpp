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
* Description:  Protocol factory implementation
*
*/

#include "mvtprotocolhandler.h"
#include "cdatasourceproxy.h"
#include "cdatasinkproxy.h"
#include "csessionhandler.h"
#include "caudioconfighandler.h"
#include "cvtuserinput.h"
#include "ccommserver.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif
// -----------------------------------------------------------------------------
// VTProtocolFactory::CreateSessionCommandL
// Create one instance. 
// -----------------------------------------------------------------------------
//
EXPORT_C MVtSessionCommand* VTProtocolFactory::CreateSessionCommandL(MVtProtocolHandler* aProtocolHandler, TBool aEnableProxy, TVt3G324MSupported& a3G324MSupported)
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateSessionCommandL<")));
    a3G324MSupported = EVt3G324MMissing;	
    TSessionHandler * sessionHandler = new (ELeave)TSessionHandler(aProtocolHandler);
    CleanupStack::PushL(sessionHandler);
    CSessionCommand* sessionCommand = new (ELeave)CSessionCommand(sessionHandler);
    CleanupStack::Pop();
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateSessionCommandL sessionCommand %d>"), sessionCommand));
    return sessionCommand;
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::DeleteSessionCommand
// This function allows the application to delete an instance of a terminal 
// and reclaim all allocated resources.
// -----------------------------------------------------------------------------
//
EXPORT_C  void VTProtocolFactory::DeleteSessionCommand( MVtSessionCommand* aSessionCommand )
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeleteSessionCommand<")));		
    CSessionCommand* sessionCommand = static_cast<CSessionCommand*>(aSessionCommand);
    delete aSessionCommand;
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeleteSessionCommand>")));	
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::CreateAudioSource
// Creates an instance of a DevSound audio data source.
// -----------------------------------------------------------------------------
//
EXPORT_C MVTAudioSource* VTProtocolFactory::CreateAudioSource()
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateAudioSource<")));
    TInt err( KErrNone );
    CAudioSourceProxy* audioSourceProxy = NULL;	
    TRAP (err, audioSourceProxy = new (ELeave) CAudioSourceProxy() );
    if (err != KErrNone)
        {
        return NULL;
        }    
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateAudioSource audioSourceProxy %d>"), audioSourceProxy));
    return audioSourceProxy;
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::DeletAudioSource
// Deletes an instance of a DevSound audio data source.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt VTProtocolFactory::DeletAudioSource(MVTAudioSource *aSource)
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeletAudioSource<")));	
    CAudioSourceProxy* audioSourceProxy = static_cast<CAudioSourceProxy*>(aSource);
    delete aSource;
   __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeletAudioSource>")));	
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::CreateAudioSink
// Creates an instance of a DevSound audio data sink.
// -----------------------------------------------------------------------------
//
EXPORT_C MVTAudioSink* VTProtocolFactory::CreateAudioSink()
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateAudioSink<")));
    TInt err( KErrNone );		
    CAudioSinkProxy* audioSinkProxy = NULL;
    TRAP(err, audioSinkProxy = new (ELeave) CAudioSinkProxy() );
    if (err != KErrNone)
        {
        return NULL;
        }
    
   __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateAudioSink audioSinkProxy %d>"), audioSinkProxy));		
    return audioSinkProxy;
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::DeleteAudioSink
// Deletes an instance of a DevSound audio data sink
// -----------------------------------------------------------------------------
//
EXPORT_C TInt VTProtocolFactory::DeleteAudioSink(MVTAudioSink *aSink)
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeleteAudioSink<")));			
    CAudioSinkProxy* audioSinkProxy = static_cast<CAudioSinkProxy*>(aSink);
    delete aSink;
     __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeleteAudioSink>")));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::CreateCommServerL
// Creates an instance of a comm server of a particular name, to be used to
// initialize the terminal.
// -----------------------------------------------------------------------------
//
EXPORT_C MCommServer* VTProtocolFactory::CreateCommServerL(const TDesC & aName, TBool aEnableBitReversal)
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateCommServerL<")));	
    CCommServer* commServer = new (ELeave)CCommServer();
    CleanupStack::PushL(commServer);
    CleanupStack::Pop();
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateCommServerL commServer %d>"), commServer));	
    return commServer;
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::DeleteCommServerL
// This function allows the application to delete an instance of a comm server
// and reclaim all allocated resources.
// -----------------------------------------------------------------------------
//
EXPORT_C void VTProtocolFactory::DeleteCommServerL(MCommServer* aCommServer)
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeleteCommServerL<")));		
    CCommServer* commServer = static_cast<CCommServer*>(aCommServer);
    delete aCommServer;
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::DeleteCommServerL>")));	
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::CreateUserInputIndication
// Create instance .
// -----------------------------------------------------------------------------
//
EXPORT_C MVTUserInput* VTProtocolFactory::CreateUserInputIndication(TUserInputType aUIIType, TUint8 aTone)
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::CreateUserInputIndication")));	
    if (aUIIType == EVtUiiDTMF)
        {
        CVTUserInputDtmf* uii = CVTUserInputDtmf::NewL(aTone);
        return uii;
        }
    else if (aUIIType == EVtUiiAlphaNumeric)
        {
        CVTUserInputAlphanumeric* uii = CVTUserInputAlphanumeric::NewL(aTone);
        return uii;
        }
    else
        {
        return NULL;
        }
    
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::DeleteUserInputIndication
// Delete instance.
// -----------------------------------------------------------------------------
//
EXPORT_C void VTProtocolFactory::DeleteUserInputIndication(MVTUserInput* aUII)
    {
    delete aUII;
    }

// -----------------------------------------------------------------------------
// VTProtocolFactory::GetAudioConfigCommandL
// Get one instance.
// -----------------------------------------------------------------------------
//
EXPORT_C MVtAudioConfigCommand* VTProtocolFactory::GetAudioConfigCommandL(MVTAudioSink* aAudioSink)
    {
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::GetAudioConfigCommandL<")));		
    CAudioSinkProxy* audioSinkProxy = static_cast<CAudioSinkProxy*>(aAudioSink);
    CAudioConfigCommand* audioConfigCommand = new (ELeave)CAudioConfigCommand();
    __IF_DEBUG(Print(_L("VTProto: VTProtocolFactory::GetAudioConfigCommandL audioConfigCommand %d>"), audioConfigCommand));	
    return audioConfigCommand;
    }

// end of file
