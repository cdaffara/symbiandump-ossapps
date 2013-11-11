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
* Description:   
*
*/



#ifndef _C_GLXDATASOURCETASKMDSCOMMAND_H_
#define _C_GLXDATASOURCETASKMDSCOMMAND_H_

/**
 * @internal reviewed 10/07/2007 by M Byrne
 */


// INCLUDES
#include <e32cmn.h>
#include <glxcommandparser.h>
#include <mdequery.h>
#include <mdeitem.h>
#include <glxcommandrequest.h>
#include "glxdatasourcetaskmds.h"

// FORWARD DECLARATIONS
class CGlxDataSourceMde;
class CGlxRequest;
class CGlxStringCache;
class CMdENamespaceDef;
class CMdEObjectDef;
class CMdEObjectQuery;
class CMdERelationDef;
class CMdERelationQuery;
class CMdESession;

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxDataSourceTaskMdeCommand class 
 *
 */
NONSHARABLE_CLASS( CGlxDataSourceTaskMdeCommand ) : public CGlxDataSourceTaskMde, 
									                public MGlxCommandParserCallback

	{
public: // Constructors / Destructors
    /**
     * Constructor.
     * @param aRequest request to be executed.
     * @param aObserver observer to be informed when task has completed.
     * @param aDataSource data source to be used by this object.
     */
     CGlxDataSourceTaskMdeCommand(CGlxCommandRequest* aRequest, 
             MGlxDataSourceRequestObserver& aObserver,
             CGlxDataSource* aDataSource);

     /**
      * Symbian 2nd stage constructor
      * See @ref CGlxDataSourceTaskMde::ConstructL
      */
     void ConstructL();
     
     /**
      * Destructor
      */
     ~CGlxDataSourceTaskMdeCommand();

private: // From MGlxCommandParserCallback	
 
     /**
      * See @ref MGlxCommandParserCallback::AddContainerL
      */ 
     void AddContainerL(const TDesC& aContainerName);
 
     /**
     * See @ref MGlxCommandParserCallback::AddToContainerL
     */ 
     void AddToContainerL(const RArray<TGlxMediaId>& aSourceIds, 
             const RArray<TGlxMediaId>& aTargetContainers);
     
     /**
      * See @ref MGlxCommandParserCallback::AddToContainerL
      */ 
     void AddToContainerL(const TDesC& aSourceUri, 
             const RArray<TGlxMediaId>& aTargetContainers);    
     
     /**
      * See @ref MGlxCommandParserCallback::CopyL
      */ 
     void CopyL(const RArray<TGlxMediaId>& aSourceIds, const TDesC& aDrive);
     
     /**
      * See @ref MGlxCommandParserCallback::MoveL
      */ 
     void MoveL(const RArray<TGlxMediaId>& aSourceIds, const TDesC& aDrive);
     
     /**
      * See @ref MGlxCommandParserCallback::RemoveFromContainerL
      */ 
     void RemoveFromContainerL(const RArray<TGlxMediaId>& aItemIds,
             const TGlxMediaId& aContainerId);
     
     /**
      * See @ref MGlxCommandParserCallback::DeleteL
      */ 
     void DeleteL(const RArray<TGlxMediaId>& aItemIds);
     
     /**
      * See @ref MGlxCommandParserCallback::RenameL
      */ 
     void RenameL(const TGlxMediaId& aSourceItemId, const TDesC& aTitle);
     
     /**
      * See @ref MGlxCommandParserCallback::SetDescriptionL
      */ 
     void SetDescriptionL(const RArray<TGlxMediaId>& aItemIds, 
             const TDesC& aDescription);
     
     /**
      * See @ref MGlxCommandParserCallback::SetCaptureLocationL
      */ 
     void SetCaptureLocationL(const RArray<TGlxMediaId>& aItemIds, 
             const TCoordinate& aCoordinate);
     
     /**
      * See @ref MGlxCommandParserCallback::ThumbnailCleanupL
      */ 
     void ThumbnailCleanupL();
     
private: // from CGlxDataSourceTaskMde
	
    /**
     * See @ref CGlxDataSourceTaskMde::DoHandleQueryCompletedL
     */
    void DoHandleQueryCompletedL(CMdEQuery& aQuery);
    
    /**
     * See @ref CGlxDataSourceTaskMde::DoNextQueryL
     */
    void DoNextQueryL();

private: 
    
    /**
     * Enumeration to describe file operations
     * (Copy or Move)
     */
	enum TFileOperation
	{
		ECopy,
		EMove
	};
	
    /**
	 * Perform a file operation (copy or move).
	 * @param aSourceIds an array of media ids to copy or move.
	 * @param aDrive destination drive.
	 * @param aFileOperation file operation to perform (either a copy or a move) 
	 */
	void FileOperationL(const TArray<TGlxMediaId>& aSourceIds, 
	        const TDesC& aDrive, TFileOperation aFileOperation);
	
    /**
	 * Gets the container id for a given media id.
	 * If aMedia id is KGlxCollectionRootId (0) then container id
	 * is determined using the collection id. E.g. if the aMediaId
	 * is KGlxCollectionRootId and the collection uid is 
	 * KGlxCollectionPluginCameraImplementationUid
	 * then the camera album id is returned by the method.
	 * When aMedia id is not equal to KGlxCollectionRootId, (the 
	 * vast majority of cases) the container id returned by the method
	 * will be the same as the media id passed in.
	 * 
	 * @param aMediaId should be either a valid container id or KGlxCollectionRootId
	 * @return a container id.
	 */
	TItemId ContainerItemId(const TGlxMediaId& aMediaId);
	
	/**
	 * Uses the PathInfo class to determine the 'root' path for 
	 * a given drive. The 'root' path is C:\data for the C drive and
	 * x:\ for all other drives (where x is a drive letter)
	 * @param aDrive the drive for which the root path is required.
	 * @param aRootPath on return contains the root path.
	 */
	void RootPath(const TDesC& aDrive, TDes& aRootPath);

#if 0	
    /**
     * Copies items from a TGlxMedia array to a TItemId array.
     * @param aDestArray destination array. (It is the callers responsibility to ensure that aDestArray is on
     *                                       the cleanup stack should this be required)
     * @param aSourceArray source array.
     */
    void CopyArrayL(RArray<TItemId>& aDestArray, const RArray<TGlxMediaId>& aSourceArray); 
#endif
    
    /**
     * Sends progress messages to the client
     * @param aCurrentStep number of currently completed steps.
     * @param aStepCount number of total steps to completion.
     */
    void SendProgressMessageL(TInt aCurrentStep, TInt aStepCount);
    
    /** 
     * Returns the container object definition (either album or tag)
     * the container object definition is calculated based on the
     * collection plug-in id associated with the task.
     * @param aContainerObjectDef a pointer to the container object
     * definition associated with the collection plugin if the 
     * function completes with out error. The caller does not take
     * ownership of the pointer.
     * @return KErrNone if the collection plug-in has a container
     * object definition associated with it or a system wide
     * error code.
     */
    TInt ContainerObjectDef(CMdEObjectDef*& aContainerObjectDef);
    
    /**
     * Handle the completion of a 'add to container' query.
     * @param aQuery Query that has been completed.
     */
    void DoHandleAddToContainerQueryCompletedL(CMdEQuery& aQuery);

    /**
     * Handle the completion of a 'add container' query.
     * @param aQuery Query that has been completed.
     */
    void DoHandleAddContainerQueryCompletedL(CMdEQuery& aQuery);
    
    /**
     * Handle the completion of a 'delete containers' query.
     * @param aQuery Query that has been completed.
     */
    void DoHandleDeleteContainersQueryCompletedL(CMdEQuery& aQuery);
    
    /**
     * Handle the completion of a 'delete items' query.
     * @param aQuery Query that has been completed.
     */
    void DoHandleDeleteItemsQueryCompletedL(CMdEQuery& aQuery);
    
    
    /**
     * Handle the completion of a 'rename' query.
     * @param aQuery Query that has been completed.
     */
    void DoHandleRenameQueryCompletedL(CMdEQuery& aQuery);
    
    /**
     * Handle the completion of a 'rename container' query.
     * @param aQuery Query that has been completed.
     */
    void DoHandleRenameConainerQueryCompletedL(CMdEQuery& aQuery);
    
    /**
     * Appends a query to the query queue that determines the number
     * of container objects that have the title aTitle. This
     * method is used to check to see if a container with a
     * specific title exists. The container type is determined 
     * using the ContainerObjectDef() method
     * @aQueryType the query type.
     * @aTitle title to search for.
     */
    void AppendContainerTitleCountQueryL(const TGlxQueryType& aQueryType, const TDesC& aTitle);
    
private: // from CGlxDataSourceTask
	/**
     * See @ref CGlxDataSourceTask::ExecuteRequestL
     */
	void ExecuteRequestL();
	
	/**
     * Matches the given resource ID with the title(user defined new album)
     */	
	
	TInt SearchStringL(TInt aResourceId);
	
	/**
	 * Timer callback to stop scheduler wait
	 * @param aPtr Pointer to object that started the timer
	 * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
	 */
	static TInt SchedulerStopCallback(TAny* aPtr);
	    
	/**
	 * Starts the scheduler wait timer. When complete, Scheduler is stopped
	 */	
	void SchedulerStopComplete();

private:	
	/**
	 * Default name space definition.
	 */
	CMdENamespaceDef* iDefaultNameSpaceDef; 
	
	/**
	 * Collection uid.
	 */
	TUid iCollectionUid;
	
	/**
	 * Before new relations are added to the database, a check must be made to see if 
     * identical relations already exist. This is done asynchronously so they need to 
     * be stored temporarily as a member variable.
	 */
	RArray<TItemId> iLeftIds;
    
    /**
     * Before new relations are added to the database, a check must be made to see if 
     * identical relations already exist. This is done asynchronously so they need to 
     * be stored temporarily as a member variable.
     */
    RArray<TItemId> iRightIds;
    
    /**
     * Used to store object titles while asynchronous operations are in progress.
     * (owned)
     */
    HBufC* iTitle;
    
    /**
     * Used to store the object to rename while asynchronous operations are in progress.
     * (owned)
     */
    CMdEObject* iObjectToRename;
    
    /**
    * Used to get the localized string from the cache
    *
    */
    CGlxStringCache* iStringCache;
    
    /**
	 *  Timer that checks if scheduler wait needs to be cancelled  
	 */
    CPeriodic* iTimer; 
    
    /**
	*  Active scheduler wait object. (Owned)
	*/
    CActiveSchedulerWait* iSchedulerWait;
	};

#endif //_C_GLXDATASOURCETASKMDSCOMMAND_H_
