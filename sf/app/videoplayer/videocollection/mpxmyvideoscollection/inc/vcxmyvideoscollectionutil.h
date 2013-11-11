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



#ifndef VCXMYVIDEOSCOLLECTIONUTIL_H
#define VCXMYVIDEOSCOLLECTIONUTIL_H

// INCLUDES
#include <vcxmyvideosdefs.h>

// FORWARD DECLARATIONS
class CMPXMedia;
class RFs;

// CONSTANTS

// CLASS DECLARATION

/**
* Util functions.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(TVcxMyVideosCollectionUtil)
    {
    public:
    
        /**
        * Creates media container and sets up empty media array to it.
        *
        * @return CMPXMedia media list.
        */
        static CMPXMedia* CreateEmptyMediaListL();

        /**
        * Creates message list and sets up empty message array to it.
        *
        * @return CMPXMessage message list.
        */
        static CMPXMedia* CreateEmptyMessageListL();
        
        /**
        * Tries to find aMdsIds from aFromList and if found, copies them
        * to aToList. KMPXMediaArrayContents arrays must be set for aFromList
        * and aToList, othewise leave (KErrArgument) occurs.
        *
        * @param aFromList Items are copied from here.
        * @param aToList   Items are copied here.
        * @param aMdsIds   Item IDs which are copied.
        */
        static void CopyFromListToListL(
                CMPXMedia& aFromList,
                CMPXMedia& aToList,
                RArray<TUint32>& aMdsIds );

        /**
        * Tries to find aMdsIds from aFromArray and if found, copies them
        * to aToList. KMPXMediaArrayContents arrays must be set for aToList,
        * othewise leave (KErrArgument) occurs.
        *
        * @param aFromArray Items are copied from here.
        * @param aToList    Items are copied here.
        * @param aMdsIds    Item IDs which are copied.
        */
        static void CopyFromListToListL(
                RArray<CMPXMedia*>& aFromArray,
                CMPXMedia& aToList,
                RArray<TUint32>& aMdsIds );

        /**
        * Appends all items from aFromList to aToList. New items will point to
        * same shared memory.
        *
        * @param aToList   List in which items are appended to.
        * @param aFromList List in which items are copied from.
        */
        static void AppendToListL( CMPXMedia& aToList, CMPXMedia& aFromList );

        /**
        * Converts drive letter to drive number.
        *
        * @param aDrive drive letter
        * @return       drive number
        */
        static TInt DriveLetter2DriveNumber( const TDesC &aDrive );

        /**
        * Checks that file system does not have aPath file already, if it does
        * then the aPath is changed to unique by adding digits to it. Result
        * is stored to aUniquePath, (event if it is the same as aPath).
        *
        * @param aFs          Open session to file server.
        * @param aPath        File name to make unique.
        * @param aUniquePath  Generated unique path.
        */
        static void MakeUniqueFileNameL( RFs& aFs, const TDesC& aPath, TDes& aUniquePath );
        
        /**
        * Gets KVcxMediaMyVideosOrigin attribute of aVideo.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        Origin, see values from TVcxMyVideosOrigin in vcxmyvideosdefs.h
        */
        static TUint8 OriginL( CMPXMedia& aVideo );

        /**
        * Gets KMPXMediaGeneralFlags attribute of aVideo.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        Flags, see values from TVcxMyVideosVideoFlags in vcxmyvideosdefs.h
        */        
        static TUint32 FlagsL( CMPXMedia& aVideo );

        /**
         * Gets KMPXMediaGeneralDate (creation date) of aVideo.
         * 
         * @param aVideo  Media object to fetch parameter from.
         * @return        Creation date, works with TTime.
         */
        static TInt64 CreationDateL( CMPXMedia& aVideo );

        /**
        * Gets KVcxMediaMyVideosDownloadId attribute of aVideo.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        Download ID in Download Manager.
        */        
        static TUint32 DownloadIdL( CMPXMedia& aVideo );

        /**
        * Gets KVcxMediaMyVideosDownloadState attribute of aVideo.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        Download state.
        */        
        static TVcxMyVideosDownloadState DownloadStateL( CMPXMedia& aVideo );

        /**
        * Gets KMPXMediaGeneralId attribute of aVideo.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        ID.
        */
        static TMPXItemId IdL( const CMPXMedia& aVideo );

        /**
        * Gets KVcxMediaMyVideosDuration attribute of aVideo.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        Duration.
        */        
        static TReal32 DurationL( CMPXMedia& aVideo );
        
        /**
        * Gets KMPXMediaGeneralTitle attribute of the video.
        *
        * @return  KMPXMediaGeneralTitle value. If attribute is not supported,
        *          then KNullDesC is returned.
        */
        static const TDesC& Title( const CMPXMedia& aVideo );

        /**
        * Gets KVcxMediaMyVideosRating attribute of the video.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        KVcxMediaMyVideosRating value. If attribute is not supported,
        *                then 0 is returned.
        */        
        static TUint8 RatingL( CMPXMedia& aVideo );

        /**
        * Gets KVcxMediaMyAudioFourCc attribute of the video.
        *
        * @param aVideo  Media object to fetch parameter from.
        * @return        KVcxMediaMyAudioFourCc value. If attribute is not supported,
        *                then 0 is returned.
        */
        static TUint32 AudioFourCcL( CMPXMedia& aVideo );

        /**
        * Checks if aAttrs contains attributes which are not supported in aVideo.
        *
        * @param aVideo   Attributes to check from.
        * @param aAttrs   Attributes to check.
        * @param aNonSupportedAttrCanBeFoundFromMds  Set to ETrue if at least one of non supported
        *                                            attributes can be found from MDS.
        * @return         ETrue if all aAttrs attributes are supported in aVideo, EFalse
        *                 otherwise.
        */
        static TBool AreSupported( CMPXMedia& aVideo, const TArray<TMPXAttribute>& aAttrs,
                TBool& aNonSupportedAttrCanBeFoundFromMds );

        /**
        * Checks if aAttrs belongs to "full" set of parameters which is saved to MDS but not
        * cached by collection.
        *
        * @param aAttr    Attribute to check.
        * @return         ETrue if aAttr belongs to full set and can be found from MDS, EFalse
        *                 otherwise.
        */
        static TBool AttrBelongsToFullSet( const TMPXAttribute& aAttr );
        
        /**
        * Converts collection category ID to origin.
        *
        * @param aCategoryId  Category ID
        * @return             Returns Origin or KErrNotFound if not found. KVcxMvcCategoryIdAll
        *                     returns also KErrNotFound.
        */
        static TInt Origin( TInt aCategoryId );

        /**
         * Gets KMPXMediaArrayContents attribute from aMedia. Leaves
         * with KErrArgument is attribute is not supported.
         * 
         * @param aMedia  Media where from the attribute is fetched.
         * @return        Pointer to media array.
         */
        static CMPXMediaArray* MediaArrayL( const CMPXMedia& aMedia );
        
        /**
         * Gets KVcxMediaMyVideosInt32Value attribute from aMedia. Leaves
         * with KErrArgument if attribute is not supported.
         * 
         * @param aMedia  Media where from the attribute is fetched.
         * @return        Value of KVcxMediaMyVideosInt32Value attribute.
         */
        static TInt32 Int32ValueL( CMPXMedia& aMedia );

        /**
         * Gets KVcxMediaMyVideosUint32Value attribute from aMedia. Leaves
         * with KErrArgument if attribute is not supported.
         * 
         * @param aMedia  Media where from the attribute is fetched.
         * @return        Value of KVcxMediaMyVideosUint32Value attribute.
         */
        static TUint32 Uint32ValueL( CMPXMedia& aMedia );
        
        /**
         * Gets KVcxMediaMyVideosCategoryItemCount attribute from aVideo.
         *
         * @param aMedia  Media where from the attribute is fetched.
         * @return        Returns 0, if attribute is not supported.
         */
        static TUint32 CategoryItemCountL( CMPXMedia& aVideo );

        /**
         * Gets KVcxMediaMyVideosCategoryNewItemCount attribute from aVideo.
         * 
         * @param aMedia  Media where from the attribute is fetched.
         * @return        Returns 0, if attribute is not supported.
         */
        static TUint32 CategoryNewItemCountL( CMPXMedia& aVideo );
        
        /**
         * Gets KVcxMediaMyVideosCategoryNewItemName attribute from aVideo.
         *
         * @param aMedia  Media where from the attribute is fetched.
         * @return        Category new video name.
         */
        static const TDesC& CategoryNewVideoName( const CMPXMedia& aVideo );
        
        /**
         * Gets KMPXMediaGeneralId attributes from aMediaArray and
         * puts them to aIdArray. aIdArray is reset before adding.
         * 
         * @param aMediaArray  Media array containing media items which
         *                     have KMPXMediaGeneralId attribute set.
         * @param aIdArray     Array where to the IDs are written to.
         */
        static void GetIdsFromMediaArrayL( CMPXMediaArray& aMediaArray,
                RArray<TUint32>& aIdArray );

#ifdef _DEBUG
        /**
        * Prints process names which have file handle open to aFileName
        *
        * @param aFileName  File to check for open file handles.
        */
        static void PrintOpenFileHandlesL( const TDesC& aFileName, RFs& aFs );
        
        /**
        * Gets process name.
        *
        * @param aThreadId    Thread ID
        * @param aProcessName Upon completion process name is written here.
        */
        static void GetProcessName( TInt aThreadId, TFullName& aProcessName );

    private:
        TBuf<500> iProcessName;        
#endif
    };
#endif   // VCXMYVIDEOSCOLLECTIONUTIL_H


