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
#include "cmultiframeloopao.h"
#include "cmultiframeprovider.h"
#include "VSPanic.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif
// ============================ CMultiframeloopAO ===============================

// -----------------------------------------------------------------------------
// CMultiframeloopAO::CMultiframeloopAO( CMultiframeProvider* aRequester )
// -----------------------------------------------------------------------------
//
CMultiframeloopAO::CMultiframeloopAO( CMultiframeProvider* aRequester )
: CActive( EPriorityLow ), iRequester( aRequester ), iFreeBMCount(0), iRequireStopping(EFalse)
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::CMultiframeloopAO() >>"), RThread().Id().operator TUint()));
    RThread me;
    iThreadId = me.Id();
    CActiveScheduler::Add( this );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::CMultiframeloopAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeloopAO::~CMultiframeloopAO()
// -----------------------------------------------------------------------------
//
CMultiframeloopAO::~CMultiframeloopAO()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::~CMultiframeloopAO() >>"), RThread().Id().operator TUint()));
    Cancel();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::~CMultiframeloopAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeloopAO::Signal( )
// -----------------------------------------------------------------------------
//
void CMultiframeloopAO::Signal( )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::Signal() >>"), RThread().Id().operator TUint()));
    iRequester->iAnimation = ETrue;	
    if ( !IsActive() )
        {
        SetActive();
        TRequestStatus* pStatus = &iStatus;
        RThread me;
        if ( me.Id() == iThreadId )
            {
            User::RequestComplete( pStatus, KErrNone );
            }
        else
            {
            if ( me.Open( iThreadId ) == KErrNone )
                {
                *pStatus = KRequestPending;
                me.RequestComplete( pStatus, KErrNone );
                me.Close();
                }
            else
                {
                Panic( EVSPanicThreadOpenFailure );
                }
            }
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::Signal() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeloopAO::RunL()
// -----------------------------------------------------------------------------
//
void CMultiframeloopAO::RunL()
    {
    iRequester->WaitForConverting();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RunL() >>"), RThread().Id().operator TUint()));

    //stop loop ao
    if (iRequireStopping)
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RunL() CANCEL CALLED,stop by flag"), RThread().Id().operator TUint()));
        iRequireStopping = EFalse;
        iRequester->ReleaseForConverting();
        return;
        }

    // Cancel is handled in this way because
    if ( iStatus ==  KErrCancel)
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RunL() CANCEL CALLED IN PROTO THREAD STOP LOOPPING"), RThread().Id().operator TUint()));
        iRequester->ReleaseForConverting();
        return;	
        }

    //If free bitmaps proceed next frame
    if ( iFreeBMCount > 0 )
        {	
        ++iIndex;
        // Loop animation starting from first frame 				
        if ( iIndex > iFramecount - 1 )
            {
            iIndex = KFirstFrameIndx;	
            }	
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RunL() RunL index %d"), RThread().Id().operator TUint(), iIndex));	
        iRequester->ConvertAndScaleL( iIndex );					
        }
    //Wait until free bitmaps to proceed 		
    else
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RunL() NO FREE BMs"), RThread().Id().operator TUint()));
        iStatus = KRequestPending;
        SetActive();
        }	
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RunL() <<"), RThread().Id().operator TUint()));
    iRequester->ReleaseForConverting();
    }
// -----------------------------------------------------------------------------
// CMultiframeloopAO::RemoveFreeBitmaps()
// -----------------------------------------------------------------------------
//     
void CMultiframeloopAO::RemoveFreeBitmaps( TInt aCount )
	  {
		__IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RemoveFreeBitmaps() >>"), RThread().Id().operator TUint()));
		iFreeBMCount = iFreeBMCount - aCount;	
		__IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::RemoveFreeBitmaps() frames %d <<"), RThread().Id().operator TUint(), aCount));	
	  }

// -----------------------------------------------------------------------------
// CMultiframeloopAO::SetFreeBitmaps()
// -----------------------------------------------------------------------------
//     
void CMultiframeloopAO::SetFreeBitmaps( TInt aCount )
	  {
		__IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::SetFreeBitmaps() >>"), RThread().Id().operator TUint()));
		iFreeBMCount = iFreeBMCount + aCount;		
		if ( IsActive() && iStatus == KRequestPending)
				{
				TRequestStatus* pStatus = &iStatus;
  			User::RequestComplete( pStatus, KErrNone );		
				}
	
		__IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::SetFreeBitmaps() frames %d <<"), RThread().Id().operator TUint(), aCount));	
	  }	    
// -----------------------------------------------------------------------------
// CMultiframeloopAO::SetFrameCount()
// -----------------------------------------------------------------------------
//    
void CMultiframeloopAO::SetFrameCount( TInt aCount )
	  {
		__IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::SetFrameCount() >>"), RThread().Id().operator TUint()));	
		iFramecount = aCount;
		iIndex = 0;
		__IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::SetFrameCount() frames %d <<"), RThread().Id().operator TUint(), aCount));	
	  }

// -----------------------------------------------------------------------------
// CMultiframeloopAO::CancelDataLoop()
// -----------------------------------------------------------------------------
//
 void CMultiframeloopAO::CancelDataLoop()
     {
     __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::CancelDataLoop() >>"), RThread().Id().operator TUint()));		
     if ( IsActive() )
         {
         if ( iStatus == KRequestPending )
             {
             RThread me;
             if ( me.Id() == iThreadId )
                 {
                 Cancel();
                 }
             else
                 {
                 if ( me.Open( iThreadId ) == KErrNone )
                     {
                     TRequestStatus* pStatus = &iStatus;
                     *pStatus = KRequestPending;
                     me.RequestComplete( pStatus, KErrCancel );
                     me.Close();
                     }
                 else
                     {
                     Panic( EVSPanicThreadOpenFailure );
                     }
                 }
             }
         else
             {
             iRequireStopping = ETrue;
             }
         }
     __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::CancelDataLoop() <<"), RThread().Id().operator TUint()));					
     }

// -----------------------------------------------------------------------------
// CMultiframeloopAO::DoCancel()
// -----------------------------------------------------------------------------
//
void CMultiframeloopAO::DoCancel()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::DoCancel() >>"), RThread().Id().operator TUint()));
    if ( iStatus == KRequestPending )
    	  {
    	  TRequestStatus* pStatus = &iStatus;
    	  User::RequestComplete( pStatus, KErrCancel );
    	  }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeloopAO::DoCancel() <<"), RThread().Id().operator TUint()));
    }   

// End of File 
