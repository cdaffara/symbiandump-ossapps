/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server shutdown implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ShutdownServer.h"

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CShutdownServer* CShutdownServer::NewL()
  {
  CShutdownServer* self = new (ELeave) CShutdownServer;
  CleanupStack::PushL(self);
  self->ConstructL();
  CleanupStack::Pop(self);
  return self;  
  } // end NewL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CShutdownServer::CShutdownServer()
  :CTimer(-1)
  {
  __LOG_CONSTRUCT( "msg", "CShutdownServer" )
  CActiveScheduler::Add(this);
  } // end constructor
  
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CShutdownServer::~CShutdownServer()
    {
    __LOG_DESTRUCT
    } // end destructor
  
// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CShutdownServer::ConstructL()
  {
  CTimer::ConstructL();
  } // end ConstructL

// ==========================================================================
// FUNCTION: Start
// ==========================================================================
void CShutdownServer::Start( TTimeIntervalMicroSeconds32 aShutdownDelay )
    {
    __LOG_ENTER( "Start" )
    
    After( aShutdownDelay );
    
    __LOG_EXIT
    } // end Start

// ==========================================================================
// FUNCTION: Stop
// ==========================================================================
void CShutdownServer::Stop()
    {
    __LOG_ENTER( "Stop" )
    
    Cancel();    
    iShutdown = EFalse;
    
    __LOG_EXIT
    } // end Stop

// ==========================================================================
// FUNCTION: ShutDownNow
// ==========================================================================
void CShutdownServer::ShutDownNow()
    {
    __LOG_ENTER( "ShutDownNow" )
    
    // Stop the active scheduler, which shuts down the server.
    CActiveScheduler::Stop();        
    
    __LOG_EXIT
    } // end ShutDownNow

// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CShutdownServer::RunL()
    {
    __LOG_ENTER( "RunL" )
    if( iBackgroundOperationsInProgress )
        {
        // Wait until background operations are completed.
        __LOG_WRITE_INFO( "Background operations in progress" );
        iShutdown = ETrue;
        }
    else
        {    
        ShutDownNow();      
        } // end if
    __LOG_EXIT
    }  // end RunL   
  
// ==========================================================================
// FUNCTION: BackgroundOperationsInProgress
// ==========================================================================
void CShutdownServer::BackgroundOperationsInProgress()
    {
    __LOG_ENTER( "BackgroundOperationsInProgress" )
    
    iBackgroundOperationsInProgress = ETrue;
    
    __LOG_EXIT
    } // end BackgroundOperationsInProgress

// ==========================================================================
// FUNCTION: BackgroundOperationsCompleted
// ==========================================================================
void CShutdownServer::BackgroundOperationsCompleted()
    {
    __LOG_ENTER( "BackgroundOperationsCompleted" )
    
    if( iShutdown )
        {
        ShutDownNow();
        }
    else
        {        
        iBackgroundOperationsInProgress = EFalse;    
        } // end if
        
    __LOG_EXIT        
    } // end BackgroundOperationsCompleted
