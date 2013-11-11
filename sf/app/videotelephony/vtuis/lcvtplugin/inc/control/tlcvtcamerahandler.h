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

#ifndef LCVTCAMERAHANDLER_H
#define LCVTCAMERAHANDLER_H

#include <lccameracontrol.h>
#include <lcbrightnesscontrol.h>
#include <lczoomcontrol.h>

class CLcVtSession;

class TVtCameraHandler :    
    public MLcCameraControl,
    public MLcBrightnessControl,
    public MLcZoomControl
    {   
    public: // From MLcCameraControl
        
        TInt LcCameraCountL();

        TInt CurrentLcCameraIndex();
        
        void ToggleLcCameraL();       

    public: // From MLcBrightnessControl
        
        TInt MinLcBrightnessL();

        TInt MaxLcBrightnessL();

        TInt LcBrightnessL();

        void SetLcBrightnessL( TInt aValue );
        
        void IncreaseLcBrightnessL();
        
        void DecreaseLcBrightnessL();
        
    public: // From MLcZoomControl
        
        TInt MinLcZoomL();

        TInt MaxLcZoomL();

        TInt LcZoomValueL();

        void SetLcZoomValueL( TInt aValue );
        
        void LcZoomInL();
        
        void LcZoomOutL();     
    
    public:
        
        TVtCameraHandler();
        
        void SetSession( CLcVtSession* aSession );        

    private:
    
        enum TCurrentCamera
            {
            ECameraNotAvailable = KErrNotFound,
            EBackCamera = 0, 
            EFrontCamera = 1
            };         
   
    private: // Data
        
        CLcVtSession* ivtSession; // Not owned
 
    };

#endif //LCVTCAMERAHANDLER_H
