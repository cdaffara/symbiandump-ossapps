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
* Description:    Implementation of Wrapper class exposed to Views for HDMI
*
*/

#include <glxtvoutwrapper.h>
#include <glxtvoutwrapper_p.h>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
GlxTvOutWrapper::GlxTvOutWrapper():
                mTvOutWrapperPrivate(NULL)
    {
    // Do Nothing
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
GlxTvOutWrapper::~GlxTvOutWrapper()
    {
    if (mTvOutWrapperPrivate)
        {
        delete mTvOutWrapperPrivate;
        mTvOutWrapperPrivate = NULL;
        }
    }

// -----------------------------------------------------------------------------
// setModel 
// and Create the private instance to pass the model
// -----------------------------------------------------------------------------
void GlxTvOutWrapper::setModel(QAbstractItemModel* aModel,QSize aScreenSize)
    {
    if (!mTvOutWrapperPrivate)
        {
        mTvOutWrapperPrivate = GlxTvOutWrapperPrivate::Instance(this,aModel,aScreenSize);
        }
    else
        {
        return;
        }
    }

// -----------------------------------------------------------------------------
// setImagetoHDMI 
// -----------------------------------------------------------------------------
void GlxTvOutWrapper::setImagetoHDMI()
    {
    if(mTvOutWrapperPrivate){
        mTvOutWrapperPrivate->SetImagetoHDMI();
        }
    }

// -----------------------------------------------------------------------------
// setToCloningMode 
// -----------------------------------------------------------------------------
void GlxTvOutWrapper::setToCloningMode()
    {
    if(mTvOutWrapperPrivate){
        mTvOutWrapperPrivate->SetToCloningMode();
        }
    }

// -----------------------------------------------------------------------------
// setToNativeMode 
// -----------------------------------------------------------------------------
void GlxTvOutWrapper::setToNativeMode()
    {
    if(mTvOutWrapperPrivate){
        mTvOutWrapperPrivate->SetToNativeMode();
        }
    }

// -----------------------------------------------------------------------------
// itemNotSupported 
// -----------------------------------------------------------------------------
void GlxTvOutWrapper::itemNotSupported()
    {
    if(mTvOutWrapperPrivate){
        mTvOutWrapperPrivate->ItemNotSupported();
        }
    }

// -----------------------------------------------------------------------------
// activateZoom 
// -----------------------------------------------------------------------------
void GlxTvOutWrapper::activateZoom(bool autoZoomOut)
    {
    if(mTvOutWrapperPrivate){
        mTvOutWrapperPrivate->ActivateZoom(autoZoomOut);
        }
    }

// -----------------------------------------------------------------------------
// deactivateZoom 
// -----------------------------------------------------------------------------
void GlxTvOutWrapper::deactivateZoom()
    {
    if(mTvOutWrapperPrivate){
        mTvOutWrapperPrivate->DeactivateZoom();
        }
    }

// End of file
