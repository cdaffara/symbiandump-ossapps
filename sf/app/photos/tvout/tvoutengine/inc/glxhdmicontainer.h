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

#ifndef GLXHDMICONTAINER_H_
#define GLXHDMICONTAINER_H_

#include <coecntrl.h>
#include "glxhdmisurfaceupdater.h"

/**
 * Class Description
 * A class that creates a CCoeControl window for Screen 1 with screendevice
 * used 
 */        
class CGlxHdmiContainer : public CCoeControl, public MGlxGenCallback
    {
public:
    /*
     * NewLC 
     */
    static CGlxHdmiContainer* NewL(const TRect& aRect);
    
    /*
     * Destructor 
     */
    ~CGlxHdmiContainer();
    
    /*
     * Get window instance 
     */
    RWindow* GetWindow();

private:// from MGlxGenCallback
    void DoGenCallback();
    
private: // from CCoeControl
    void Draw();

private:
    /*
     * Ctor 
     */
    CGlxHdmiContainer(const TRect& aRect);

    /*
     * ConstructL()
     */
    void ConstructL();
    
    /*
     * Create window for HDMI
     * Create a screendevice  
     */
    void CreateHdmiWindowL();
    
private:
    TRect               iRect;
    RWsSession          iSession;
    RWindow             iWsWindow;
    CWsScreenDevice*    iScreenDevice;    
    RWindowGroup        iWsWindowGroup;
    CWindowGc*          iWindowGc;
    TInt                iWsWindowGroupID;
    };

#endif /* GLXHDMICONTAINER_H_ */
