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



#ifndef _C_GLXDATASOURCETASKMDSATTRIBUTETASK_H_
#define _C_GLXDATASOURCETASKMDSATTRIBUTETASK_H_

/**
 * @author Simon Brooks, Alex Birkett
 */

#include <e32std.h>
#include "glxdatasourcetaskmds.h"


// CLASS DECLARATION

class CGlxGetRequest;    

NONSHARABLE_CLASS(TGlxQueryAttribute)
    {
public:
    TGlxQueryAttribute(
        TMPXAttribute aAttribute,
        CMPXMedia* aMedia,
        TGlxFilterProperties aFilterProperties
        ) : 
        iAttribute(aAttribute),
        iMedia(aMedia),
        iFilterProperties(aFilterProperties)
            {
            
            };
public:
    TMPXAttribute iAttribute;
    CMPXMedia* iMedia;
    TGlxFilterProperties iFilterProperties;
    };

/**
 * Attribute retrieval task.
 */
NONSHARABLE_CLASS(CGlxDataSourceTaskMdeAttributeMde) : public CGlxDataSourceTaskMde
	{
public:
    /**
     * Constructor.
     * @param aRequest request to be executed.
     * @param aObserver observer to be informed when task has completed.
     * @param aDataSource data source to be used by this object.
     */
	CGlxDataSourceTaskMdeAttributeMde(CGlxGetRequest* aRequest, 
	        MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource);
	
    /**
     * Destructor.
     */
    ~CGlxDataSourceTaskMdeAttributeMde();
	
private: // From CGlxDataSourceTask     
    /**
     * See @ref CGlxDataSourceTask::ExecuteRequestL
     */ 
    void ExecuteRequestL();
	
private: // From CGlxDataSourceTaskMde
    /**
     * See @ref CGlxDataSourceTaskMde::DoHandleQueryCompletedL
     */ 
    void DoHandleQueryCompletedL(CMdEQuery& aQuery);

    /**
     * See @ref CGlxDataSourceTaskMde::DoNextQueryL
     */ 
    void DoNextQueryL();
    
private:	

	void AddAttributesL(CMdEObject& aObject, CMPXMedia* aEntry);

	void AddCollectionAttributesL(CMPXMedia* aEntry);
	
	void AddContainerAttributesL(CMPXMedia* aEntry, CMdEObject* aContainer, 
	        CGlxDataSource::TContainerType aType);
	
	void AddItemAttributesL(CMPXMedia* aEntry, CMdEObject* aItem, 
	        CGlxDataSource::TItemType aType);
	
    /**
     * Add location information to a media object from MDS.
     * @param aLocationId the id of a location object.
     * @param aMedia media object to add a location attribute to.
     */
    void AddLocationAttributeToMediaL(CMPXMedia& aMedia, const TItemId& aLocationId);
        

    void DoHandleAttributeQueryCompletedL();
    
    /**
	 * Handles the completion of a query of type ELocationQuery.
	 */
	void DoHandleLocationQueryCompletedL();
	

	void DoHandleImageVideoQueryCompletedL();
		
    /**
     * Creates a location query and adds it to the queue of queries.
     */
    void QueueLocaitonQueryL();

    /**
     * @param aObjectDef object definition which defines objects which will be queried
     * @param aIsContent If ETrue, the definition of the objects being queried is set by the item type filter property, if EFalse, the definition
     * of the object being queried is provided by the aObjectDef parameter
     *  
     */
    void QueueObjectQueryL(CMdEObjectDef& aObjectDef, TBool aIsContent, 
            TGlxQueryType aQueryType, TQueryResultMode aResultMode, 
                          const TGlxMediaId& aContainerId, 
                          TMPXAttribute aAttribute, CMPXMedia* aEntry, 
                          const TGlxFilterProperties& aFilterProperties);
    

	void DoHandleAttributeResponseCompleteL();

	
    /**
     * Determines if the request contains the KGlxMediaGeneralLocation attribtue.
     * @return ETrue if the the KGlxMediaGeneralLocation attribute is requrested else EFalse
     */
    TBool LocationAttributeRequested();
    
    /**
     * Executes the next query in the queue of queries.
     */
    void ExecuteQueryL(); 

private:
    /**
     * Results media array (owned)
     */
    CMPXMediaArray* iMediaArray;
    
    /**
     * Attributes associated with outstanding queries
     */
    RArray< TGlxQueryAttribute > iQueryAttributes;
    
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif    
	};


#endif // _C_GLXDATASOURCETASKMDSATTRIBUTETASK_H_
