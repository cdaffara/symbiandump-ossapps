/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef VCXMYVIDEOSCOLLECTIONPLUGINTEST_H
#define VCXMYVIDEOSCOLLECTIONPLUGINTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "MVCXMyVideosCollectionPluginTesterObserver.h"
#include <f32file.h>
#include "MVcxTestTimerObserver.h"
//#ifdef __WINSCW__
#include <harvesterclient.h>
//#endif // __WINSCW__
#include "VCXMyVideosTestUtils.h"

// CONSTANTS
const TInt KTimeoutMinute = 60 * 1000000; // a minute
const TInt KTimeoutSecond = 1000000;
_LIT( KVCXTestZeroIap, "VcxTestZeroIap" );
_LIT( KVCXTestInvalidIap, "VcxTestInvalidIap" );
_LIT( KVCXTestZeroUrl, "VcxTestZeroUrl" );

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KVCXMyVideosCollectionPluginTestLogPath, "\\logs\\testframework\\VCXMyVideosCollectionPluginTest\\" );
// Log file
_LIT( KVCXMyVideosCollectionPluginTestLogFile, "VCXMyVideosCollectionPluginTest.txt" );

// FORWARD DECLARATIONS
class CVCXMyVideosCollectionPluginTest;
class CVCXMyVideosCollectionPluginTester;
class CVCXMyVideosTestUtils;
class CVcxTestActiveWait;
class CVcxTestTimer;
class CMPXMedia;
class TMPXAttributeData;
class CVcxVodContentContentBriefDetails;
class CVCXMyVideosTestServiceEmu;
class CVCXTestMdsDbModifier;
class CFileMan;
class CMPXMediaArray;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
enum TVcxTestMediaAttributeType
    {
    EVcxTestMediaAttributeTypeString = 1,
    EVcxTestMediaAttributeTypeTTime,
    EVcxTestMediaAttributeTypeTDateTime,
    EVcxTestMediaAttributeTypeTInt,
    EVcxTestMediaAttributeTypeTUint,
    EVcxTestMediaAttributeTypeTUint8,
    EVcxTestMediaAttributeTypeTUint32,
    EVcxTestMediaAttributeTypeTReal32,
    EVcxTestMediaAttributeTypeTInt32,
    EVcxTestMediaAttributeTypeTMPXItemId,
    EVcxTestMediaAttributeTypeTInt8,
    EVcxTestMediaAttributeTypeTInt64,
    EVcxTestMediaAttributeTypeMvFlags,
    };

enum TVcxTestMediaAttributeIdMapping
    {
    EVcxTestMapping_KMPXMediaGeneralId = 1, /* Brief */
    EVcxTestMapping_KMPXMediaGeneralTitle, /* Brief */
    EVcxTestMapping_KMPXMediaGeneralComment,
    EVcxTestMapping_KMPXMediaGeneralUri, /* Brief */
    EVcxTestMapping_KMPXMediaGeneralSize, /* Brief, not used */
    EVcxTestMapping_KMPXMediaGeneralExtSizeInt64, /* Brief */
    EVcxTestMapping_KMPXMediaGeneralDate,
    EVcxTestMapping_KMPXMediaGeneralFlags,
    EVcxTestMapping_KMPXMediaGeneralCopyright,
    EVcxTestMapping_KMPXMediaGeneralMimeType,
    EVcxTestMapping_KVcxMediaMyVideosModifiedDate,
    EVcxTestMapping_KVcxMediaMyVideosAgeProfile, /* Brief */
    EVcxTestMapping_KVcxMediaMyVideosAudioLanguage,
    EVcxTestMapping_KVcxMediaMyVideosAuthor,
    EVcxTestMapping_KVcxMediaMyVideosOrigin,
    EVcxTestMapping_KVcxMediaMyVideosDuration,
    EVcxTestMapping_KMPXMediaGeneralLastPlaybackPosition,
    EVcxTestMapping_KVcxMediaMyVideosDownloadId,
    /* Rest are not saved into MDS */
    EVcxTestMapping_KVcxMediaMyVideosRemoteUrl,
    EVcxTestMapping_KVcxMediaMyVideosDownloadState,
    EVcxTestMapping_KVcxMediaMyVideosIapId,
    EVcxTestMapping_KVcxMediaMyVideosUsername,
    EVcxTestMapping_KVcxMediaMyVideosPassword,
    EVcxTestMapping_KVcxMediaMyVideosCommandId,
    EVcxTestMapping_KVcxMediaMyVideosDownloadProgress,
    EVcxTestMapping_KVcxMediaMyVideosCategoryItemCount,
    EVcxTestMapping_KVcxMediaMyVideosCategoryNewItemCount,
    EVcxTestMapping_KVcxMediaMyVideosModifiedDateCurrent,
    EVcxTestMapping_KVcxMediaMyVideosCategoryNewItemName,
    EVcxTestMapping_KVcxMediaMyVideosAudioFourCc
    };

typedef RPointerArray<CVcxVodContentContentBriefDetails> TVcxTestContentArray;

// TestUtils DECLARATION

/**
*  CVCXMyVideosCollectionPluginTest test TestUtils for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVCXMyVideosCollectionPluginTest) : public CScriptBase,
            public MVCXMyVideosCollectionPluginTesterObserver, public MVcxTestTimerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosCollectionPluginTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosCollectionPluginTest();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /*
         * From CVCXMyVideosCollectionPluginTesterObserver.
         */
        virtual void HandleVcxMvTesterMessageL( TInt aMessage, TInt aError );

        /**
        * From MVcxTestTimerObserver Handles timer completion
        * @since
        * @param aTimerId
        * @param aError
        */
        void TimerComplete(TInt aTimerId, TInt aError);

    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosCollectionPluginTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
         * Methods for STIF testscripter.
         */

        /**
        * Initializes the test module.
        *
        * @param CaseId When this is given then events and other major calls are logged into a
        *        separate text file.
        */
        virtual TInt CreateL( CStifItemParser& aItem );

        /**
         * Sets CenRep value for My Videos MPX plugin sort.
         * If category is whole collection is refreshed:
         *  - Sort order set to none.
         *  - Close/Open(index) called for Mpx collection.
         *  - Sort order set as specified.
         *  - Close/Open(index) called for Mpx collection.
         *
         * @param aSortingOrder
         */
        virtual TInt SetSortingOrderL( CStifItemParser& aItem );

        /**
         * Use to toggle automatic refresh of contents when open collection changes.
         * Use with caution, some actions need the refresh.
         */
        virtual TInt SetAutomaticRefresh( CStifItemParser& aItem );

        /**
         * Use to disable some of the logging of test module.
         */
        virtual TInt SetQuietMode( CStifItemParser& aItem );

        /**
         * Waits until VCX MV plugin stops sending messages.
         * Messages are processed but not signaled to test script.
         *
         * @param timeout minutes (default 10)
         */
        virtual TInt CoolDownL( CStifItemParser& aItem );

        /**
         * Toggle filecheck on/off.
         */
        virtual TInt SetFileCheckL( CStifItemParser& aItem );

        /**
         * Sets requests that use multiple medias to use copy of the current medias,
         * those medias will not be updated before this is called again.
         */
        virtual TInt SetUseCopiedMediasL( CStifItemParser& aItem );

        /**
         * Opens MPX collection.
         * Asynchronous, test script will be signaled.
         *
         * @param collection UID as hex.
         */
        virtual TInt OpenCollectionL( CStifItemParser& aItem );

        /**
         * Opens collection level, e.g. My Videos category.
         * Asynchronous, test script will be signaled.
         *
         * @param name name of the level.
         */
        virtual TInt OpenLevelL( CStifItemParser& aItem );

        /**
         * Opens collection level, e.g. My Videos category.
         * Asynchronous, test script will be signaled.
         *
         * @param index of the level.
         */
        virtual TInt OpenLevelByIndexL( CStifItemParser& aItem );

        /**
         * Closes collection level, e.g. My Videos category.
         * Asynchronous, test script will be signaled.
         */
        virtual TInt CloseLevelL( CStifItemParser& aItem );

        /**
         * Refreshes the contents of currently open level.
         * Asynchronous, test script will be signaled.
         */
        virtual TInt RefreshContentsL( CStifItemParser& aItem );

        /**
         * Gets medias by their MPX IDs
         * Video category must be open, current entries will be the items with
         * requested MPX IDs when this succeeds.
         * Asynchronous, test script will be signaled.
         *
         * See CVCXMyVideosCollectionPluginTester header for SelectMediasL for selection rules.
         *
         * @param aStartIndex index at current entries.
         * @param aEndIndex, index at current entries.
         * @param aSyncCall, sync call, default is false for async. 
         */
        virtual TInt GetMediasByMpxIdL( CStifItemParser& aItem );

        /**
         * Gets details of the selected MPX item, e.g. My Videos video.
         * Synchronous.
         *
         * @param driveLetter
         * @param index
         */
        virtual TInt GetMediaFullDetailsL( CStifItemParser& aItem );

        /**
         * Gets details of the selected MPX item, e.g. My Videos video.
         * Synchronous.
         *
         * @param driveLetter
         * @param index
         */
        virtual TInt GetMediaFullDetailsByMpxIdL( CStifItemParser& aItem );

        /**
         * Gets details of all current MPX items and prints them into debug output.
         */
        virtual TInt GetAllMediaFullDetailsL( CStifItemParser& aItem );

        /**
         * Checks an attribute of a video item.
         *
         * This will fetch the full media from MPX.
         *
         * @param driveLetter
         * @param itemIndex or itemName
         * @param media attribute mapping id, see vcxmvcollectionplugintest.inc
         * @param expectedValue
         * @param expectedValue2...X
         * @param When checking for real number use expectedValue2 for digits.
         *        For TMPXItemId expectedValue2 is the iId2 member.
         *        For KMPXMediaGeneralFlags there can be multiple expected values.
         */
        virtual TInt CheckMediaDetailL( CStifItemParser& aItem );

        /**
         * Sets attribute of media item made with CreateCopyOfMediaL.
         *
         * @param media attribute mapping id, see vcxmvcollectionplugintest.inc
         * @param value
         * @param value2...X
         *       When setting real number use value2 for digits.
         *        For TMPXItemId value2 is the iId2 member.
         *        For KMPXMediaGeneralFlags there can be multiple values.
         */
        virtual TInt SetMediaDetailL( CStifItemParser& aItem );

        /**
         * Checks the album count.
         *
         * @param expectedCount
         */
        virtual TInt CheckAlbumCountL( CStifItemParser& aItem );

        /**
         * Renames an album.
         *
         * @param albumName
         * @param newAlbumName
         */
        virtual TInt RenameAlbumL( CStifItemParser& aItem );
        
        /**
         * Checks that an album exists.
         *
         * @param albumName
         */
        virtual TInt CheckAlbumExistsL( CStifItemParser& aItem );                
        
        /**
         * Checks the video count and that the videos really exist.
         *
         * @param driveLetter, if specified checks only videos only on that drive
         * @param expectedCount
         */
        virtual TInt CheckMediaCountL( CStifItemParser& aItem );

        /**
         * Checks the video count and that the videos really exist.
         *
         * @param driveLetter, if specified checks only videos only on that drive
         * @param minimumCount
         */
        virtual TInt CheckMinimumMediaCountL( CStifItemParser& aItem );

        /**
         * Checks the video count and that the videos really exist.
         *
         * @param driveLetter, if specified checks only videos only on that drive
         * @param maximumCount
         */
        virtual TInt CheckMaximumMediaCountL( CStifItemParser& aItem );
        
        /**
         * Creates copy of a media from currently listed medias and stores it to
         * a member variable. It is used by AddMediaL, SetMediaL and
         * SetMediaDetailsL.
         *
         * This will fetch the full media from MPX.
         *
         * @param aDrive
         * @param aIndex
         * @param aFullCopy Optional, default is 1. If set to 0 then empty
         *        MPX media item is created which only has the ID of original.
         */
        virtual TInt CreateCopyOfMediaL( CStifItemParser& aItem );

        /**
         * Creates empty media item to member variable.
         */
        virtual TInt CreateEmptyMediaL( CStifItemParser& aItem );

        /**
         * Adds media to the collection. Uses the media created with CreateCopyOfMediaL / CreateEmptyMediaL,
         * if they are not called then empty item will be used instead.
         *
         * @param aSyncCall
         */
        virtual TInt AddMediaL( CStifItemParser& aItem );

        /**
         * Updates media to the collection. Uses the media created with CreateCopyOfMediaL / CreateEmptyMediaL
         *
         * @param aSyncCall
         */
        virtual TInt SetMediaL( CStifItemParser& aItem );

        /**
         * Makes a direct media update to MDS DB without using My Videos MPX plugin.
         *
         * Uses the media created with CreateCopyOfMediaL.
         */
        virtual TInt OutsideMediaUpdateL( CStifItemParser& aItem );

        /**
         * Media item will be removed with MPX MV plugin command KMPXCommandIdCollectionRemoveMedia.
         * Asynchronous, test script will be signaled.
         *
         * @param aDrive
         * @param aIndex
         * @param aSyncCall
         */
        virtual TInt RemoveMediaL( CStifItemParser& aItem );

        /**
         * Removes all media items with MPX MV plugin command KMPXCommandIdCollectionRemoveMedia.
         *
         * @param aOneByOne, if true then each removal is waited to complete before next is done.
         */
        virtual TInt RemoveAllMediaL( CStifItemParser& aItem );

        /**
         * Moves a media.
         *
         * @param aSourceDriveLetter, where file will be selected from.
         * @param aIndex, index of moved file on source disk.
         * @param aDestDriveLetter, dest drive letter.
         * @param aSyncCall
         *
         * See CVCXMyVideosCollectionPluginTester header for SelectMediasL for selection rules.
         */
        virtual TInt MoveMediaL( CStifItemParser& aItem );

        /**
         * Moves multiple medias.
         *
         * @param aSourceDriveLetter, where file will be selected from.
         * @param aIndex, index of moved file on source disk.
         * @param aStartIndex
         * @param aEndIndex
         * @param aDestDriveLetter
         * @param aSyncCall
         *
         * See CVCXMyVideosCollectionPluginTester header for SelectMediasL for selection rules.
         */
        virtual TInt MoveMediasL( CStifItemParser& aItem );

        /**
         * Cancels move request.
         *
         * @param aSyncCall
         */
        virtual TInt CancelMoveL( CStifItemParser& aItem );

        /**
         * Copies a media.
         *
         * @param aSourceDriveLetter, where file will be selected from.
         * @param aIndex, index of moved file on source disk.
         * @param aDestDriveLetter, dest drive letter.
         * @param aSyncCall
         *
         * See CVCXMyVideosCollectionPluginTester header for SelectMediasL for selection rules.
         */
        virtual TInt CopyMediaL( CStifItemParser& aItem );

        /**
         * Copies multiple medias.
         *
         * @param aSourceDriveLetter, where file will be selected from.
         * @param aStartIndex
         * @param aEndIndex
         * @param aDestDriveLetter, dest drive letter.
         * @param aSyncCall
         *
         * See CVCXMyVideosCollectionPluginTester header for SelectMediasL for selection rules.
         */
        virtual TInt CopyMediasL( CStifItemParser& aItem );

        /**
         * Cancels copy request.
         *
         * @param aSyncCall
         */
        virtual TInt CancelCopyL( CStifItemParser& aItem );

        /**
         * Deletes a media.
         *
         * @param aSourceDriveLetter, where file will be selected from.
         * @param aIndex, index of moved file on source disk.
         * @param aSyncCall
         *
         * See CVCXMyVideosCollectionPluginTester header for SelectMediasL for selection rules.
         */
        virtual TInt DeleteMediaL( CStifItemParser& aItem );

        /**
         * Delete multiple medias.
         *
         * @param aSourceDriveLetter, where file will be selected from.
         * @param aIndex, index of moved file on source disk.
         * @param aStartIndex
         * @param aEndIndex
         * @param aSyncCall
         *
         * See CVCXMyVideosCollectionPluginTester header for SelectMediasL for selection rules.
         */
        virtual TInt DeleteMediasL( CStifItemParser& aItem );

        /**
         * Cancels Delete request.
         *
         * @param aSyncCall
         */
        virtual TInt CancelDeleteL( CStifItemParser& aItem );

        /**
         * Cancels operation right after it starts. For Move/Copy/Delete.
         */
        virtual TInt CancelNextOperationL( CStifItemParser& aItem );

        /**
         * Deletes the file of selected media in current entries.
         *
         * @param aDrive
         * @param aIndex
         */
        virtual TInt DeleteFileOfMediaL( CStifItemParser& aItem );

        /**
         * Delete files of all the current media entries.
         */
        virtual TInt DeleteFilesOfAllMediasL( CStifItemParser& aItem );

        /**
         * Adds an album. If category or album is currently open the change is not 
         * seen by tests before current level is closed.
         *
         * @param aAlbumName
         * @param aAsyncCall, default true
         */
        virtual TInt CreateAlbumL( CStifItemParser& aItem );        

        /**
         * Deletes albums. If category or album is currently open the change is not 
         * seen by tests before current level is closed.
         *
         * @param aAlbumName(s)
         */
        virtual TInt DeleteAlbumsL( CStifItemParser& aItem );
        
        /**
         * Deletes all albums. If category or album is currently open the change is not 
         * seen by tests before current level is closed.
         */
        virtual TInt DeleteAllAlbumsL( CStifItemParser& aItem );

        /**
         * Adds medias to an album.
         *
         * @param aAlbumName album where medias are added.
         * @param aDrive drive where medias are
         * @param aStartIndex index of first media 
         * @param aEndIndex index of last media 
         */
        virtual TInt AddMediasToAlbumL( CStifItemParser& aItem );        

        /**
         * Removes medias from an album.
         *
         * @param aAlbumName album where medias are removed from.
         * @param aDrive drive where medias are
         * @param aStartIndex index of first media 
         * @param aEndIndex index of last media 
         */
        virtual TInt RemoveMediasFromAlbumL( CStifItemParser& aItem );
        
        /**
         * Creates a video file to the file system.
         * Test module keeps track of these files with iSideloadedFiles array.
         * If size is -1 video size will not be altered.
         * If category is open then this will wait until collection is refreshed with new data.
         */
        virtual TInt CreateVideoFileL( CStifItemParser& aItem );

        /**
         * Creates a video file to the file system.
         * Test module keeps track of these files with iSideloadedFiles array.
         * If size is -1 video size will not be altered.
         */
        virtual TInt CreateVideoFileNoWaitL( CStifItemParser& aItem );

        /**
         * Creates video file(s).
         */
        void CreateVideoFileL( CVCXMyVideosTestUtils::TVcxTestVideoType aType, const TDesC& aDriveLetter, const TDesC& aFileName, TInt aSize, TInt aVideoCount, TBool aDoSync );

        /**
         * Ensures that drive has specified amount of videos, they are created and removed if needed.
         * 
         * @param drive
         * @param amount of videos
         * @param filename for new videos
         */
        virtual TInt EnsureDriveForVideosL( CStifItemParser& aItem );
        
        /**
         * Deletes all videos created with CreateVideoFileL method.
         */
        virtual TInt EmptySideloadFoldersL( CStifItemParser& aItem );

        /**
         * Toggle deletion of created videos in the destructor.
         */
        virtual TInt SetDeleteSideloadedVideos( CStifItemParser& aItem );

        /**
         * Starts a wait for message or messages of same type.
         * STIF script is signaled when all of the messages have been
         * received.
         *
         * @param message type
         * @param timeout (optional. seconds, default is 5 seconds )
         * @param message count (optional)
         */
        virtual TInt WaitForMessageL( CStifItemParser& aItem );

        /**
         * Starts a wait for messages
         * STIF script is signaled when any of specified messages
         * is received.
         *
         * @param timeout ( seconds )
         * @param message type
         * @param message type2
         * @param message type3 etc...
         */
        virtual TInt WaitForAnyMessageL( CStifItemParser& aItem );

        /**
         * Starts a wait for messages.
         * STIF script is signaled when all of specified messages
         * have been received.
         *
         * @param timeout ( seconds )
         * @param message type
         * @param message type2
         * @param message type3 etc...
         */
        virtual TInt WaitForMessagesL( CStifItemParser& aItem );

        void WaitForMessagesL( TBool aBlock, RArray<TInt>& aMessages, TInt aTimeout, TBool aWaitAll );

        /**
         *  Other methods.
         */

        /**
         * Delete directory if it's empty.
         */
        TInt DeleteDirectoryIfEmpty( RFs& aFs, CFileMan& aFileMan, const TDesC& aDirectory );

        /**
         *
         */
        TInt MapMpxMediaAttribute( TVcxTestMediaAttributeIdMapping aMapping, TMPXAttributeData& aAttribute,
                TVcxTestMediaAttributeType& aType );

        /**
         *
         */
        void GetMessageDesc( TInt aMessage, TDes& aMsgBuff );

        /**
         * CheckMediasL
         *
         * aExpectedVideoCount
         * aDrive
         * aCheckType, 0 = exact, 1 no more than specified count, -1 no less than specified count.
         */
        void CheckMediasL( TInt aExpectedVideoCount, TInt aDrive, TInt aCheckType );

        /**
         * Gets drive parameter from aItem.
         */
        TInt GetDriveParam( CStifItemParser& aItem, TInt& aDrive );

        /**
         * MPX collection -> Close & Open( index ) & Open
         */
        void DoTotalRefreshL();
        
        /**
         * Waits ongoing refresh to finish, can be forced to wait for next refresh.
         */
        void WaitForRefreshL( TBool aForcedWait );
        
        /**
         * Gets medias for the specified level. 
         */
        CMPXMediaArray* GetMediasForLevelL( TInt aLevel, TUint aFlagsFilter );
        
    private:    // Data

        // Instance of class holding the test logic to test MV MPX collection.
        CVCXMyVideosCollectionPluginTester *iTester;

        // CVCXMyVideosTestUtils
        CVCXMyVideosTestUtils* iTestUtils;

        // RFs
        RFs iFs;
        
        // I'm a FileMan
        CFileMan* iFileMan;

        // Filenames created with CreateVideoFileL method are stored here.
        RPointerArray<HBufC> iSideloadedFiles;

        // Folders used for sideloading.
        RPointerArray<HBufC> iSideloadFolders;

        // CVcxTestActiveWait
        CVcxTestActiveWait* iActiveWait;

        // CVcxTestActiveWait
        CVcxTestActiveWait* iActiveWaitBlocking;

        TBool iDeleteSideloadedFilesAndFolders;

        // List of messages which are in currently waited.
        // Failed messages cause signal always.
        RArray<TInt> iWaitedTestMessages;

        TInt iLastReceivedMessage;

        TBool iWaitingForAllMessages;

        // Seconds to wait before message is got, default is 5
        TInt iMessageWaitTimeout;

        // Wait is not called from script but from test module.
        TBool iMessageWaitIsBlocking;

//#ifdef __WINSCW__
        RHarvesterClient iClient;
//#endif // __WINSCW__

        // Timeout timer
        CVcxTestTimer* iTimeoutTimer;

        TBuf<256> iCaseId;

        TBool iLoggingEnabled;

        CVcxTestTimer* iCoolDownTimer;

        TBool iCoolDownWait;

        TInt iEventError;

        CMPXMedia* iMpxMedia;

        TBool iFileCheckDisabled;

        CVCXMyVideosTestServiceEmu* iServiceEmu;

        CVCXTestMdsDbModifier* iMdsDbModifier;

        TInt iCurrentSortOrder;

        // To cancel move/copy/delete operations right after they start.
        TBool iCancelNextOperation;
        
        // This is for passing media count check when mpx plugin build is urel, 
        // which causes add media cmd to fail. 
        TInt iAddMediaCmdCount;
    };

#endif      // VCXMYVIDEOSCOLLECTIONPLUGINTEST_H

// End of File
