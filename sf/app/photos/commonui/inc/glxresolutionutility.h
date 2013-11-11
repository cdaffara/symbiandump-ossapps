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



#ifndef __CGLXRESOLUTIONUTILITY_H__
#define __CGLXRESOLUTIONUTILITY_H__

#include <e32cmn.h>                 // for RPointerArray
#include <e32base.h>

// forward declarations
class MGlxResolutionChangeObserver;
class MGlxResolutionSize;


/**
 * Class Description
 * Singleton containing resolution utility methods
 * @author Loughlin
 */        
NONSHARABLE_CLASS (CGlxResolutionUtility) : public CBase
    {
public:    
	/**
	 * Construction of singleton
	 * @return A pointer to the single instance of CGlxResolutionUtility 
	 */
    IMPORT_C static CGlxResolutionUtility* InstanceL();
    
    /**
	 * Destroys the renderer instance of CGlxResolutionUtility when the 
	 * reference count is zero
     */	
    IMPORT_C void Close();

    /**
	 * Get the screen size
	 * @return The screen size in POSS
     */	
    IMPORT_C TSize ScreenSize() const;

    /**
     * Get the lenght of in POSS
     * @param The lenght in pixels which is to be converted to POSS
     * @return The converted lenght & width in POSS
     */
    IMPORT_C TReal32 PixelsToPoss( const TReal32& aLenPixels ) const;

    /**
     * Get the lenght of in pixels
     * @param The length in POSS which is to be converted to pixels
     * @return The converted size in pixels
     */
    IMPORT_C TReal32 PossToPixels( const TReal32& aSzPoss ) const;

    /**
     * Add a resolution change observer
     * @param The observer that will recieve notification on resolution 
     *           change
     */
    IMPORT_C void AddObserverL( MGlxResolutionChangeObserver& aObserver );

    /**
     * Remove a resolution change observer
     * @param The observer that will no longer recieve notification on 
     *           resolution change
     */
    IMPORT_C void RemoveObserver( MGlxResolutionChangeObserver& aObserver );
   
public: // not exported

    /**
     * Receives notification of a change in the screen size
     * All resolution observers are notified of the resolution change
     * @param aScreenSize the new screen size
     */
    void SetScreenSize( TSize aScreenSize );
    
private: 

    /**
     * Symbian constructor - used by SingletonStore to create instance
     */
    static CGlxResolutionUtility* NewL();
    
    /**
     * C++ constructor
     */
    CGlxResolutionUtility();
    
    /**
     * Destructor
     */
    ~CGlxResolutionUtility();
 
private: // class member variables
 
    // Size of the screen
    TSize iScreenSize;
    // Screen Size aspect ratio - POSS is a unit of measurement 
    // where 1 == the longest screen axis
    TReal32 iOnePossToPixelRatio;
    // The ratio of pixels to poss - stored to save on division 
    TReal32 iPixelToPossRatio;
    // Own: The container for the event observers
	RPointerArray < MGlxResolutionChangeObserver > iObserverList;
    };

#endif //__CGLXRESOLUTIONUTILITY_H__
