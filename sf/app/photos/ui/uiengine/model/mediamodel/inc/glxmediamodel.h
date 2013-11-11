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
* Description:   ?Description
*
*/



#ifndef GLXMEDIAMODEL_H
#define GLXMEDIAMODEL_H

#include <QAbstractItemModel>
#include <glxuistd.h>
#include <QDateTime>
#include <glxexternalinterfacedefs.h>
//forward declarations

#ifdef BUILD_MEDIAMODEL
#define GLX_MEDIAMODEL_EXPORT Q_DECL_EXPORT
#else
#define GLX_MEDIAMODEL_EXPORT Q_DECL_IMPORT
#endif

class HbIcon;
class GlxModelParm;
class GlxMLWrapper;
class GlxDRMUtilityWrapper;
#include <QCache>
#include <QVector>

class GLX_MEDIAMODEL_EXPORT GlxMediaModel : public QAbstractItemModel
{
Q_OBJECT
friend class TestGlxMediaModel;

public :	
    /**
     * Constructor
     * @param - model parm struct collectionID, depth, filter, context mode etc
     */	
    GlxMediaModel( GlxModelParm & modelParm );

    /**
     * Destructor
     */
    ~GlxMediaModel();
    
    /**
     * rowCount() - return the number of rows in the model.
     * @param - parent model index, not used
     */
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    
    /**
     * columnCount() - return 1
     * @param - parent model index, not used
     */
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    
    /**
     * parent() - return the model index of the child.
     * It always return invalid model index since no there is no parent
     */
    QModelIndex parent( const QModelIndex &child ) const;
    
    QVariant itemImageData( const QModelIndex &index, int role ) const;
    QVariant collectionProperty( int role ) const;
    QVariant itemProperty( const QModelIndex &index, int role ) const;    
    
    /**
     * data() -  return the data of corresponding role.
     * @param - model index of interested item.
     * @param - role of corresponding data.
     */
    QVariant data( const QModelIndex &index, int role ) const;
    
    /**
     * setData() - to set the data for corresponding item and role.
     * @param - model index of interested item.
     * @param - data of the item
     * @param - role of corresponding data.
     */
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    
    /**
     * index -  create a new model index for an item.
     * @param - row index of the item.
     * @param - column index of the item, it is always one.
     */
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;

	
signals :
    /**
     * albumTitleAvailable() - emit this signal when album title is available.
     * @parma - album title
     */
	void albumTitleAvailable( QString aTitle );
	
	/**
	 * populated() - emit this signal once medialist is populated
	 */
	void populated();
	
	/**
	 * thumbnailPopulated() - emit the signal once some initial page of thumbnail are loaded 
	 * into cache. 
	 */
	void thumbnailPopulated();
	
	/**
	 * iconAvailable() - emit the signal to update the icon in the cache.
	 * @param - item index
	 * @param - HbIcon
	 * @param - media list context type
	 */
    void iconAvailable( int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType ) const;
    
    /**
     * updateDetailsView() - emit the signal to update the details view
     */
    void updateDetailsView();
    
    /**
     * fullScreenDataChanged() - emit the signal when fullscreen thumbnail is updated.
     * @param - start index of updated data
     * @param - end index of updated data
     */    
    void fullScreenDataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );

private:
    /**
     * GetGridIconItem() - To fetch the grid size thumbnail
     * @param - item index
     * @param - media list context type
     * @return - return the hbicon of the item
     */
	HbIcon* GetGridIconItem( int itemIndex, GlxTBContextType tbContextType ) const;

    /**
     * GetFsIconItem() - To fetch the full screen size thumbnail
     * @param - item index
     * @param - media list context type
     * @return - return the hbicon of the item
     */	
	HbIcon* GetFsIconItem( int itemIndex, GlxTBContextType tbContextType )const;
	
	HbIcon gridToFullscreenIcon( HbIcon * icon, int itemIndex ) const;
	
	HbIcon getFullScreenIcon( int itemIndex ) const;
	
	QImage getFullScreenImage( int itemIndex ) const;

    /**
     * setContextMode() - for setting the attribute context mode will be used mainly for 
     * retreiving thumbnail of different sizes.
     */
    void setContextMode( GlxContextMode contextMode );
    
	/**
	 *removeContextMode() - Removes the context
	 */
    void removeContextMode( GlxContextMode contextMode );
    
    /**
     * setFocusIndex() -  To set the focus index in medialist
     * @param - selected item index( focus index )
     */
    void setFocusIndex( const QModelIndex &index );
    
    /**
     * getFocusIndex() - Retrun the focus index from the medialist.
     * @return -  focus index.
     */
    QModelIndex getFocusIndex() const;
    
    /**
     * setSelectedIndex() - To make the item as seleted in media list to perform some coomand.
     * @param - selected item index
     */
    void setSelectedIndex( const QModelIndex &index );

    /**
     * getCorruptDefaultIcon() - IT will return the corrupt icon if item is corrupted else return the 
     * default item.
     * @param - item index 
     */    
    HbIcon *getCorruptDefaultIcon ( const QModelIndex &index ) const;
    
    /**
     * thumbnailPopulatedCheck() - To check the some initial page of item is loaded into cache or not
     * @param -  loaded thumbnail image index in cache.
     */
    void thumbnailPopulatedCheck( int index );
    
public slots:
    /**
     * itemUpdated() - call back fuction, when thumbnail is added into cache.
     * It remove the icon from the local cache and emit the signal of data change.
     * @param - item index
     * @param - media list context type 
     */
	void itemUpdated( int mlIndex, GlxTBContextType tbContextType );
	
	/**
	 * itemsAdded() - call back function when new items are added into collection.
	 * @param - start index of the newly added item
	 * @param - end endex of the newlyadd item
	 */
	void itemsAdded( int startIndex, int endIndex );
	
	/**
	 * itemsRemoved() - call back function when some item are removed from the collection.
     * @param - start index of the remove items
     * @param - end endex of the remove items
	 */
	void itemsRemoved( int startIndex, int endIndex );
	
	/**
	 * itemCorrupted() - call back function, when a item is corrupted.
	 * @param - index of the corrupt item
	 */
	void itemCorrupted( int itemIndex );
	
    /**
     * albumTitleUpdated() - call back function, when a album collection title is available.
     * @param - title of the album.
     */	
	void albumTitleUpdated( QString aTitle );
	
	/**
	 * modelpopulated() - callback function, when meida list item is populated.
	 * It set the visual window index and emit the signal to view about it.
	 */
	void modelpopulated();
	
	/**
	 * updateDetailItems() - Emits signal to update the details view
	 */
	void updateDetailItems();
	
private slots:
    /**
     * updateItemIcon() - update the icon into local cache.
     * @param - item index
     * @param - HbIcon
     * @param - media list context type
     */
    void updateItemIcon( int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType );
    
private:
	GlxMLWrapper* mMLWrapper;
    QCache<int, HbIcon> itemIconCache;
    QCache<int, HbIcon> itemFsIconCache;
    HbIcon* m_DefaultIcon;
    HbIcon* m_CorruptIcon;
    GlxContextMode mContextMode;
    int mFocusIndex;
    int mSubState;
    GlxDRMUtilityWrapper* mDRMUtilityWrapper;
    int mTempVisibleWindowIndex;
    bool thumbnailPopulatedFlag;
    bool mPaintPage;
};


#endif /* GLXMEDIAMODEL_H_ */
