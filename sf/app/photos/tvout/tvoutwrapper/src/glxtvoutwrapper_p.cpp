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
* Description:    Implementation of private wrapper class
*
*/

#include "glxtvoutwrapper.h"
#include "glxtvoutwrapper_p.h"
#include "glxtvconnectionobserver.h"
#include "glxhdmicontroller.h"
#include "glxmodelparm.h"
#include "glxviewids.h"

#include "glxtracer.h"
#include "glxlog.h"

// -----------------------------------------------------------------------------
// Static method to create the private wrapper instance 
// -----------------------------------------------------------------------------
GlxTvOutWrapperPrivate* GlxTvOutWrapperPrivate::Instance(GlxTvOutWrapper* aTvOutWrapper,
        QAbstractItemModel* aModel,QSize aScreenSize)
    {
    TRACER("GlxTvOutWrapperPrivate::Instance()");
    GlxTvOutWrapperPrivate* self = new GlxTvOutWrapperPrivate(aTvOutWrapper,aModel);
    if (self){
        TRAPD(err,self->ConstructL(aScreenSize));
        if(err != KErrNone){
            delete self;
            self = NULL;
            }
        }
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL
// This creates the Connection observer and the Hdmi Controller
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::ConstructL(QSize aScreenSize)
    {
    TRACER("GlxTvOutWrapperPrivate::ConstructL()");
    iConnectionObserver = CGlxConnectionObserver::NewL(this);
    if (!iHdmiController) {
        TRect rect(0,0,aScreenSize.width(),aScreenSize.height());
        iHdmiController = CGlxHdmiController::NewL(rect);
        iHdmiConnected = iConnectionObserver->IsHdmiConnected();
        }
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
GlxTvOutWrapperPrivate::GlxTvOutWrapperPrivate(GlxTvOutWrapper* aTvOutWrapper,
        QAbstractItemModel* aModel):iTvOutWrapper(aTvOutWrapper),
                iModel(aModel),
                iConnectionObserver(NULL),
                iHdmiController(NULL),
                iHdmiConnected(false),
                isImageSetToHdmi(false),
                iIsPhotosInForeground(false)
    {
    TRACER("GlxTvOutWrapperPrivate::GlxTvOutWrapperPrivate()");
    // Do Nothing
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
GlxTvOutWrapperPrivate::~GlxTvOutWrapperPrivate()
    {
    TRACER("GlxTvOutWrapperPrivate::~GlxTvOutWrapperPrivate()");
    if (iConnectionObserver){
    delete iConnectionObserver;
    iConnectionObserver = NULL;
    }
    if (iHdmiController){
    delete iHdmiController;
    iHdmiController = NULL;
    }
    }

// -----------------------------------------------------------------------------
// HandleConnectionChange
// observes the connection change to modify the connection flag
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::HandleConnectionChange(bool aConnected)
    {
    TRACER("GlxTvOutWrapperPrivate::HandleConnectionChange()");
    iHdmiConnected = aConnected;
    // if Connection state positive and uri/bmp are not passed to HDMI already
    // then it is a new image - Set it.
    GLX_LOG_INFO2("GlxTvOutWrapperPrivate::HandleConnectionChange() - isImageSetToHdmi-%d, iHdmiConnected-%d",
            isImageSetToHdmi,iHdmiConnected);
    if (!isImageSetToHdmi && iHdmiConnected && getSubState() !=IMAGEVIEWER_S && iIsPhotosInForeground)
        {
        SetNewImage();
        }
    }

// -----------------------------------------------------------------------------
// SetImagetoHDMI
// if the Hdmi is connected, Set the new image else do nothing
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetImagetoHDMI()
    {
    TRACER("GlxTvOutWrapperPrivate::SetImagetoHDMI()");
    iIsPhotosInForeground = true;
    if (iHdmiConnected)
        {
        // Set the Image
        SetNewImage();
        // Set the flag after HDMI is set for the image
        isImageSetToHdmi = true;    
        }
    else
        {
        // UnSet the flag as HDMI not set
        // this would be in future use if HDMI not set for a image in FS/SS and
        // cable connected
        isImageSetToHdmi = false;
        }
    }

// -----------------------------------------------------------------------------
// SetNewImage
// Private implementation of setting the image to HDMI
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetNewImage()
    {
    TRACER("GlxTvOutWrapperPrivate::SetNewImage()");
    QVariant focusVariant =(iModel->data(iModel->index(0,0),GlxFocusIndexRole)); 
    int focusIndex;
    if (focusVariant.isValid() && focusVariant.canConvert<int>()) {
        focusIndex = (focusVariant.value<int>());
        GLX_LOG_INFO1("GlxTvOutWrapperPrivate::SetNewImage() focusindex = %d",focusIndex);
	}
	else{
		return ;
	}
    
    // Get the image uri
    QString imagePath = (iModel->data(iModel->index(focusIndex,0),GlxUriRole)).value<QString>();
    if(imagePath.isNull()) {
    // Null path no need to proceed
        return ;
    }
   
    TPtrC aPtr = reinterpret_cast<const TUint16*>(imagePath.utf16());
    QVariant var = (iModel->data(iModel->index(focusIndex,0),GlxHdmiBitmap));
    CFbsBitmap* bmp = var.value<CFbsBitmap*>();
    iHdmiController->SetImageL(aPtr,bmp);
    iIsPhotosInForeground = true;
    }

// -----------------------------------------------------------------------------
// setToCloningMode 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetToCloningMode()
    {
    TRACER("GlxTvOutWrapperPrivate::SetToCloningMode()");
    iIsPhotosInForeground = false;
    if(iHdmiController && iHdmiConnected){
    iHdmiController->ShiftToCloningMode();
    }
    }

// -----------------------------------------------------------------------------
// setToNativeMode 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetToNativeMode()
{
    TRACER("GlxTvOutWrapperPrivate::SetToNativeMode()");
    iIsPhotosInForeground = true;

    if ( iHdmiConnected ) {
        if ( !isImageSetToHdmi ) {
            SetNewImage(); // this case can occur when FS image is opened and set to background
            // HDMI cable connected and then FS is brought to foreground
        }
    }
    else {
        iHdmiController->ShiftToPostingMode();
    }
}

// -----------------------------------------------------------------------------
// itemNotSupported 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::ItemNotSupported()
    {
    TRACER("GlxTvOutWrapperPrivate::ItemNotSupported()");
    if(iHdmiController && iHdmiConnected){
    iHdmiController->ItemNotSupported();
    }
    }

// -----------------------------------------------------------------------------
// activateZoom 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::ActivateZoom(bool autoZoomOut)
    {
    if(iHdmiController && iHdmiConnected){
    iHdmiController->ActivateZoom(autoZoomOut);
    }
    }

// -----------------------------------------------------------------------------
// deactivateZoom 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::DeactivateZoom()
    {
    if(iHdmiController && iHdmiConnected){
    iHdmiController->DeactivateZoom();
    }
    }

// -----------------------------------------------------------------------------
// getSubState 
// -----------------------------------------------------------------------------
int GlxTvOutWrapperPrivate::getSubState()
{
    int substate = NO_FULLSCREEN_S;
    QVariant variant = iModel->data( iModel->index(0,0), GlxSubStateRole );    
    if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
        substate = variant.value<int>();
    }
    return substate;
}

// End of file
