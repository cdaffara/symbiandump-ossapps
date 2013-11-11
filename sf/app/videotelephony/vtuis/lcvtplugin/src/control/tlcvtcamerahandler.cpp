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
* Description:  LiveComm Videotelephony's Camera Handler
*
*/


#include "tlcvtcamerahandler.h"
#include "clcvtsession.h"
#include "tlcvtstates.h"
#include "lcvtplugincommand.h"
#include "lcvtutility.h"
#include "cvtengmodel.h"


TVtCameraHandler::TVtCameraHandler() :
ivtSession( NULL )    
    {
    }

// -----------------------------------------------------------------------------
// From MLcCameraControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::LcCameraCountL()
    {
    return 2;
    }

// -----------------------------------------------------------------------------
// From MLcCameraControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::CurrentLcCameraIndex()
    {    
    if(ivtSession->LcVtStates().MediaState().IsPrimaryCameraInUse())
        return EFrontCamera;
    
    return EBackCamera;
    }

// -----------------------------------------------------------------------------
// From MLcCameraControl
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::ToggleLcCameraL()
    {
    if(ivtSession->LcVtStates().MediaState().IsPrimaryCameraInUse())
        ivtSession->HandleCommandL(EPluginCmdUseSecondaryCamera);
    else
        ivtSession->HandleCommandL(EPluginCmdUsePrimaryCamera);
    }

// -----------------------------------------------------------------------------
// From MLcBrightnessControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::MinLcBrightnessL()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// From MLcBrightnessControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::MaxLcBrightnessL()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// From MLcBrightnessControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::LcBrightnessL()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// From MLcBrightnessControl
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::SetLcBrightnessL( TInt aValue )
    {
    }

// -----------------------------------------------------------------------------
// From MLcBrightnessControl
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::IncreaseLcBrightnessL()
    {
    }

// -----------------------------------------------------------------------------
// From MLcBrightnessControl
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::DecreaseLcBrightnessL()
    {
    }

// -----------------------------------------------------------------------------
// From MLcZoomControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::MinLcZoomL()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// From MLcZoomControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::MaxLcZoomL()
    {
    MVtEngMedia& media = ivtSession->Model().Media();
    
    if(!LcVtUtility::IsZoomAllowed(media))
        {
        __VTPRINT( DEBUG_GEN, "TVtCameraHandler.MaxLcZoomL ZoomNotAllow" )
        return 0;
        }
    
    TInt max( 0 );
    const TInt error( media.GetMaxZoomStep( max ) );
    max = ( error ? 0 : max );
    
    __VTPRINT2( DEBUG_GEN, "TVtCameraHandler.MaxLcZoomL value=%d", max )
    
    return max;
    }

// -----------------------------------------------------------------------------
// From MLcZoomControl
// -----------------------------------------------------------------------------
//
TInt TVtCameraHandler::LcZoomValueL()
    {
    MVtEngMedia& media = ivtSession->Model().Media();

    if(!LcVtUtility::IsZoomAllowed(media))
        {
        __VTPRINT( DEBUG_GEN, "TVtCameraHandler.LcZoomValueL ZoomNotAllow" )
        return 0;
        }
    
    TInt current( 0 );
    const TInt error( media.GetCurrentZoomStep( current ) );
    current = ( error ? 0 : current );    
    __VTPRINT2( DEBUG_GEN, "TVtCameraHandler.LcZoomValueL value=%d", 0 )
    return current;
    }

// -----------------------------------------------------------------------------
// From MLcZoomControl
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::SetLcZoomValueL( TInt aValue )
    {    
    MVtEngMedia& media = ivtSession->Model().Media();
    
    if(!LcVtUtility::IsZoomAllowed(media))
        {
        __VTPRINT( DEBUG_GEN, "TVtCameraHandler.SetLcZoomValueL ZoomNotAllow" )
        return;
        }
    
    if( aValue < 0 || aValue > MaxLcZoomL() )
        {
        __VTPRINT( DEBUG_GEN, "TVtCameraHandler.SetLcZoomValueL - Invalidate value" )
        return;
        }
    
    __VTPRINT2( DEBUG_GEN, "TVtCameraHandler.SetLcZoomValueL Call SetZoomFactorL(%d)" , aValue );
    ivtSession->SetZoomFactorL( aValue );
    
    }

// -----------------------------------------------------------------------------
// From MLcZoomControl
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::LcZoomInL()
    {
    MVtEngMedia& media = ivtSession->Model().Media();
    
    if(!LcVtUtility::IsZoomAllowed(media))
        {
        __VTPRINT( DEBUG_GEN, "TVtCameraHandler.LcZoomInL ZoomNotAllow" )
        return;
        }
    
    __VTPRINT( DEBUG_GEN, "TVtCameraHandler.LcZoomInL Call SetLcZoomValueL" )
    SetLcZoomValueL(LcZoomValueL() + 1);    
    }

// -----------------------------------------------------------------------------
// From MLcZoomControl
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::LcZoomOutL()
    {
    MVtEngMedia& media = ivtSession->Model().Media();
    
    if(!LcVtUtility::IsZoomAllowed(media))
        {
        __VTPRINT( DEBUG_GEN, "TVtCameraHandler.LcZoomOutL ZoomNotAllow" )
        return;
        }
    
    __VTPRINT( DEBUG_GEN, "TVtCameraHandler.LcZoomOutL Call SetLcZoomValueL" )
    SetLcZoomValueL(LcZoomValueL() - 1);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TVtCameraHandler::SetSession( CLcVtSession* aSession )
    {
    ivtSession = aSession;
    }


