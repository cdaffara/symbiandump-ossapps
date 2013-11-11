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



#ifndef GLXVIEWMANAGER_H
#define GLXVIEWMANAGER_H

#include <QObject>
#include <QList>
#include <hbeffect.h>
#include "glxuistd.h"
#include <QModelIndex>

class GlxView;
class HbMainWindow;
class QAbstractItemModel;
class HbAction;
class GlxMenuManager;
class HbToolBar;
class HbAction;
class QItemSelectionModel;
class HbMenu;
class GlxEffectEngine;
class HbProgressDialog;
class GlxMainWindowEventFilter;

#ifdef BUILD_VIEWMANAGER
#define GLX_VIEWMANAGER_EXPORT Q_DECL_EXPORT
#else
#define GLX_VIEWMANAGER_EXPORT Q_DECL_IMPORT
#endif

typedef enum
{
    NO_ACTION_ID           = 0x00,
    GLX_ALL_ACTION_ID      = 0x01,
    GLX_ALBUM_ACTION_ID    = 0x02,
    GLX_CAMERA_ACTION_ID   = 0x04,
    GLX_OVI_ACTION_ID      = 0x08,
    GLX_ALL_ID             = 0xFF
} glxToolBarActionIds;

/**
 * view manager class
 */
class GLX_VIEWMANAGER_EXPORT GlxViewManager : public QObject
{
Q_OBJECT

public :
    /**
     * Constructor
     */
    GlxViewManager();
    /**
     * Destructor
     */
    ~GlxViewManager();

    void setupItems( );

    /**
     * launch application
     * @param id viewId
     * @param model model to be used for the view
     */
    void launchApplication( qint32 id, QAbstractItemModel *model );

    /**
     * add back key action
     */
    void addBackSoftKeyAction();

    /**
     * This will return the orientation of main window
     */    
    Qt::Orientation orientation() const;

    /**
     * This will deactivate the current function
     * to be used only in cases where External launch was done
     */     
    void deactivateCurrentView();

    /**
     *  To update the tool bar enable and disable icon
     *  id = This should be selected toolbar tab id
     */    
    void updateToolBarIcon( int id );

    /**
     * Enable the marking mode of the view to select multiple item
     */    
    void enterMarkingMode( qint32 viewId, qint32 commandId );

    /**
     * Enable the normal mode of the view
     */    
    void exitMarkingMode( qint32 viewId );

    /**
     * Pass the user action to the view
     */    
    void handleUserAction( qint32 viewId, qint32 commandId );

    /**
     *  Return the selection model to the user
     */    
    QItemSelectionModel * getSelectionModel( qint32 viewId );

    /**
     * To set the model of current view
     */
    void setModel( QAbstractItemModel *model );
	    
signals :
    /**
     *  emit the user action
     */
    void actionTriggered( qint32 id );
    void externalCommand( int cmdId );

public slots:
    /**
     *  This public slot is used to launch the view
     */
    void launchView ( qint32 id, QAbstractItemModel *model );

    /**
     *  It is over load slot and used to run the animation for view transition and launch the view
     */    
    void launchView ( qint32 id, QAbstractItemModel *model, GlxEffect effect, GlxViewEffect viewEffect );
    
    void launchProgressDialog( int maxValue );
    void updateProgressDialog( int currentValue );

    /**
     *  It will removed and deleted the view.
     *  Currently It is not used so may be in future, It will be removed.
     */    
    void destroyView ( qint32 id );

    /**
     *  It will pass the user action to the state manager
     */    
    void actionProcess( qint32 id );

    /**
     *  It will pass the user selected menu action to state manager 
     *  check for depricated with actionProcess api
     */    
    void handleMenuAction( qint32 commandId );

    /**
     *  It will pass the user action ( tool bar + back ) to state manager
     */    
    void handleAction();
    void cancelTimer();

    /**
     *  This is slot used for the animation finished call back
     */    
    void effectFinished( );

    /**
     *  This will open the item specifc Menu
     */    
    void itemSpecificMenuTriggered( qint32, QPointF );
    
    void handleReadyView();
	
private slots:

   void checkMarked();
   void hideProgressDialog();
   
private:
    /**
     * It will create and return the view
     */
    GlxView * resolveView ( qint32 id );

    /**
     *  It will find a view from the view list and return it
     */    
    GlxView * findView ( qint32 id );

    /**
     *  It will deativate the current view
     */    
    void deActivateView();

    /**
     *  It will activate and show the view
     */
    void activateView();

    /**
     *  It will create the grid and list view tool bar action
     */    
    void createActions();

    /**
     *  It will create the marking mode toll bar action
     */    
    void createMarkingModeActions(); 

    /**
     *  It will create the grid and list view tool bar
     */    
    void createToolBar();

    /**
     *  It will create the marking mode tool bar
     */    
    void createMarkingModeToolBar();
    
    /**
     * setMarkingToolBarAction() - set the toolbar action text
     */
    void setMarkingToolBarAction( qint32 commandId );

    /**
     *  It will add all the view manager related connection
     */    
    void addConnection();

    /**
     *  It will remove all the view manager releted connection
     */    
    void removeConnection();
    
    /**
     * 
     */    
    int getSubState();

private:
    QList<GlxView *> mViewList;  //It contains all the view created by it self.
    HbMainWindow     *mMainWindow; //main window pointer, It have ownership only if 
    HbAction         *mBackAction; // For back soft key
    GlxMenuManager   *mMenuManager; //Pointer of menu manger to add the menu in the view 
    GlxEffectEngine  *mEffectEngine; //To run the animation in between view transition
    QList<HbAction *>  mActionList; //Tool bar action list
    QList<HbAction *>  mMarkingActionList; //marking mode tool bar action list
    HbToolBar          *mViewToolBar; //view tool bar
    HbToolBar          *mMarkingToolBar; //marking mode tool bar
    GlxView *mView;
    QAbstractItemModel *mModel; //no ownership
    QItemSelectionModel * mSelectionModel;
    HbProgressDialog *mProgressDialog;
    GlxMainWindowEventFilter* mWindowEventFilter;

};


#endif /* GLXVIEWMANAGER_H_ */
