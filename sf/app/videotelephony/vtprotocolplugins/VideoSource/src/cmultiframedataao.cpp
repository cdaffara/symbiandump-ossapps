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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES
#include "cmultiframedataao.h"
#include "mmultiframeprovider.h"
#include "cmultiframeprovider.h"
#include "VSPanic.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif
// ============================ CMultiframeDataAO ===============================

// -----------------------------------------------------------------------------
// CMultiframeDataAO::CMultiframeDataAO( CMultiframeProvider* aRequester )
// -----------------------------------------------------------------------------
//
CMultiframeDataAO::CMultiframeDataAO( CMultiframeProvider* aRequester )
: CTimer( EPriorityHigh ), iRequester( aRequester )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::CMultiframeDataAO() >>"), RThread().Id().operator TUint()));
    RThread me;
    iThreadId = me.Id();
    CActiveScheduler::Add( this );
    TRAP_IGNORE(CTimer::ConstructL());
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::CMultiframeDataAO() <<"), RThread().Id().operator TUint()));
    }
// -----------------------------------------------------------------------------
// CMultiframeDataAO::SetObserver( MMultiframeprovider* aObserever )
// -----------------------------------------------------------------------------
//
void CMultiframeDataAO::SetObserver( MMultiframeprovider* aObserver )
	  {
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::SetObserver() >>"), RThread().Id().operator TUint()));
	  iObserver = aObserver;
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::SetObserver() <<"), RThread().Id().operator TUint()));	
	  }
// -----------------------------------------------------------------------------
// CMultiframeDataAO::~CMultiframeDataAO()
// -----------------------------------------------------------------------------
//
CMultiframeDataAO::~CMultiframeDataAO()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::~CMultiframeDataAO() >>"), RThread().Id().operator TUint()));
    Cancel();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::~CMultiframeDataAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeDataAO::Activate( )
// -----------------------------------------------------------------------------
//
void CMultiframeDataAO::Activate( TTimeIntervalMicroSeconds aUpdateTime, TBool aInit, TBool aVFUpdate )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::Activate() >>"), RThread().Id().operator TUint()));
			
    TTimeIntervalMicroSeconds32 time = aUpdateTime.Int64();
    iInit = aInit;
    iVFUpdate = aVFUpdate;
    if ( !IsActive() )
    	  {
    	  CTimer::After( time );
  		  }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::Activate() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeDataAO::LocalBuffer( )
// -----------------------------------------------------------------------------
//
void CMultiframeDataAO::LocalBuffer( CVtImageBitmap* aLocalBuffer )
	  {
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::LocalBuffer() >>"), RThread().Id().operator TUint()));	
		iLocalBuffer.iBitmap = aLocalBuffer;
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::LocalBuffer() <<"), RThread().Id().operator TUint()));
	  }

// -----------------------------------------------------------------------------
// CMultiframeDataAO::RemoteBuffer( )
// -----------------------------------------------------------------------------
//
void CMultiframeDataAO::RemoteBuffer( CVtImageBitmap* aRemoteBuffer )
	  {
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::RemoteBuffer() >>"), RThread().Id().operator TUint()));		
		iRemoteBuffer.iBitmap = aRemoteBuffer;
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::RemoteBuffer() <<"), RThread().Id().operator TUint()));	
	  }
	
// -----------------------------------------------------------------------------
// CMultiframeDataAO::RunL()
// -----------------------------------------------------------------------------
//
void CMultiframeDataAO::RunL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::RunL() >>"), RThread().Id().operator TUint()));

    //stop loop ao
    if (iRequireStopping)
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::RunL() cancel by flag"), RThread().Id().operator TUint()));
        iRequireStopping = EFalse;
        return;
        }

    if ( !iVFUpdate )
        {
        iObserver->RefreshYUVData( iRemoteBuffer );
        }	
    iObserver->RefreshViewFinder( iLocalBuffer );
    iRequester->UseSecondVfBuffer();
    if ( iInit )
        {
        iObserver->InitializeReady(); 	
        }
    if ( !iVFUpdate )
        {	
        iRequester->AddBitmapToFreeQueueL();
        }
    if ( !iInit && !iVFUpdate )  
        {				
        iRequester->NextFrame();
        }			
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::RunL() <<"), RThread().Id().operator TUint()));	
    }

// -----------------------------------------------------------------------------
// CMultiframeDataAO::CancelTimeLoop()
// -----------------------------------------------------------------------------
//
void CMultiframeDataAO::CancelTimeLoop()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::CancelTimeLoop >>"), RThread().Id().operator TUint()));        
    if ( IsActive() )
        {
        if ( iStatus == KRequestPending )
            {
            RThread me;
            if ( me.Id() == iThreadId )
                {
                Cancel();
                iRequireStopping = EFalse;
                }
            else
                {
                iRequireStopping = ETrue;
                }
            }
        else
            {
            iRequireStopping = ETrue;
            }
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeDataAO::CancelTimeLoop <<"), RThread().Id().operator TUint()));                    
    }


//  End of File
