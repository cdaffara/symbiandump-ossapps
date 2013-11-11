/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _C_GLXDATASOURCE_H_
#define _C_GLXDATASOURCE_H_

// INCLUDES
#include <e32base.h>
#include <e32cmn.h>
#include <mpxattribute.h>
#include <mpxcollectionpath.h>

#include <ecom/ecom.h>
#include "mglxdatasource.h"

// FORWARD DECLARATIONS
class CGlxRequest;
class CGlxDataSourceTask;
class CGlxDataSource;
// CONSTANTS

// CLASS DECLARATION

/**
 * CDataSourceArrayOwner
 * A singleton that owns a pointer array of datasources
 */
NONSHARABLE_CLASS(CDataSourceArrayOwner) : public CBase
    {
public:
	/**
	 * Returns the CDataSourceArrayOwner singleton
	 * @return the CDataSourceArrayOwner singleton
	 */
    static CDataSourceArrayOwner* InstanceL();
    
    /**
     * Closes the singleton
     */
    void Close();
  
    /**
     * Retuns a reference to the array owned by this object
     * @return a reference to the array owned by this object
     */
    RPointerArray<CGlxDataSource>& Array();
    
private:
	/**
	 * Constructor (private so that only
	 * the singleton store can use it)
	 */
    static CDataSourceArrayOwner* NewL();    
    /**
     * Destructor
     */
    ~CDataSourceArrayOwner();
    
private:
      /**
       * Array of datasource
       * (owned)
       * (array contents not owned)
       */
      RPointerArray<CGlxDataSource> iDataSources;
      };
      
/**
 *  CGlxDataSource class 
 *
 *  Data Source is the Data Source base class.
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxDataSource : public CActive,
					   public MGlxDataSource
	{
	
public:
	IMPORT_C CGlxDataSource();
    IMPORT_C virtual ~CGlxDataSource();
	
public: // from MGlxDataSource
	/**
	 * Asynchronous call to retrieve 
	 * 
	 * @param aListId list for this query relates to
	 * @return error code
	 */
	IMPORT_C void DataSourceRequestL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver);
	/**
	 * Cancel asynchronous request call
	 * as each requestor can only have one active request the observer is sufficient to cancel the request.
	 * 
	 */
	IMPORT_C void CancelRequest(MGlxDataSourceRequestObserver& aObserver);
	/**
	 * Decrements the reference count of the data source. Deletes the 
	 * data source if there are no more references.
	 */
	IMPORT_C void Close(MGlxDataSourceUpdateObserver& aObserver);


public:
    IMPORT_C void HandleTaskComplete(CGlxDataSourceTask* aRequest);

public:
	inline const TUid& Uid() const
		{
		return iUid;
		};
	inline void SetUid(const TUid& aUid)
		{
		iUid = aUid;
		};
	inline const TInt Count() 
		{
		return iDataSourceObservers.Count();
		};
	inline void SetDtorKey(const TUid& aDtorKey)
		{
		iDtorKey = aDtorKey;
		};
	inline void SetEComSession(REComSession& aEComSession)
		{
		iEComSession = aEComSession;
		};
	
	/**
	 * Set the datasource array owner
	 * Should be called after object construction
	 * @param aDataSourceArrayOwner the datasource array owner
	 */
	inline void SetDataSourceArrayOwner(CDataSourceArrayOwner* aDataSourceArrayOwner)
		{
		iDataSourceArrayOwner = aDataSourceArrayOwner;
		};
		
public:
	void AddObserverL(MGlxDataSourceUpdateObserver& aObserver);

public: // from CActive
    IMPORT_C void RunL();
    IMPORT_C void DoCancel();
    IMPORT_C TInt RunError(TInt aError);

public:
	enum TItemType
		{
		EItemTypeNotAnItem = 0,
		EItemTypeImage,
		EItemTypeVideo
		};
		
	enum TContainerType
		{
		EContainerTypeNotAContainer = 0,
		EContainerTypeAlbum,
		EContainerTypeTag,
		EContainerTypeMonth
		};
		
private:
    enum TAction
    {
        EExecuteOnly = 1,
        ECleanupAndExecute
    };
    
private:
	virtual CGlxDataSourceTask* CreateTaskL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver) = 0;
	
	void CompleteSelf(TAction aAction);
	
	virtual void TaskStartedL() = 0;
	virtual void TaskCompletedL()= 0;
	
protected:
	IMPORT_C void BroadcastMessage(CMPXMessage& aMessage);
	IMPORT_C void TryStartTask(TBool aFirstStart = EFalse);
	
protected:
	RPointerArray< MGlxDataSourceUpdateObserver > iDataSourceObservers;
	RPointerArray< CGlxDataSourceTask > iTasks;
	TInt iDataSourceId; // session specific id count from DSM, used to mask the item ids.
	TInt iCount;
	TUid iUid;
	TUid iDtorKey;
	REComSession iEComSession;
	TBool iDataSourceReady;

	/**
	 * Singleton, reference counted
	 * Close() should be called when in order
	 * when object is no longer required.
	 */
	CDataSourceArrayOwner* iDataSourceArrayOwner;
	};




#endif //_C_GLXDATASOURCE_H_
