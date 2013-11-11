/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Graphical effects/animations for nmail ui.
*
*/

#include "nmuiheaders.h"

static const char *NmSendAnimation = ":/effects/mail_send.fxml";

/*!
    \class NmEffects
    \brief This class is for launching the effects
*/

/*!
    Constructor
*/
NmUiEffects::NmUiEffects(HbMainWindow &mainWindow) :
    mMainWindow(mainWindow),
    mSendAnimationScreenShot(NULL),
    mDoSendAnimation(false)
{
    NM_FUNCTION;
}

/*!
    Destructor
*/
NmUiEffects::~NmUiEffects()
{
    NM_FUNCTION;
    
    // Clean send animation if sendAnimationComplete slot is not called for some reason.
    // E.g. red key pressed.
    resetSendAnimation();
}

/*!
    Function will perform needed initializations for selected effect.
*/
void NmUiEffects::prepareEffect(NmUiEffectType effect)
{
    NM_FUNCTION;
    
    switch (effect) {
        case NmEditorSendMessageAnimation: {
            // delete any existing stuff
            resetSendAnimation();
            
            // This effect is for editor send message. Get the screen capture of
            // editor view for animation which will be lauched soon.
            mDoSendAnimation = true;
    
            // take screen shot
            mSendAnimationScreenShot = screenShot();
            
            if (mSendAnimationScreenShot){
                // Create graphics item based pixmap image but don't show it yet.
                mSendAnimationScreenShot->hide();
                mSendAnimationScreenShot->setPos(0,0);
                mSendAnimationScreenShot->setZValue(0);
        
                // Adds or moves the item and all its childen to this scene.
                // This scene takes ownership of the item.
                mMainWindow.scene()->addItem(mSendAnimationScreenShot);
        
                // Set editor screen capture visible before old view is popped.
                // New view is drawn under this image.
                mSendAnimationScreenShot->show();
        
                HbEffect::add(mSendAnimationScreenShot, NmSendAnimation, "mail_send");        
            }
            break;
        }
    }
}

/*!
    Function will start the selected effect.
*/
void NmUiEffects::startEffect(NmUiEffectType effect)
{
    NM_FUNCTION;
    
    switch (effect) {
        case NmEditorSendMessageAnimation: {
            // Send message animation for editor view.
            if (mDoSendAnimation && mSendAnimationScreenShot) {
                mDoSendAnimation = false;
                // Start animation and connect completion signal to sendAnimationComplete slot.
                HbEffect::start(mSendAnimationScreenShot, "mail_send", this, "sendAnimationComplete");
            }
            break;
        }
    }
}

/*!
   Generates a screenshot of the current screen. Picture is rotated
   according to the main window orientation.
 */
QGraphicsPixmapItem *NmUiEffects::screenShot()
{
    NM_FUNCTION;
    
    // Grab whole view into pixmap image (also chrome is included)
    QPixmap screenCapture = QPixmap::grabWindow(mMainWindow.internalWinId());
    QGraphicsPixmapItem *ret(NULL);
    
    // for landscape, the screenshot must be rotated
    if(mMainWindow.orientation() == Qt::Horizontal) {
        QMatrix mat;
        mat.rotate(-90); // rotate 90 degrees counter-clockwise
        ret = new QGraphicsPixmapItem(screenCapture.transformed(mat));
    }
    else {
        ret = new QGraphicsPixmapItem(screenCapture);
    }
    
    return ret;    
}

/*!
   Clean up for send animation
 */
void NmUiEffects::resetSendAnimation()
{
    NM_FUNCTION;
    
    if (mSendAnimationScreenShot) {
        // Clean send animation
        HbEffect::remove(mSendAnimationScreenShot, NmSendAnimation, "mail_send");
        // Ownership of QGraphicsPixmapItem is tranferred to GraphicsScene when it has been added
        // to it GraphicsScene.
        // GraphicsPixmapItem needs to be removed from the GraphicsScene before deleting
        // it to prevent double deletion.
        mMainWindow.scene()->removeItem(mSendAnimationScreenShot);
        delete mSendAnimationScreenShot;
        mSendAnimationScreenShot = NULL;
        mDoSendAnimation = false;
    }
}

/*!
    Slot is called when send message animation for editor view has been completed.
*/
void NmUiEffects::sendAnimationComplete(HbEffect::EffectStatus status)
{
    NM_FUNCTION;
    
    Q_UNUSED(status);
    resetSendAnimation();
}
