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
* Description:    Class definition that monitors tv window visibility
*
*/


#ifndef __GLXWINVISIBILITYMONITOR_H__
#define __GLXWINVISIBILITYMONITOR_H__

// External Includes
#include <e32base.h>                // for CBase
#include <AknWsEventObserver.h>     // for MAknWsEventObserver


// Forward Declarations
class MGlxWindowVisibilityObserver;       


/**
 * Class Description
 * An Active object derived class is used to monitor the visibility of the
 * TV out window.
 */        
NONSHARABLE_CLASS(CGlxWindowVisibilityMonitor) : public CBase, 
                                                 public MAknWsEventObserver
    {
public:
    /**
     * Static Symbian 2 stage constructor.
     */
    static CGlxWindowVisibilityMonitor* NewL(
                            MGlxWindowVisibilityObserver& aVisibilityObserver );
    
    /**
     * Destructor.
     */
    ~CGlxWindowVisibilityMonitor();
    
    
    /**
     * Close.
     */
    void Close();
    
private:

    /**
     * Standard C++ constructor
     */
    CGlxWindowVisibilityMonitor( 
                        MGlxWindowVisibilityObserver& aVisibilityObserver );
    
    /*
     * Symbian second stage construction
     */
    void ConstructL(); 

public: /// class member functions

    /**
     * Provides the caller with the current visible state
     * @return ETrue if the window is visible or false otherwise
     */
    TBool IsVisible() const;

public: // from MAknWsEventObserver

    void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );     

private: /// class member data
    /// Not ownded: The Akn event monitor
    CAknWsEventMonitor* iAknEventMonitor;
    
    /// Not owned: Window visibility observer
    MGlxWindowVisibilityObserver& iVisibilityObserver;

    /// The visible state 
    TBool iIsVisible; 
    };
       


#endif //  __GLXWINVISIBILITYMONITOR_H__
