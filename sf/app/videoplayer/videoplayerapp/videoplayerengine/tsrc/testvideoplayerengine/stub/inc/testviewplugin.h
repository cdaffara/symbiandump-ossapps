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
* Description:  Stub test class
*
*/

// Version : %version: %

#include "stub/inc/mpxviewpluginqt.h"

class TestViewPlugin : public MpxViewPlugin
{
    Q_OBJECT

public:
    TestViewPlugin();
    ~TestViewPlugin();

    /**
     * createView is the second operation (after plugin construction which is done somewhere 
     * in the plugin framework) executed in the application layer to make a fully operational 
     * View Plugin descendant.
     * I should allocate all of those resources that are required by plugin, but which are
     * too expensive to allocate in constructor (eg. in case of plugin prefetching).
     */
    virtual void createView();

    /**
     * destroyView is an operation that should be executed just before plugin deletion.
     * Implementation should destroy all of resources allocated during createView execution.
     * It's reason d'etre is based on a fact, that application doesn't controll when exactly
     * plugin will be deleted, so destructor execution could be postponed still holding resources.
     */
    virtual void destroyView();

    /* COMMENT:
     * view activation and deactivation are reversible operations
     * that allows us to give up resources that we could temporary deallocate when that 
     * specific view plugin goes into background.
     */

    /**
     * Called to notice view activation.
     */
    virtual void activateView();

    /**
     * Called to notice view deactivation.
     */
    virtual void deactivateView();

    /**
     * Is active.
     */
    virtual bool activated();
    
    /**
     * gives actual view component, ready to put somewhere into the app layout.
     * However, beware that calling to activate/deactivate in the meantime
     * can invalidate that pointer.
     * 
     * NOTE: Returned type is choosen to be as generic as possible, 
     * so please ensure that it fulfills all your needs (it was HbView* before)
     */
    virtual QGraphicsWidget* getView();
    
    
    virtual MpxViewPlugin* viewPlugin();

public slots:

    /**
     * Signalled by application when orientation has changed.
     */ 
    virtual void orientationChange(Qt::Orientation orientation);

    /**
     * Implementation should take care either to implement
       * go-back operation internally or to emit proper
     * command signal to delegate that responsibility to
     * framework (eg. to switch to previous view).
     */ 
    virtual void back();
    
private:

    bool mActive;
};

