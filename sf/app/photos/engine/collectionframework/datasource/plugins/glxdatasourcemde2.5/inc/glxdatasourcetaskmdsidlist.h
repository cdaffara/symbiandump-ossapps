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
* Description:    Data Source Task MDS IDlist class
*
*/



#ifndef GLXDATASOURCETASKMDSIDLIST_H_
#define GLXDATASOURCETASKMDSIDLIST_H_

/**
 * @author Simon Brooks, Alex Birkett
 */

// INCLUDES
#include <e32cmn.h>
#include <mdequery.h>

#include <glxdatasourcetask.h>
#include <glxdatasource.h>
#include <mdccommon.h>
#include <mpxfilter.h>
#include <glxfilterproperties.h>
#include "glxdatasourcetaskmds.h"
// FORWARD DECLARATIONS
class CGlxDataSourceMde;
class CGlxIdListRequest;
class CMdEObjectDef;
class CMdEObjectQuery;
class CMdESession;

/**
 *  CGlxDataSourceTaskMdeIdList class 
 *  Task to handle id list requests.
 */
NONSHARABLE_CLASS(CGlxDataSourceTaskMdeIdList) : public CGlxDataSourceTaskMde
	{
public:
    /**
     * Constructor.
     * @param aRequest request to be executed.
     * @param aObserver observer to be informed when task has completed.
     * @param aDataSource data source to be used by this object.
     */
     CGlxDataSourceTaskMdeIdList(CGlxIdListRequest* aRequest, 
             MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource);
        
     /**
      * Destructor.
      */
     ~CGlxDataSourceTaskMdeIdList();
	 
public: // From CGlxDataSourceTask    
    /**
     * See @ref CGlxDataSourceTask::ExecuteRequestL
     */ 
    void ExecuteRequestL();

private: // from CGlxDataSourceTaskMde
    /**
     * See @ref CGlxDataSourceTask::DoHandleQueryCompletedL
     */ 
    void DoHandleQueryCompletedL(CMdEQuery& aQuery);

private:
	void DoHandleListQueryCompletedL();
		
    void DoMonthListCreationL(CMdEQuery& aQuery, const TGlxFilterProperties& 
            aFilterProperties);
        
    void DoPostFilterComplete(const RArray<TGlxMediaId>& aIdArray, TInt aErrorCode);	
    
    void PostFilterL(const RArray<TGlxMediaId>& aFilteredList,
            const TGlxFilterProperties& aFilterProperties);                                                                            
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif
	};

#endif // GLXDATASOURCETASKMDSIDLIST_H_
