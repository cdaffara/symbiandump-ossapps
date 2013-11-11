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
* Description:   ?Description
*
*/



#ifndef GLXSLIDESHOWVIEW_H
#define GLXSLIDESHOWVIEW_H

//User Includes
#include "glxview.h"
#include "glxslideshowwidget.h"

//Qt/Orbit forward declarations
class HbMainWindow;
class HbDocumentLoader;

//User Includes forward Declaration
class GlxView;
class GlxSlideShowWidget;
class GlxTvOutWrapper;

/**
 * Class Description
 * This is Slideshow view class used to play the slide show on set of images.
 */
class GlxSlideShowView : public GlxView
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param - HbMainWindow object
     * @param - fullscreen docloader object
     */
    GlxSlideShowView( HbMainWindow *window, HbDocumentLoader *Docloader );
    
    /**
     * Destructor
     */    
    ~GlxSlideShowView();

    /**
     * activate()
     * ///From GlxView
     */    
    void activate() ;
    
    /**
     * deActivate()
     * ///From GlxView
     */    
    void deActivate();
    
    /**
     * setmodel() 
     * ///from GlxView
     */    
    void setModel( QAbstractItemModel *model );

    /**
     * setModelContext() - to set the context mode of the model.
     */    
    void setModelContext();
    
    /**
     * handleUserAction() - To handle the used action like back to paly the orientation change animation.
     */
    void handleUserAction( qint32 commandId );
        
public slots :
    /**
     * orientationChanged() - Slot to upadte the layout when orientation of device has changed
     * @param new orientation
     */
    void orientationChanged( Qt::Orientation ) ;
    
    /**
     * slideShowEventHandler() - to handle the widget events like ui on / off, empty data etc
     * @param slide show widget event type 
     */
    void slideShowEventHandler( GlxSlideShowEvent e );
    
    /**
     * indexchanged() - call back to handle the selected image change. 
     */
    void indexchanged();
    
    /**
     * modelDestroyed() - call back to monitor the model destroy.
     */
    void modelDestroyed();
    
    /**
     * playLsOrientChangeAnim() - To play the protrait to landscape orientation animation.
     */
    void playLsOrientChangeAnim();

    /**
     * playLsOrientChangeAnim() - To play the landscape to protrait orientation animation.
     */
    void playPtOrientChangeAnim();
    
    /**
     * LsOrientChangeAnimFinished() - Call back fuction when protrait to landscape orientation animation 
     * has been finished.
     * @param - staus of effect
     */
    void LsOrientChangeAnimFinished( const HbEffect::EffectStatus );
    
    /**
     * PtOrientChangeAnimFinished() - Call back fuction when landscape to protrait orientation animation 
     * has been finished.
     * @param - staus of effect
     */    
    void PtOrientChangeAnimFinished( const HbEffect::EffectStatus );
    
protected :
    /**
     * eventFilter - To handle the application foregrond and background event
     * ///From HbView
     */
    bool eventFilter( QObject *obj, QEvent *ev );
    
private:
    /**
     * loadWidgets() - The widgets are retrieved from the docml
     */
    void loadObjects();

private:
    QAbstractItemModel  *mModel;
    HbMainWindow        *mWindow;
    GlxSlideShowWidget  *mSlideShowWidget;    
    HbDocumentLoader    *mDocLoader; //Docml loader to load the widgets from docml
    GlxTvOutWrapper*    mTvOutWrapper;
};

#endif /*GLXSLIDESHOWVIEW_H*/
