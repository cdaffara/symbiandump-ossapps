/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility class for managing the screen size
*
*/


/**
 * @internal reviewed 24/08/2007 by D Holland
 */

//  CLASS HEADER
#include "glxresolutionutility.h"
    
//  EXTERNAL INCLUDES
#include <AknUtils.h> // for AknLayoutUtils

//  INTERNAL INCLUDES
#include <glxsingletonstore.h>              // for singletonstore
#include "mglxresolutionchangeobserver.h"   // for MGlxResolutionChangeObserver

#include <glxpanic.h>

//-----------------------------------------------------------------------------
// CGlxResolutionUtility::InstanceL
//-----------------------------------------------------------------------------
//
EXPORT_C CGlxResolutionUtility* CGlxResolutionUtility::InstanceL()
    {
    GLX_LOG_INFO("CGlxResolutionUtility::InstanceL");
    CGlxResolutionUtility* self = CGlxSingletonStore::InstanceL( &NewL );
    return self;
    }

//-----------------------------------------------------------------------------
// CGlxResolutionUtility::Close
//-----------------------------------------------------------------------------
//
EXPORT_C void CGlxResolutionUtility::Close()
    {
    GLX_LOG_INFO("CGlxResolutionUtility::Close");
    CGlxSingletonStore::Close( this );
    }

//-----------------------------------------------------------------------------
// ScreenSize
//-----------------------------------------------------------------------------
//
EXPORT_C TSize CGlxResolutionUtility::ScreenSize() const
    {
    GLX_LOG_INFO("CGlxResolutionUtility::ScreenSize");
    return iScreenSize;
    }

//-----------------------------------------------------------------------------
// PixelsToPoss
//-----------------------------------------------------------------------------
//
EXPORT_C TReal32 CGlxResolutionUtility::PixelsToPoss(
    const TReal32& aPixelLen ) const
    {
    GLX_LOG_INFO("CGlxResolutionUtility::PixelsToPoss");    
    // Screen   = 240 : 320  
    // POSS     = .75 : 1
    // aPixelLen= 240
    // retVal   = .75
    return aPixelLen * iPixelToPossRatio;
    }

//-----------------------------------------------------------------------------
// PossToPixels
//-----------------------------------------------------------------------------
//
EXPORT_C TReal32 CGlxResolutionUtility::PossToPixels(
    const TReal32& aPossLen ) const
    {
    GLX_LOG_INFO("CGlxResolutionUtility::PossToPixels");    
    // Screen   = 240 : 320  
    // POSS     = .75 : 1
    // aLenPoss = .75
    // retVal   = 240
    return aPossLen * iOnePossToPixelRatio;
    }



//-----------------------------------------------------------------------------
// AddObserverL.
//-----------------------------------------------------------------------------
//
EXPORT_C void CGlxResolutionUtility::AddObserverL( 
                                    MGlxResolutionChangeObserver& aObserver )
	{
    GLX_LOG_INFO( "CGlxResolutionUtility::AddObserverL" );
    if( KErrNotFound == iObserverList.Find( &aObserver ) )
        {
    	iObserverList.AppendL( &aObserver );
        }
	}
	
	
    
//-----------------------------------------------------------------------------
// RemoveObserver
//-----------------------------------------------------------------------------
//
EXPORT_C void CGlxResolutionUtility::RemoveObserver(
                                MGlxResolutionChangeObserver& aObserver )
    {
    GLX_LOG_INFO("CGlxResolutionUtility::RemoveObserver");    
    
    TInt index = iObserverList.Find( &aObserver );
    if ( KErrNotFound != index )
        {
        iObserverList.Remove( index );
        }
    }
	
//-----------------------------------------------------------------------------
// HandleScreenSizeChanged
//-----------------------------------------------------------------------------
//	
void CGlxResolutionUtility::SetScreenSize( TSize aScreenSize )
    {
    GLX_LOG_INFO("CGlxResolutionUtility::HandleScreenSizeChanged");    

    if( iScreenSize != aScreenSize )
        {
        // "Percentage Of Screen Size". A unit of measurement, in which 1.0
        // is as long as the longer edge of the screen.
    	iOnePossToPixelRatio = 
    	    ( aScreenSize.iHeight > aScreenSize.iWidth ) ?
    	        aScreenSize.iHeight : aScreenSize.iWidth;
    	// calculate the multiplier
        iPixelToPossRatio = 1.0 / iOnePossToPixelRatio; 
        iScreenSize = aScreenSize;

        // Notify observers of a change in the screen resolution
        TInt ii = iObserverList.Count();
        while ( ii-- > 0 )
            {
            iObserverList[ ii ]->HandleResolutionChanged();
            }
        }
    }

//-----------------------------------------------------------------------------
// NewL
//-----------------------------------------------------------------------------
//	
CGlxResolutionUtility* CGlxResolutionUtility::NewL()
    {
    GLX_LOG_INFO("CGlxResolutionUtility::NewL");    
    CGlxResolutionUtility* self = new( ELeave ) CGlxResolutionUtility;
    return self;	
    }

//-----------------------------------------------------------------------------
// ~CGlxResolutionUtility
//-----------------------------------------------------------------------------
//	
CGlxResolutionUtility::CGlxResolutionUtility()
    {
    // set the initial screen size
    // ask avkon for the full screen size
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen ); 
    // set the initial size
    SetScreenSize( screen.Size() );
    }
    
//-----------------------------------------------------------------------------
// ~CGlxResolutionUtility
//-----------------------------------------------------------------------------
//	
CGlxResolutionUtility::~CGlxResolutionUtility()	
    {
    GLX_LOG_INFO("CGlxResolutionUtility::~CGlxResolutionUtility");    
   	// Close the observer array 
	iObserverList.Close();
    }
