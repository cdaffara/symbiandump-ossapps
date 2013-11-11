/*
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
* Description: 
*
*/

/*glxmlwrapper.h*/

#ifndef GLXMLWRAPPER_H
#define GLXMLWRAPPER_H

#include <QObject>
#include <glxfiltergeneraldefs.h>
#include <QtGlobal>
#include <qmetatype.h>
#include <glxuistd.h>
#include <QSize>
#include <QVariant>

#include <qdatetime.h>
//forward declarations
class GlxMLWrapperPrivate;
class HbIcon;
class CFbsBitmap;
class QImage;
//constant declarations


#ifdef BUILD_MLWRAPPER
#define GLX_MLWRAPPER_EXPORT Q_DECL_EXPORT
#else
#define GLX_MLWRAPPER_EXPORT Q_DECL_IMPORT
#endif

Q_DECLARE_METATYPE(CFbsBitmap*)

class GLX_MLWRAPPER_EXPORT GlxMLWrapper : public QObject
{
    Q_OBJECT
    friend class TestGlxMLWrapper;
public:
	/**
	*constructor
	*/
    GlxMLWrapper(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType,QString uri = NULL,GlxFetcherFilterType fetcherFilterType = EGlxFetcherFilterNone);
	/**
	*destructor
	**/
	~GlxMLWrapper();
    /**
     * for setting the attribute context mode will be used mainly for retreiving thumbnail of different sizes.
     */
    void setContextMode(GlxContextMode contextMode);
    /**
      * for removing the contect once it is not rquired anymore
      */
    void removeContextMode(GlxContextMode contextMode);
    
     /**
     * for retreiving the data Count.
     */
    int getItemCount();
	/**
	* retrieveItemIcon()
	*/
	HbIcon* retrieveItemIcon(int index, GlxTBContextType aTBContextType);
	QImage retrieveItemImage(int index, GlxTBContextType aTBContextType);
    void itemsAdded(int startIndex,int endIndex);
    void itemsRemoved(int startIndex,int endIndex);
    void handleReceivedIcon(int itemIndex, GlxTBContextType tbContextType);
    void handleIconCorrupt(int itemIndex);
    void handleGeneralError(int error);
	void handleListItemAvailable(int itemIndex);
	void handleDetailsItemAvailable(int itemIndex);
    int getFocusIndex() const;
    void setFocusIndex(int itemIndex);
    
    void setSelectedIndex(int aItemIndex);

	QString retrieveListTitle(int index);
	QString retrieveListSubTitle(int index);
	/*
	 * fetches the comments entered for the image
	 */
	QString retrieveListDesc(int index);
    int getVisibleWindowIndex();
    void setVisibleWindowIndex(int itemIndex);
	QString retrieveItemUri(int index);
	QSize retrieveItemDimension(int index);
	/*
	 * fetches the size for the image
	 */
	int   retrieveItemSize(int index);
    /*
	 * fetches the date for the image
	 */	
	QDate retrieveItemDate(int index);
	/*
	 * fetches the time for the image
	 */	
	QTime retrieveItemTime(int index);
	int retrieveItemFrameCount(int index);
	QString retrieveViewTitle();
	void handlepopulated();	
	bool IsPopulated();
	bool IsDrmProtected(int index = -1);	
	bool IsDrmValid(int index);
	void setDrmValid(int index,bool valid);			
	
	/*
	 * retriveBitmap helps to retrive the bitmap
	 * @param1 index 
	 */
	QVariant RetrieveBitmap(int index);
    /*
     * retrieveListItemCount helps to retrive the number of images in the album
     * @param1 index 
     */
	int  retrieveListItemCount(int index);
    /*
     * isSystemItem helps to get the system item information
     * @param1 index 
     */
	bool isSystemItem( int aItemIndex );
	void handleTitleAvailable(QString aTitle);
	
    bool isCorruptedImage( int index );
signals:
     void updateItem(int index, GlxTBContextType tbContextType);
	 void insertItems(int startIndex,int endIndex);
	 void removeItems(int startIndex,int endIndex);
	 void itemCorrupted(int itemIndex);
	 void updateAlbumTitle(QString aTitle);
	 void populated();
 	/*
	 * emits the signal to update details view
	 */	
	 void updateDetails();

private:
	GlxMLWrapperPrivate* mMLWrapperPrivate;
};

#endif //GLXMLWRAPPER_H
