
/**
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
* Description:   Stub videocollectionclient class for videocollectionsortfilterproxy unit tests
*
*/

#ifndef __VIDEOCOLLECTIONCLIENT_H
#define __VIDEOCOLLECTIONCLIENT_H

//#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <qstring.h>
#include <qlist.h>

// FORWARD DECLARATIONS
class VideoCollectionUtils;


class  VideoCollectionClient
{

    public: // Constructors and destructor

        /**
         * constructor
         *
         */
    VideoCollectionClient();

        /**
         * destructor
         *
         */
    virtual ~VideoCollectionClient();

    public:

    /**
     *
     * if mFailInit is true returns -1
     * if mFailInit is false returns 0
     *
     * @return int
     */
    int initialize();

    /**
     *
     * if mFailStartOpen is true returns -1
     * if mFailStartOpen is false returns 0
     *
     * @return int
     */
    int startOpenCollection(int level);

    /**
     *
     * if mFailMediaPlayback is true returns -1
     * if mFailMediaPlayback is false returns 0
     *
     * @return int
     */
    int openItem(int mpxId1);
   
    /**
     *
     * if mFailMediaDetails is true returns -1
     * if mFailMediaDetails is false returns 0
     *
     * @return int
     */
    int getVideoDetails(int mpxId1);

    /**
     * @return mNewAlbumId
     */
    TMPXItemId addNewAlbum(QString name);
    
    /**
     * no funtionality needed here for unit tests
     */
    int addItemsInAlbum(TMPXItemId albumId, QList<TMPXItemId> items);
    
    /**
     * no funtionality needed here for unit tests
     */
    int removeAlbums(const QList<TMPXItemId> &items);
    
    /**
     * saves contant id (1,2) into provided id
     */
    void getCategoryId(TMPXItemId &id);
    
    /**
     * calls collection to go back to collection level
     * 
     */
    int back();
    
    /**
     * returns mRenameItemReturnValue
     */
    int renameItem(const TMPXItemId &itemId, const QString &newTitle);    

public:
    static bool mFailInit;
    static bool mFailStartOpen;
    static bool mFailMediaPlayback;
    static bool mFailMediaDetails;
    static bool mFailSetSort;
    static bool mFailAddNewCollection;
    static TMPXItemId mNewAlbumId;
    static TVcxMyVideosSortingOrder mSettedSortOrder;
    static int mSortOrderSetCount;
    static QString mAddNewCollectionName;
    static QString mAddNewCollectionThumb;
    static QList<TMPXItemId> mAddNewCollectionIds;
    static int mRenameItemReturnValue;
    static int mBackReturnValue;
    
private:

    /**
     * static instance from this class
     */
    static VideoCollectionClient *testInstance;
};

#endif // __VIDEOCOLLECTIONCLIENT_H
