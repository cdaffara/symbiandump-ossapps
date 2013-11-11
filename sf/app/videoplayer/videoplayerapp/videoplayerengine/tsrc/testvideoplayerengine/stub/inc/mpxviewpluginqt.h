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
* Description:   MpxViewPlugin stub
*
*/

#ifndef MPXVIEWPLUGIN_H_
#define MPXVIEWPLUGIN_H_

#include <QObject>

#define PluginViewInterface MpxViewPlugin

class QGraphicsWidget;

class MpxViewPlugin : public QObject 
{

    Q_OBJECT

public:
	
	/* COMMENT:
	 * following two methods are kind of second-phase create & destroy. Rationale for this step is that
	 * we need some way to promote lazy resources allocation (on application demand) and early resource
	 * deallocation (again, on application direct command).
	 */

	/**
	 * createView is the second operation (after plugin construction which is done somewhere 
	 * in the plugin framework) executed in the application layer to make a fully operational 
	 * View Plugin descendant.
	 * I should allocate all of those resources that are required by plugin, but which are
	 * too expensive to allocate in constructor (eg. in case of plugin prefetching).
	 */
	virtual void createView() = 0;	

	/**
	 * destroyView is an operation that should be executed just before plugin deletion.
	 * Implementation should destroy all of resources allocated during createView execution.
	 * It's reason d'etre is based on a fact, that application doesn't controll when exactly
	 * plugin will be deleted, so destructor execution could be postponed still holding resources.
	 */
	virtual void destroyView() = 0;

	/* COMMENT:
	 * view activation and deactivation are reversible operations
	 * that allows us to give up resources that we could temporary deallocate when that 
	 * specific view plugin goes into background.
	 */

	/**
	 * Called to notice view activation.
	 */
	virtual void activateView() = 0;

	/**
	 * Called to notice view deactivation.
	 */
	virtual void deactivateView() = 0;

    /**
     * Is active.
     */
    virtual bool activated() = 0;
	
	/**
	 * gives actual view component, ready to put somewhere into the app layout.
	 * However, beware that calling to activate/deactivate in the meantime
	 * can invalidate that pointer.
	 * 
	 * NOTE: Returned type is choosen to be as generic as possible, 
	 * so please ensure that it fulfills all your needs (it was HbView* before)
	 */
	virtual QGraphicsWidget* getView() = 0;
	
	virtual MpxViewPlugin* viewPlugin() = 0;

public slots:

	/**
	 * Signalled by application when orientation has changed.
	 */	
	virtual void orientationChange(Qt::Orientation orientation) = 0;

	/**
	 * Implementation should take care either to implement
       * go-back operation internally or to emit proper
	 * command signal to delegate that responsibility to
	 * framework (eg. to switch to previous view).
	 */	
	virtual void back() = 0;

signals:

	/**
	 * Command is the only way to notify from plugin to application
	 * about action needed to be executed.
	 *
	 * @param aCommand  enumeration of command type. 
	 * Currently supported are: ViewBack, CloseApp, GoToNowPlaying, GoToCollectionView.
	 * NOTE: It should be specified how to determine between broadly supported operations 
	 * (back, close etc.) and application/plugin specific (go to collection, go to now playing etc.)
	 *
	 * There is also one major issue here, that there is no guaranted order of command delivery,
	 * so there is second option to use common observer pattern. Should be discussed.
	 */	
	void command(int aCommand);

};


#endif /*MPXVIEWPLUGIN_H_*/
