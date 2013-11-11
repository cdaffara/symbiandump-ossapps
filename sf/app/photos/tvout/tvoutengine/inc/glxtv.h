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
* Description:    Handles & propogates UI state change notifications.
*
*/



/**
 * @internal reviewed 24/08/2007 by D Holland
 */

#ifndef __GLXTV_H__
#define __GLXTV_H__

// External Includes
#include <e32def.h>
#include <e32base.h>

// Internal Includes
#include <glxtvconstants.h>

// Forward Declarations
class MGlxTvObserver;
class CGlxWindowVisibilityMonitor;
class CGlxTvConnectionMonitor;
class CRepository;

// For window server visibility notifications 
class MGlxWindowVisibilityObserver
    {
public:
    /**
     * Handle changes to the application window visiblity
     * Called when window visible state changes
     * @param aChangeType The visibility change type
     */
    virtual void HandleWindowVisibilityChangedL( TTvChangeType aChangeType ) = 0;
    };
        
        
        
// For Tv connection notification
class MGlxTvConnectionObserver
    {
public:
    /**
     * Handle TV/HDMI/Headset connection state changes
     * @param aChangeType The connection change type
     */
    virtual void HandleTvConnectionStatusChangedL(TTvChangeType aChangeType) = 0;
    };
        
        
        
        
/**
 * Class Description
 * A class that handles and propagates UI change notifications
 */        
NONSHARABLE_CLASS (CGlxTv) : public CBase,
                             public MGlxWindowVisibilityObserver,
                             public MGlxTvConnectionObserver
    {
public:
    /**
     * Static Symbian 2 stage constructor.
     * @param a TvObserver
     */
    IMPORT_C static CGlxTv* NewL( MGlxTvObserver& aTvObserver );
    
    /**
     * Destructor.
     */
    IMPORT_C ~CGlxTv();
    
private:

    /**
     * Standard C++ constructor
     * @param a TvObserver     
     */
    CGlxTv(MGlxTvObserver& aTvObserver);
    
    /**
     * Symbian second stage construction
     */
    void ConstructL(); 
   
public: /// class public method   
   
    /**
     * Get the current screen size
     * @return the Screen size in pixels
     */
    IMPORT_C TSize ScreenSizeL() const;
   
    /**
     * Is the Analog TV Connected
     * @return ETrue if active, otherwise EFalse
     */
    IMPORT_C TBool IsConnected() const; 
    
    /**
     * Is the TV widescreen
     * @return ETrue if widescreen, otherwise EFalse
     */
    IMPORT_C TBool IsWidescreen() const; 
    
    /**
     * Is the HDMI TV Connected
     * @return ETrue if active, otherwise EFalse
     */
    IMPORT_C TBool IsHDMIConnected() const;
    
public: // from MGlxWindowVisibilityObserver
    /**
     * @ref MGlxWindowVisibilityObserver::HandleWindowVisibilityChangedL
     */
    void HandleWindowVisibilityChangedL( TTvChangeType aChangeType );
    
                                
public: // from MGlxTvConnectionObserver
    /**
     * @ref MGlxTvConnectionObserver::HandleTvConnectionStatusChangedL
     */
    void HandleTvConnectionStatusChangedL(TTvChangeType aChangeType);
    
private: // new private methods
    /**
     * Retrive the TV Display Aspect Ratio and TV Setting
     * Calculate the screen size from aspect ratio and TV type
     */    
    void CalcTvScreenSzL();

    /**
     * Start monitoring the environment for changes to the TV aspect ratio
     * and the Gallery window groups visiblity 
     */        
    void MonitorEnvironmentL();

    /**
     * Stop monitoring the environment for changes to the TV aspect ratio
     * and the Gallery window groups visiblity 
     */      
    void StopMonitoringEnvironment();
    
private: // class member data    
    /// Not Owned: The TV out observer 
    MGlxTvObserver& iTvObserver;

    /// Owned: The Window visibility monitor
    CGlxWindowVisibilityMonitor* iWindowVisibilityMonitor;

    /// Owned: The TV connection monitor
    CGlxTvConnectionMonitor* iTvConnectionMonitor;
 
    /// Size of the TV Out display size
    TSize iSzInPixels;
    
    /// Owned: The central repository where TV display ratio is held.
    CRepository* iCenRep;
    
    /// The implementation of the class, hidden from clients
	class CGlxTvOutCenRepMonitor;
	
	/// Owned: Monitoring class for Tv Ratio Values
	CGlxTvOutCenRepMonitor* iTvDisplayAspectRatioMonitor; 
	
	/// The TV aspect ratio
	TInt iAspectRatio;
    };
    
    
#endif // __GLXTV_H__
