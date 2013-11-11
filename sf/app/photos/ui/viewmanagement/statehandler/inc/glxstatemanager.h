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




#ifndef STATEHANDLER_H
#define STATEHANDLER_H

#include <QObject>
#include <QList>
#include <glxbasestate.h>
#include <qmap.h>
#include <QModelIndex>

class GlxState;
class GlxViewManager;
class GlxMediaModel;
class GlxAlbumModel;
class QAbstractItemModel;
class GlxActionHandler;
class GlxTNObserver;
class AfActivityStorage;
class AfActivation;

#ifdef BUILD_STATEMANAGER
#define GLX_STATEMANAGER_EXPORT Q_DECL_EXPORT
#else
#define GLX_STATEMANAGER_EXPORT Q_DECL_IMPORT
#endif

/**
 * Class Description
 * class to create and manage different states and view navigation 
 */

class GLX_STATEMANAGER_EXPORT GlxStateManager : public QObject
{
    Q_OBJECT
	friend class TestGlxStateManager;
	
public :
    /**
     * Constructor
     */
    GlxStateManager();
    
    /**
     * Destructor
     */
    virtual ~GlxStateManager();

    /**
     * launchFetcher() - launch image fetcher
     */
	void launchFetcher(int fetcherFilterType);    

	/**
     * launchFromExternal() - Fuction to launch the application from some external world
     */
    void launchFromExternal();
    
    /**
     * cleanupExternal() - Clean the all externel data
     */
    void cleanupExternal();
    
    /**
     *  enterMarkingMode() - Move the views to the multiple item selection state
     */
    void enterMarkingMode();
    
    /**
     * exitMarkingMode() - Exit from the multiselection state
     */
    void exitMarkingMode();
    
    /**
     * executeCommand() - Execute the commant on multiple selected item.
     * @param - command Id to execute.
     */
    bool executeCommand( qint32 commandId );
    
    /**
     *  eventFilter() - when application goes into background or come back to foreground
     *  set and reset the background thumbnail generation property
     *  @param QObject
     *  @param QEvent
     */
    bool eventFilter( QObject *obj, QEvent *ev );    

signals :
    /**
     * externalCommand() - Send the user activities ( command ) to the external world
     * @param - command Id to execute.
     */
    void externalCommand( int cmdId );
    
    /**
     * setupItemsSignal() - TO send the signal to initialise the non critical resoruce
     * which is not created in the launch sequence
     */
    void setupItemsSignal();

    /**
     * gridItemSelected() - signal to send current model and selected index for fetcher service
     * @param - selected item model index
     * @param - model
     */
    void gridItemSelected( const QModelIndex &,QAbstractItemModel & );   
    
public slots:
    /**
     * launchApplication() - To launch the application
     */
    void launchApplication();
    
    /**
     * actionTriggered() - To handle the user action, view switching etc
     * @param - user action ID.
     */
    void actionTriggered( qint32 id );
    
    /**
     * setupItems() -To create the items which is not created in the aluch sequence
     */
    void setupItems();
    
    /**
     * updateTNProgress() - call back function to monitor the change in thumbnail manager
     * @param - number of item left to generate the thumbnail
     */
    void updateTNProgress( int count );
    
    /**
     * thumbnailPopulated() - call back to get the information that some initial page are loaded 
     * into cache.
     * It will vanish the progressbar dialog.
     */
    void thumbnailPopulated();
    
    /**
     * saveData() - To save the activity data
     */
    void saveData();

public :
    /**
     * changeState() - It will create a new state and replace the current state with new state in the stack.
     * It will use in the case of state switching.
     * use -1 if state does not have internal state
     * @param - new state id.
     * @param - internal or substate of new state.
     */    
    void changeState( qint32 newStateId, int internalState );
    
    /**
     * previousState() - Go back to previous state and if there is no state in the stack
     * then exit the application. 
     */    
    void previousState();

    /**
     * goBack() - Go back to a state in hierarchy and pop all the state upto that level.
     * if state is not found, then all the state from the hierachy is removed and create a new state on level 0
     * This function will be use in the case when more then one back is required in single event.
     * @param - new state id.
     * @param - internal or substate of new state.
     */
    void goBack( qint32 stateId, int internalState );
       
    /**
     * nextState() - Create a new state and current state should be pushed into stack.
     * use -1 if state does not have internal state
     * @param - new state id.
     * @param - internal or substate of new state.
     */  
    void nextState( qint32 newStateId, int internalState );

    /**
     * removeCurrentModel() - It will delete the current model
     */      
    void removeCurrentModel();
    
    /**
     * cleanAllModel() - It will delete the all model used by state manager
     */
    void cleanAllModel();
    
private:

    /**
     *  launchProgressDialog() - Launch the harvesting and TN generation progress bar dialog
     */
    void launchProgressDialog();
    
    /**
     * vanishProgressDialog() - create the grid view model and wait for loading of some initial page of data into 
     * cache.
     */
    void vanishProgressDialog();
    
    /**
     * createState() - Factory function to create the state.
     */  
    GlxState * createState( qint32 state );
    
    /**
     * createModel() - Factory function to create the model.
     * @param - state id of the state.
     * @param -  state navigation direction
     */  	
    void createModel( qint32 stateId, NavigationDir dir = NO_DIR );

    /**
     * createGridModel() - Factory function to create the grid model.
     * @param -  internal state of grid state.
     * @param - state navigation direction.
     */  	
    void createGridModel( int internalState, NavigationDir dir = NO_DIR );

    /**
     * setFullScreenContext() - To set the fullscreen context based on the currrent orientation
     */    
    void setFullScreenContext();
    
    /**
     * setFullScreenContext() - Apllication event handler function
     * @param - event id
     */
    void eventHandler( qint32 &id );
    
    /**
     * exitApplication() - A function to care the exit for application, in the case when application launch from internal and external
     */        
    void exitApplication();

    /**
     * launchActivity() -Launch Application as an activity.
     * Return Value @0 : If launching an activity fails
     *              @1 : If launch activity passes
     */
     bool launchActivity();

     /**
      * saveImage() - save current image if it is launched from private path.
      */
     void saveImage();
     
     bool validateActivityData();
     
private:
    GlxViewManager      *mViewManager;
    /// for all grid
    GlxMediaModel       *mAllMediaModel;        
    /// for album grid
    GlxMediaModel       *mAlbumGridMediaModel;  
    /// for album list
    GlxAlbumModel       *mAlbumMediaModel;      
    /// for image viewer
	GlxMediaModel       *mImageviewerMediaModel;
	/// no owner ship
    QAbstractItemModel  *mCurrentModel;         
    GlxState            *mCurrentState;		
    GlxActionHandler    *mActionHandler;
    GlxTNObserver       *mTNObserver;
    int                 mCollectionId;
    bool                isProgressbarRunning;
    QMap<QString, qint32> mSaveActivity;
    GlxFetcherFilterType mFetcherFilterType;
    AfActivityStorage   *mActivityStorage;
    AfActivation        *mActivation;
};


#endif /* STATEHANDLER_H_ */
