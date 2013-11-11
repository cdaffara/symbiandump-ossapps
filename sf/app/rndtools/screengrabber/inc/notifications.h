/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef NOTIFICATIONS_H_
#define NOTIFICATIONS_H_

#include <hbmessagebox.h> 
#include <hbdeviceprogressdialog.h>



class Notifications
    {
public:
    
    /**
     * shows about box
     */
    static void about();
    
    /**
     * shows image captured note 
     */
    static void imageCaptured();
    
    /**
     * shows video captured note 
     */
    static void videoCaptured();
    
    /** 
     * shows seguantial images captured note
     */
    static void sequantialImagesCaptured(int amount);
    
    /** 
     * shows error message 
     */
    static void error(const QString& errorMessage);
    
    /**
     * to shows progressbar
     */
    static HbDeviceProgressDialog* showProgressBar(int max);
    
    };



#endif // NOTIFICATIONS_H_
