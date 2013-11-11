/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#ifndef CVCXTESTMDSDBMODIFIER_H
#define CVCXTESTMDSDBMODIFIER_H

#include <mdesession.h>
#include <mdequery.h>
#include <harvesterclient.h>
#include <mpxcollectionmessagedefs.h>

// Copied from vcxmyvideosmdsdb.cpp, keep up to date.

/**
 *  MDS database class for VCX testing.
 */
NONSHARABLE_CLASS(CVCXTestMdsDbModifier) :
                                public CBase,
                                public MMdESessionObserver,
                                public MMdEQueryObserver,
                                public MMdESchemaObserver,
                                public MMdEObjectObserver
    {

public:

    /**
     * Two-phased constructor.
     */
    static CVCXTestMdsDbModifier* NewL();

    /**
     * Two-phased constructor.
     */
    static CVCXTestMdsDbModifier* NewLC();


    /**
    * Destructor.
    */
    virtual ~CVCXTestMdsDbModifier();

    /**
     * Remove video.
     *
     * @param aMdsId Id of the item to be removed.
     * @return       KErrNotFound if video was not found, KErrNone if found and deleted.
     */
    TInt RemoveVideoL(
        TUint32 aMdsId );

    /**
     * Update video.
     *
     * @param aVideo Video to update in MDS database.
     */
    void UpdateVideoL(
        CMPXMedia& aVideo );

    /**
     * Delete an existing video.
     *
     * @param aFileId      File id (key) of the video to delete.
     * @param aResultCount Result count is saved to this parameter.
     * @return KErrNone if successful, otherwise another of the system-wide
     *         error codes.
     */
    TInt DeleteVideoL(
        TUint32 aFileId,
        TInt& aResultCount,
        TBool aForcedDelete = EFalse );

// from MMdESessionObserver

	/**
	 * Called to notify the observer that opening the session has been
	 * completed and, if the opening succeeded, the session is ready for use.
	 *
	 * @param aSession  session
	 * @param aError    <code>KErrNone</code>, if opening the session succeeded
	 *     or one of the system-wide error codes, if opening the session failed
	 */
	void HandleSessionOpened( CMdESession& aSession, TInt aError );

	/**
	 * Called to notify the observer about errors, which are not a direct
	 * consequence of the operations initiated by the client but caused by some
	 * external source (e.g., other clients). The error cannot be recovered and
	 * all on-going operations initiated by the client have been aborted.
	 * Any attempts to continue using the session will cause a panic. The
	 * client should close the session immediately and try to open a new
	 * session if it needs to continue using the metadata engine.
	 *
	 * @param aSession  session
	 * @param aError    one of the system-wide error codes
	 */
	void HandleSessionError( CMdESession& aSession, TInt aError );


//  from MMdEQueryObserver

    /**
     * Called to notify the observer that new results have been received
     * in the query.
     *
     * @param aQuery              Query instance that received new results.
     * @param aFirstNewItemIndex  Index of the first new item that was added
     *                            to the result item array.
     * @param aNewItemCount       Number of items added to the result item
     *                            array.
     */
    void HandleQueryNewResults( CMdEQuery& aQuery,
                                       TInt aFirstNewItemIndex,
                                       TInt aNewItemCount );

    /**
     * Called to notify the observer that the query has been completed,
     * or that an error has occured.
     *
     * @param aQuery  Query instance.
     * @param aError  <code>KErrNone</code>, if the query was completed
     *                successfully. Otherwise one of the system-wide error
     *                codes.
     */
    void HandleQueryCompleted( CMdEQuery& aQuery, TInt aError );


//  from MMdESchemaObserver

	/**
	 * Called to notify the observer that the schema has been modified.
	 *
	 * @param none
	 */
	void HandleSchemaModified();

// from MMdEObjectObserver

    void HandleObjectNotification( CMdESession& aSession,
            TObserverNotificationType aType,
            const RArray<TItemId>& aObjectIdArray);

private:

    /**
     * Constructor.
     * @param aObserver The db change observer.
     */
    CVCXTestMdsDbModifier();

    void ConstructL();

    /**
     * Fill the object from media.
     *
     * @param aMedia            The media class to read from.
     * @param aObject           The object to modify.
     */
    void Media2ObjectL( CMPXMedia& aVideo, CMdEObject& aObject );

    /*
     * Get the schema definitions needed.
     */
    void GetSchemaDefinitionsL();

private: // data

    /**
     * The MDS session object. Own.
     */
    CMdESession* iMDSSession;

    /**
     * The error code saved from the callbacks.
     */
    TInt iMDSError;

    /**
     * The default namespace definition. Not own.
     */
    CMdENamespaceDef* iNamespaceDef;

    /**
     * The Video object definition. Not own.
     */
    CMdEObjectDef* iVideoObjectDef;

    /**
     * 2.
     * The Title property definition. Not own.
     */
    CMdEPropertyDef* iTitlePropertyDef;

    /**
     * 3.
     * The Description property definition. Not own.
     */
    CMdEPropertyDef* iDescriptionPropertyDef;

    // 4. URI ( = file path ), set with SetUriL method

    /**
     * 5.
     * The Size property definition. Not own.
     */
    CMdEPropertyDef* iSizePropertyDef;

    /**
     * 6.
     * The Creation Date property definition. Not own.
     */
    CMdEPropertyDef* iCreationDatePropertyDef;

    /**
    * Time offset from the universal time. Not own.
    */
    CMdEPropertyDef* iTimeOffsetPropertyDef;

    /**
     * 7.
     * The Flags property definition. Not own.
     */
    CMdEPropertyDef* iFlagsPropertyDef;

    /**
     * 8.
     * The Copyright property definition. Not own.
     */
    CMdEPropertyDef* iCopyrightPropertyDef;

    /**
     * 9.
     * The Item Type property definition. Not own.
     */
    CMdEPropertyDef* iItemTypePropertyDef;

    /**
     * 10.
     * The Modified Date property definition. Not own.
     */
    CMdEPropertyDef* iLastModifiedDatePropertyDef;

    /**
     * 11.
     * The Age Profile property definition. Not own.
     */
    CMdEPropertyDef* iAgeProfilePropertyDef;

    /**
     * 12.
     * The Audio Language property definition. Not own.
     */
    CMdEPropertyDef* iAudioLanguagePropertyDef;

    /**
     * 13.
     * The Author property definition. Not own.
     */
    CMdEPropertyDef* iAuthorPropertyDef;

    /**
     * 14.
     * The Origin property definition. Not own.
     */
    CMdEPropertyDef* iOriginPropertyDef;

    /**
     * 15.
     * The Duration property definition. Not own.
     */
    CMdEPropertyDef* iDurationPropertyDef;

    /**
     * 16.
     * The Last Play Point property definition. Not own.
     */
    CMdEPropertyDef* iLastPlayPositionPropertyDef;

    /**
     * 17.
     * The Download ID property definition. Not own.
     */
    CMdEPropertyDef* iDownloadIdPropertyDef;
	
    /**
    * 18.
    * Rating property definition, not own.
    */
    CMdEPropertyDef* iRatingPropertyDef;
    
    /**
    * 19.
    * Bitrate property definition, not own.
    */
    CMdEPropertyDef* iBitratePropertyDef;
    
    /**
    * DRM protection flag, not own.
    */
    CMdEPropertyDef* iDrmPropertyDef;

    /**
    * Not own.
    */
    CMdEPropertyDef* iAudioFourCcPropertyDef;


    /**
     * Active scheduled waiter Own..
     */
    CActiveSchedulerWait* iActiveSchedulerWait;

    /**
     * The file server session.
     */
    RFs iRfs;

    /**
     * Is the db already in use. If so, return KErrInUse.
     */
    TBool iDbInUse;

    public:

    };

#endif // CVCXTESTMDSDBMODIFIER_H

