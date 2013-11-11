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
* Description: The data source mds class   
*
*/



/**
 * @author Simon Brooks, Alex Birkett
 */

/**
 * @internal reviewed 10/07/2007 by M Byrne
 */

#ifndef _C_GLXDATASOURCEMDS_H_
#define _C_GLXDATASOURCEMDS_H_

// INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <e32property.h> 

#include <e32cmn.h>
#include <f32file.h>

#include <mdesession.h>

#include <harvesterclient.h>
#include <harvestereventenum.h>
#include <mpxcollectionmessagedefs.h>
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include "mthumbnailfetchrequestobserver.h"
#include "glxdatasource.h"

// FORWARD DECLARATIONS
class CGlxRequest;
class CGlxDataSourceMde ;

const TUid KHarvesterPSShutdown = { 0x200009F5 } ;
const TInt KMdSShutdown     = 0x00000002; // values 1 = shutdown, 0 = restart, normal state

static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_C1(KPowerMgmtPolicy,ECapabilityPowerMgmt);

/**
 * PSCW Listener Observer interface for signaling that MDS has Shutdown/restarted
 */
class MGlxMDSShutdownObserver
    {
public:

    virtual void ShutdownNotification(TInt aShutdownState) = 0;
    };

/**
 *  Active object for observing P&S keys
 *
 *  @since S60 v5.0
 */
class CGlxMDSShutdownObserver: public CActive
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v5.0
     * @return Instance of CGlxMDSShutdownObserver.
     */
    static CGlxMDSShutdownObserver* NewL( MGlxMDSShutdownObserver& aObserver, const TUid& aKeyCategory,
                                      const TInt aPropertyKey, TBool aDefineKey);

    /**
     * Destructor
     *
     * @since S60 v5.0
     */
    virtual ~CGlxMDSShutdownObserver();
    
protected:

    /**
     * Handles an active object's request completion event.
     *
     * @since S60 v5.0
     */
    void RunL();

    /**
     * Implements cancellation of an outstanding request.
     *
     * @since S60 v5.0
     */
    void DoCancel();

private:

    /**
     * C++ default constructor
     *
     * @since S60 v5.0
     * @return Instance of CGlxMDSShutdownObserver.
     */
    CGlxMDSShutdownObserver( MGlxMDSShutdownObserver& aObserver, const TUid& aKeyCategory,
                         const TInt iPropertyKey, TBool aDefineKey);

    /**
     * Symbian 2nd phase constructor can leave.
     *
     * @since S60 v5.0
     */
    void ConstructL();

private:
    
    // not own
    MGlxMDSShutdownObserver& iObserver;
    
    const TUid& iKeyCategory;
    RProperty iProperty;
    TInt iPropertyKey;
    
    TBool iDefineKey;
};



// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxDataSourceMde class 
 *
 *  Data Source Mde is a Meta Data Data Source.
 *  @lib GlxDataSourceMde.lib
 *  @ingroup collection_component_design
 */
class CGlxDataSourceMde : public CGlxDataSource,
                          public MMdESessionObserver,
                          public MGlxMDSShutdownObserver,
                          public MMdEObjectObserver,
                          public MMdEObjectPresentObserver,
                          public MMdERelationObserver,
                          public MMdERelationPresentObserver,
                          public MHarvesterEventObserver,
						  public MThumbnailManagerObserver
    {
public:
    static CGlxDataSourceMde* NewL();

private:
    ~CGlxDataSourceMde();
    
    CGlxDataSourceMde();
    
    void ConstructL();

public:
    // from CGlxDataSource
    CGlxDataSourceTask* CreateTaskL(CGlxRequest* aRequest,
            MGlxDataSourceRequestObserver& aObserver);
public:
    // from MHarvesterEventObserver
    void HarvestingUpdated( 
                HarvesterEventObserverType aHEObserverType, 
                HarvesterEventState aHarvesterEventState,
                TInt aItemsLeft );

public:
	void FetchThumbnailL(CGlxRequest* aRequest, 
	        MThumbnailFetchRequestObserver& aObserver);
	TInt CancelFetchThumbnail();

private: 
    /**
    *  from MMdESessionObserver
    */
    void HandleSessionOpened(CMdESession& aSession, TInt aError);
    
    void HandleSessionError(CMdESession& aSession, TInt aError);

private: // MMdEObjectObserver
	/**
	 * See @ref MMdEObjectObserver::HandleObjectNotification
	 */
	void HandleObjectNotification(CMdESession& aSession, 
						TObserverNotificationType aType,
						const RArray<TItemId>& aObjectIdArray);

private: // MMdEObjectPresentObserver
	/**
	 * See @ref MMdEObjectPresentObserver::HandleObjectPresentNotification
	 */
	void HandleObjectPresentNotification(CMdESession& aSession, 
			TBool aPresent, const RArray<TItemId>& aObjectIdArray);

private: // MMdERelationObserver
	/**
	 * See @ref MMdERelationObserver::HandleRelationNotification
	 */
	void HandleRelationNotification(CMdESession& aSession, 
			TObserverNotificationType aType,
			const RArray<TItemId>& aRelationIdArray);
	    
private: //MMdERelationPresentObserver
	/**
	 * See @ref MMdERelationPresentObserver::HandleRelationPresentNotification
	 */
	void HandleRelationPresentNotification(CMdESession& aSession,
			TBool aPresent, const RArray<TItemId>& aRelationIdArray);

    // from MThumbnailManagerObserver
    void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );
    
    void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );
		
private:
    void BackgroundThumbnailMessageL(const TGlxMediaId& aId, const TSize& aSize, 
            TInt aError);

    void DoSessionInitL();

    void AddMdEObserversL();

    void ProcessUpdateArray(const RArray<TItemId>& aArray, TMPXChangeEventType aType,
            TBool aIsObject);

    void ProcessItemUpdateL();

    static TInt ProcessItemUpdate(TAny* aPtr);
    void CreateSessionL();

    static TInt CreateSession(TAny* aPtr);

    void PrepareMonthsL();
    void TaskCompletedL();

    void TaskStartedL();
    
    /*
     * This function doesnt add up any value, added to reduce compiler warnings
     */
    void ThumbnailReadyL( TInt aError, MThumbnailData& aThumbnail,
        TThumbnailRequestId aId, TBool aQuality );
    
public:

    void CreateSession();
    
    inline CMdESession& Session() const;
    
    inline RFs& FileServerSession();

    inline const TGlxMediaId& CameraAlbumId() const;

    inline const TGlxMediaId& FavoritesId() const;

    inline CMdENamespaceDef* NamespaceDef() const;

    inline CMdEObjectDef& ObjectDef() const;

    inline CMdEObjectDef& ImageDef() const;

    inline CMdEObjectDef& VideoDef() const;
    
    inline CMdEObjectDef& AlbumDef() const;
    
    inline CMdEObjectDef& MediaDef() const;
    inline CMdEObjectDef& TagDef() const;
    
    /**
     * Return object definition used to describe months.
     * @return object definition used to describe months.
     */
    inline CMdEObjectDef& MonthDef() const;
    
    inline CMdERelationDef& ContainsDef() const;
    
    inline CMdERelationDef& ContainsLocationDef() const;

    /**
     * Get the location definition.
     * @return location definition.
     */
    inline CMdEObjectDef& LocationDef() const;
    
    TContainerType ContainerType(CMdEObject* aObject);
    
    TContainerType ContainerType(CMdEObjectDef* aObjectDef);
    
    TItemType ItemType(CMdEObject* aObject);
 
    const TGlxMediaId GetMonthIdL(const TTime& aMonth);

    TBool SameMonth(const TTime& aOldDate, const TTime& aNewDate);
     
	TBool ContainerIsLeft(CMdEObjectDef& aObjectDef);    
	
	// from MGlxMDSShutdownObserver
    void ShutdownNotification(TInt aShutdownState);

private:
	MThumbnailFetchRequestObserver* iTnFetchObserver; 
    CThumbnailManager* iTnEngine;
    CFbsBitmap* iTnThumbnail;
    CFbsBitmap* iThumbnail;
    TThumbnailRequestId iTnThumbnailCbId;
    TBool iTnRequestInProgress;
    TInt iTnHandle;
    TGlxMediaId iMediaId;
    
    CMdESession*              iSession;
    RFs iFs;
    TBool                     iSessionOpen;
    TGlxMediaId               iCameraAlbumId;
    TGlxMediaId               iFavoritesId;
    
    CMdENamespaceDef* iNameSpaceDef;
    CMdEObjectDef* iObjectDef;
    CMdEObjectDef* iImageDef;
    CMdEObjectDef* iVideoDef;
    CMdEObjectDef* iAlbumDef;
    CMdEObjectDef* iMediaDef;
    CMdEObjectDef* iTagDef;
    CMdEObjectDef* iMonthDef;
    /**
     * Location object definition.
     */
    CMdEObjectDef* iLocationDef;
    CMdERelationDef* iContainsDef;
    CMdERelationDef* iContainsLocationDef;
    
    RArray<TTime> iMonthArray;
    RArray<TGlxMediaId> iMonthList;
    RArray<TItemId> iAddedItems;
    
    TTime iFirstMonth;
    TTime iLastMonth;
    
    CAsyncCallBack* iUpdateCallback;
    
    CAsyncCallBack* iCreateSessionCallback;
    
    struct TUpdateData
        {
        TItemId iId;
        TMPXChangeEventType iType;
        TBool iIsObject;
        };
    
    RArray<TUpdateData> iUpdateData;
	TBool iPauseUpdate;
    TInt iDeletedCount;
    
    RHarvesterClient iHC;
    TBool iHarvestingOngoing;

    CGlxMDSShutdownObserver* iMDSShutdownObserver;

#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif    
    };

#include "glxdatasourcemds.inl"

#endif //_C_GLXDATASOURCEMDE_H_
