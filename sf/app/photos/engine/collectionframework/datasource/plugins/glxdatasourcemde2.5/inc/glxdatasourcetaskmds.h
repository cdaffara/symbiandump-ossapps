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



#ifndef _C_GLXDATASOURCETASKMDS_H_
#define _C_GLXDATASOURCETASKMDS_H_

/**
 * @author Simon Brooks, Alex Birkett
 */

// INCLUDES
#include <e32cmn.h>
#include <glxdatasource.h>
#include <glxdatasourcetask.h>
#include <glxfilterproperties.h>
#include <mdccommon.h>
#include <mdequery.h>
#include <mpxfilter.h>

// FORWARD DECLARATIONS
class CGlxDataSourceMde;
class CGlxRequest;
class CMdEObjectDef;
class CMdEObjectQuery;
class CMdESession;


/**
 * Query types supported by subclasses of CGlxDataSourceTaskMde
 */
enum TGlxQueryType
    {
    EContainerFirstItemQuery,    // CGlxDataSourceTaskMdeThumbnail+
    EIdListQuery,                // GlxDataSourceTaskMdeIdList
    EAttributeQuery,             // CGlxDataSourceTaskMdeAttribute
    ELocationAttributeQuery,     // CGlxDataSourceTaskMdeAttribute
    EImageVideoQuery,            // CGlxDataSourceTaskMdeAttribute (but should be general)
    ECommandRemoveFromContainer, // CGlxDataSourceTaskMdeCommand
    ECommandAddToContainer,      // CGlxDataSourceTaskMdeCommand
    ECommandRemoveLocation,      // CGlxDataSourceTaskMdeCommand
    ECommandAddContainer,        // CGlxDataSourceTaskMdeCommand adds a container
    ECommandDeleteContainers,    // CGlxDataSourceTaskMdeCommand deletes containers objects
    ECommandDeleteItems,          // CGlxDataSourceTaskMdeCommand deletes containers objects
    ECommandRenameContainer,
    ECommandRename
    };

/**
 *  CGlxDataSourceTaskMde class 
 *  Base class for mde tasks: provides task generic functionality.
 */
NONSHARABLE_CLASS(CGlxDataSourceTaskMde) : 	public CGlxDataSourceTask, 
                                            public MMdEQueryObserver
	{
public:
    /**
     * Constructor.
     * @param aRequest request to be executed.
     * @param aObserver observer to be informed when task has completed.
     * @param aDataSource data source to be used by this object.
     */
    CGlxDataSourceTaskMde(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& 
            aObserver, CGlxDataSource* aDataSource);
    
    /**
     * Destructor.
     */
    ~CGlxDataSourceTaskMde();
    
    /**
     * Second stage constructor
     */
    virtual void ConstructL();

protected: // to be implemented by deriving classes
    /**
     * See @ref CGlxDataSourceTask::ExecuteRequestL
     */ 
    virtual void ExecuteRequestL() = 0;

    /**
     * Must be implemented by deriving classes. To handle query completion
     * @param aQuery Query that has been completed.
     */
    virtual void DoHandleQueryCompletedL(CMdEQuery& aQuery) = 0;
    
    /**
     * Optionally implemented by deriving classes. Used if the deriving class has
     * to run more than one query. Should start the next query or call HandleRequestComplete() 
     * if no futher queries are required to be run. If the deriving class only has a single query,
     * HandleRequestComplete() can be called from DoHandleQueryCompletedL()
     */
    virtual void DoNextQueryL() {}
    
public: // From CGlxDataSourceTask
    /**
     * See @ref CGlxDataSourceTask::CancelRequest
     */ 
    void CancelRequest();

private: // from MMdEQueryObserver
    /**
     * See @ref MMdEQueryObserver::HandleQueryNewResults
     */ 
    void HandleQueryNewResults( CMdEQuery& aQuery,
                                TInt aFirstNewItemIndex,
                                TInt aNewItemCount );
    /**
     * See @ref MMdEQueryObserver::HandleQueryNewResults
     */
    void HandleQueryNewResults( CMdEQuery& aQuery,
                                TInt aNewObjectItemCount,
                                TInt aNewRelationItemCount,
                                TInt aNewEventItemCount);
    /**
     * See @ref MMdEQueryObserver::HandleQueryCompleted
     */
    void HandleQueryCompleted( CMdEQuery& aQuery, TInt aError);
    
protected:

    /**
     * Returns a pointer to the CGlxDataSourceMde object passed in on instantiation.
     * @return a pointer to the CGlxDataSourceMde object passed in on instantiation.
     */
    CGlxDataSourceMde* DataSource();

    void AddMonthFilterL(const TGlxMediaId& aContainerId, 
            TGlxFilterProperties& aFilterProperties);
    
    void AddMonthFilterL(CMdEObject* aMonth, TGlxFilterProperties& aFilterProperties);
        
	/**
	 * Set query contditions by container id and filter properties.
     * @param aLogicCondition logic condition to add conditions to
     * @param aContainerId Id of the container
     * @param aContainerIsLeft. This should be set to to ETrue if the container is on the left side of the releation, or
     * EFalse if the container is on the right side of the relation
     * 
     */	
    void SetQueryConditionsL(CMdEQuery& aQuery, const TGlxFilterProperties& aFilterProperties, 
            const TGlxMediaId aContainerId, CMdEObjectDef& aObjectDef);

    void SetQueryFilterConditionsL(CMdELogicCondition& aLogicCondition, CMdEObjectDef& aObjectDef,
            const TGlxFilterProperties& aFilterProperties);

    void SetSortOrderL(CMdEQuery& aQuery, CMdEObjectDef& aObjectDef, 
            const TGlxFilterProperties& aFilterProperties);

    /**
     * Returns the maximum number of items that a query should find.
     * @return Maximum number of results
     */
    TInt MaxQueryResultsCount(const TGlxFilterProperties& aFilterProperties) const;
    
    /**
     * Removes and destroys the oldest query in iQueries.
     */
    void RemoveQuery();

    void DoQueryL(CMdEObjectDef& aObjectDef, TBool aIsContent, TGlxQueryType aQueryType,
            TQueryResultMode aResultMode, const TGlxMediaId& aContainerId);

    /**
     * Adds queries for images and or videos to the query queue. The queries will return objects
     * of type image or video that have ids specified by aObjectIds. 
     * @param aObjectIds A list of the ids of the required objects.
     * @param aFilterProperties TGlxFilterProperties.iItemType is used to determine if only 
     * image or video queries are required if iItemType is not set to EGlxFilterImage or 
     * EGlxFitlerVideo a query for images will be queued first followed by a query for videos.
     */
    void QueueImageVideoObjectQueriesL(const RArray<TGlxMediaId>& aObjectIds, 
            const TGlxFilterProperties& aFilterProperties);
    
    /**
     * Adds a query for tag objects that have ids specified by aObjectIds
     * to the query queue.
     * @param aObjectIds A list of the ids of the required objects.
     */
    void QueueTagObjectQueryL(const RArray<TGlxMediaId>& aObjectIds);

    /**
     * Adds a query for album objects that have ids specified by aObjectIds
     * to the query queue.
     * @param aObjectIds A list of the ids of the required objects.
     */
    void QueueAlbumObjectQueryL(const RArray<TGlxMediaId>& aObjectIds);
    
    /**
     * Adds a query for "month" objects that have ids specified by aObjectIds
     * to the query queue.
     * @param aObjectIds A list of the ids of the required objects.
     */
    void QueueMonthObjectQueryL(const RArray<TGlxMediaId>& aObjectIds);
    
    /**
     * Adds a query for objects of type specified by aObjectDef
     * that have ids specified by aObjectIds to the query queue.
     * @param aObjectDef type of objects required.
     * @param aObjectIds A list of the ids of the required objects.
     * @param aQueryType The query type.
     */
    void QueueObjectQueryL(CMdEObjectDef& aObjectDef, 
            const RArray<TGlxMediaId>& aObjectIds, const TGlxQueryType& aQueryType);

    /**
     * Appends a query to the query queue. (the ownership of the query is transferred)
     * @param aQuery query to append to the query queue ownership is transferred to 
     * the query queue. It is expected that aQuery is not on the cleanupstack before the
     * call to AppendQueryL is made
     * @param aQueryType the query type.
     */
    void AppendQueryL(CMdEQuery* aQuery, const TGlxQueryType& aQueryType);
    
    /**
     * Executes the query at ordinal 0 in the
     * query list implementation can be overidden by
     * deriving classes.
     */
    virtual void ExecuteQueryL();
    
private:
    /* 
    * Helper function TRAPed and called by HandleQueryCompleted()
    * @param aQuery the query that has been completed
    */
    void HandleQueryCompletedL(CMdEQuery& aQuery);

    /**
     * Removes the observer from any existing queries, cancels and destroys them
     */
    void DestroyQueries();
    
private:
    /**
     * Default namespace def (not owned)
     */
    CMdENamespaceDef* iDefaultNameSpaceDef;

protected:
    /**
     * Outstanding queries array (owned)
     */
    RPointerArray <CMdEQuery> iQueries;
    
    /**
     * Query types array (owned)
     */
    RArray <TGlxQueryType> iQueryTypes;

    /**
     * Filter properties
     */
    TGlxFilterProperties iFilterProperties;  
    };

#endif //_C_GLXDATASOURCETASKMDS_H_