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
* Description:    Header of private wrapper class which speaks to HDMI and
*               also observes TVout connections
*
*/
#ifndef GLXTVOUTWRAPPERPRIVATE_P_H
#define GLXTVOUTWRAPPERPRIVATE_P_H

#include <QAbstractItemModel>
#include <fbs.h>
#include <QMetaType>
#include <QSize>

class GlxTvOutWrapper;
class CGlxConnectionObserver;
class CGlxHdmiController;

// Converting the QVariant to CFbsBitmap*
Q_DECLARE_METATYPE(CFbsBitmap*)

/**
 * Class Description This is Private Wrapper class for HDMI
 * 
 * Client shouldnt access this directly, instead access through GlxTvOutWrapper 
 * 
 */

class GlxTvOutWrapperPrivate
    {
public:
    /**
     * Static method to create an instance
     * @param aTvOutWrapper GlxTvOutWrapper Instance 
     * @param aModel item model
     * @param aScreenSize screensize of the phone
     * 
     * @return GlxTvOutWrapperPrivate Instance
     */
    static GlxTvOutWrapperPrivate* Instance(GlxTvOutWrapper* aTvOutWrapper,
            QAbstractItemModel* aModel,QSize aScreenSize);

    /**
     * destructor
     */
    ~GlxTvOutWrapperPrivate();

    /**
     *  HandleConnectionChange
     *  @param aConnected true if connected, false if disconnected
     *  Note this is only being done for HDMI as there is no 
     *  implementation for Analog TV. 
     */
    void HandleConnectionChange(bool aConnected);
    
    /**
     * SetImagetoHDMI
     */
    void SetImagetoHDMI();

    /**
     * Sets HDMI to cloning mode
     */
    void SetToCloningMode();
    
    /**
     * Sets HDMI to Native posting mode
     */
    void SetToNativeMode();
    
    /**
     * views should call this if for any item it doesnt want to move to 
     * HDMI posting mode
     */
    void ItemNotSupported();
    
    /**
     * Activate zoom in posting mode
     */
    void ActivateZoom(bool autoZoomOut);
    
    /**
     * Deactivate zoom in posting mode.
     */
    void DeactivateZoom();
    
    /**
     * Fadeing of the Surface
     * @param aFadeInOut - ETrue - FadeIn ( as in gaining brightness )
     *                     EFalse - FadeOut ( as in loosing brightness ) 
     */
    void FadeSurface(bool aFadeInOut);

private:
    /**
     * constructor
     * @param tvoutwrapper GlxTvOutWrapper instance
     * @param model QAbstractItemModel instance
     */
    GlxTvOutWrapperPrivate(GlxTvOutWrapper* aTvOutWrapper,
            QAbstractItemModel* aModel);
    
    /**
     * constructL()
     * @param aScreenSize screensize 
     */
    void ConstructL(QSize aScreenSize);
    
    /**
     * SetNewImage
     * Get the uri and bmp from the media model 
     * and pass it to HDMI controller
     */
    void SetNewImage();
    
    /**
     * getsubstate
     */
    int getSubState();
private:
    GlxTvOutWrapper*            iTvOutWrapper;          // Instance of the Tv Wrapper
    QAbstractItemModel*         iModel;                 // Instance of the model
    CGlxConnectionObserver*     iConnectionObserver;    // Symbian connection observer
    CGlxHdmiController*         iHdmiController;        // Symbian Hdmi Controller
    
    bool       iHdmiConnected;                          // To check if the HDMI is connected
    bool       isImageSetToHdmi;                        // to check if the image is set to HDMI
    bool       iIsPhotosInForeground;                   // to check if the Photos is on foreground
    };

#endif //GLXTVOUTWRAPPERPRIVATE_P_H 

// End of file
