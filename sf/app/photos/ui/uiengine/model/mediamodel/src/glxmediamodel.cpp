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

#include <glxmediamodel.h>
#include <hbicon.h>
#include <qimage.h>
#include <glxmodelparm.h>
#include <QCache>
#include <QDebug>
#include <glxmlwrapper.h>
#include"glxdrmutilitywrapper.h"

#include <glxfiltergeneraldefs.h>

#include <glximageviewermanager.h>
//#define GLXPERFORMANCE_LOG  
#include <glxperformancemacro.h>

#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxcollectionpluginall.hrh"
#include "glxviewids.h"

#define DELTA_IMAGE 5
#define INITIAL_POPULATE_IMAGE_COUNT 30

GlxMediaModel::GlxMediaModel( GlxModelParm & modelParm )
{
    qDebug( "GlxMediaModel::GlxMediaModel" );

    thumbnailPopulatedFlag = modelParm.collection() == KGlxCollectionPluginAllImplementationUid
        ? false : true;
    mPaintPage = modelParm.collection() == KGlxCollectionPluginAllImplementationUid ? true : false;
    mMLWrapper = new GlxMLWrapper( modelParm.collection(), 0, modelParm.filterType(), NULL,
        modelParm.fetcherFiterType() );
    mMLWrapper->setContextMode( modelParm.contextMode() );
    mContextMode = modelParm.contextMode();
    mDRMUtilityWrapper = new GlxDRMUtilityWrapper();

    int err = connect( mMLWrapper, SIGNAL(updateItem(int, GlxTBContextType)), this,
        SLOT(itemUpdated(int, GlxTBContextType)) );
    qDebug( "updateItem() connection status %d", err );
    err = connect( mMLWrapper, SIGNAL(itemCorrupted(int)), this, SLOT(itemCorrupted(int)) );
    qDebug( "itemCorrupted() connection status %d", err );
    err = connect( mMLWrapper, SIGNAL(insertItems(int, int)), this, SLOT(itemsAdded(int, int)) );
    qDebug( "updateItem() connection status %d", err );
    err = connect( mMLWrapper, SIGNAL(removeItems(int, int)), this, SLOT(itemsRemoved(int, int)) );
    qDebug( "updateItem() connection status %d", err );
    err = connect( this, SIGNAL(iconAvailable(int, HbIcon*, GlxTBContextType)), this,
        SLOT(updateItemIcon(int, HbIcon*, GlxTBContextType)) );
    qDebug( "iconAvailable() connection status %d", err );
    err = connect( mMLWrapper, SIGNAL(updateAlbumTitle(QString)), this,
        SLOT(albumTitleUpdated(QString)) );
    qDebug( "updateAlbumTitle() connection status %d", err );
    err = connect( mMLWrapper, SIGNAL(populated()), this, SLOT(modelpopulated()) );
    err = connect( mMLWrapper, SIGNAL(updateDetails()), this, SLOT(updateDetailItems()) );

    itemIconCache.setMaxCost( 20 ); //Changed While Doing Media Wall
    itemFsIconCache.setMaxCost( 5 );

    m_DefaultIcon = new HbIcon( GLXICON_DEFAULT );
    m_CorruptIcon = new HbIcon( GLXICON_CORRUPT );
    mFocusIndex = -1;
    mSubState = -1;
    mTempVisibleWindowIndex = 0;
}

GlxMediaModel::~GlxMediaModel()
{
    //itemadded.clear();
    qDebug( "GlxMediaModel::~GlxMediaModel" );
    itemIconCache.clear();
    itemFsIconCache.clear();
    delete m_DefaultIcon;
    m_DefaultIcon = NULL;
    delete m_CorruptIcon;
    m_CorruptIcon = NULL;
    delete mMLWrapper;
    delete mDRMUtilityWrapper;
}

//------------------------------------------------------------------------------------------------------------
//setContextMode
//------------------------------------------------------------------------------------------------------------
void GlxMediaModel::setContextMode( GlxContextMode contextMode )
{
    if ( contextMode == GlxContextComment ) {
        mMLWrapper->setContextMode( contextMode );
    }
    else {
        if ( mContextMode != contextMode ) {

            itemFsIconCache.clear();
            mMLWrapper->setContextMode( contextMode );
            mContextMode = contextMode;
            if ( mContextMode == GlxContextLsGrid || mContextMode == GlxContextPtGrid ) {
                itemIconCache.clear();
                //emit dataChanged( index( 0, 0), index( rowCount() - 1, 0) );  // Not Needed for HgWidget
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------
//removeContextMode
//------------------------------------------------------------------------------------------------------------
void GlxMediaModel::removeContextMode( GlxContextMode contextMode )
{
    mMLWrapper->removeContextMode( contextMode );
}

int GlxMediaModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    qDebug( "GlxMediaModel::rowCount %d", mMLWrapper->getItemCount() );
    return mMLWrapper->getItemCount();
}

int GlxMediaModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex GlxMediaModel::parent( const QModelIndex &child ) const
{
    Q_UNUSED(child);

    //returns always invalid model index
    return QModelIndex();
}


QVariant GlxMediaModel::itemImageData( const QModelIndex &index, int role ) const
{
    HbIcon* itemIcon = NULL;
    QImage itemImage;
    int itemIndex = index.row();

    if ( role == Qt::DisplayRole ) {
        return QVariant();
    }

    if ( role == GlxHgVisibilityRole ) {
        return TRUE;
    }

    if ( role == GlxDefaultImage ) {
        // this image Creation is Slow. But what to do, Q class's Does not undersatnd our Localised File names
        return m_DefaultIcon->pixmap().toImage().convertToFormat( QImage::Format_ARGB32_Premultiplied );
    }

    qDebug( "GlxMediaModel::data buffer concept index %d role %d", index.row(), role );
    if ( ( !index.isValid() ) || ( index.row() >= rowCount() ) ) {
        return QVariant();
    }

    //retrieve Data from Media List     
    if ( role == Qt::DecorationRole ) {
        itemIcon = GetGridIconItem( itemIndex, GlxTBContextGrid );
        if ( itemIcon == NULL || itemIcon->isNull() ) {
            itemIcon = getCorruptDefaultIcon( index );
        }
        return *itemIcon;
    }

    if ( role == GlxQImageSmall ) {
        return mMLWrapper->retrieveItemImage( itemIndex, GlxTBContextGrid );
    }

    if ( role == GlxFsImageRole ) {
        return getFullScreenIcon( itemIndex );
    }
    
    if ( role == GlxQImageLarge ) {
        return getFullScreenImage( itemIndex );
    }
    
    if ( role == GlxHdmiBitmap ) {
        return mMLWrapper->RetrieveBitmap( itemIndex );
    } 
    
    return QVariant();
}

QVariant GlxMediaModel::collectionProperty(  int role ) const
{
    QModelIndex idx;
    if ( GlxFocusIndexRole == role ) {
        idx = getFocusIndex();
        return idx.row();
    }
    
    if ( role == GlxSubStateRole ) {
        return mSubState;
    }
    
    if ( role == GlxPopulated ) {
        return mMLWrapper->IsPopulated();
    }
    
    if ( role == GlxViewTitle ) {
        return mMLWrapper->retrieveViewTitle();
    }     

    if ( role == GlxVisualWindowIndex ) {
        return mMLWrapper->getVisibleWindowIndex();
    }

    if ( role == GlxPaintPageFlag ) {
        return mPaintPage;
    }
    
    return QVariant();
}

QVariant GlxMediaModel::itemProperty( const QModelIndex &index, int role ) const
{
    if ( ( !index.isValid() ) || ( index.row() >= rowCount() ) ) {
        return QVariant();
    }
    
    int itemIndex = index.row();

    if ( role == GlxFrameCount ) {
        return ( mMLWrapper->retrieveItemFrameCount( itemIndex ) );
    }

    if ( role == GlxImageCorruptRole ) {
        return mMLWrapper->isCorruptedImage( itemIndex );
    }

    if ( role == GlxUriRole ) {
        return ( mMLWrapper->retrieveItemUri( itemIndex ) );
    }
    
    if ( role == GlxSizeRole ) {
        return mMLWrapper->retrieveItemSize( itemIndex );
    }
    
    if ( role == GlxImageTitle ) {
        return mMLWrapper->retrieveListTitle( itemIndex );
    }

    if ( role == GlxDimensionsRole ) {
        return ( mMLWrapper->retrieveItemDimension( itemIndex ) );
    }

    if ( role == GlxDateRole ) {
        return ( mMLWrapper->retrieveItemDate( itemIndex ) );
    }

    if ( role == GlxTimeRole ) {
        return mMLWrapper->retrieveItemTime( itemIndex );
    }
    
    if ( role == GlxDescRole ) {
        return mMLWrapper->retrieveListDesc( itemIndex );
    }
    
    return QVariant(); 
    
}

QVariant GlxMediaModel::data( const QModelIndex &index, int role ) const
{
    if ( role < GlxItemImageDataEnd ) {
        return itemImageData( index, role );
    }
    
    if ( role < GlxCollectionPropertyEnd ) {
        return collectionProperty( role );
    }
    
    if ( role < GlxItemPropertyEnd ) {
        return itemProperty( index, role);
    }
    
    return QVariant();
}

//GetGridIconItem
HbIcon* GlxMediaModel::GetGridIconItem( int itemIndex, GlxTBContextType tbContextType ) const
{
    HbIcon* itemIcon = itemIconCache[ itemIndex ]; //Initialize icon from the Cache will be NULL if Item not present
    if ( !itemIcon ) {
        itemIcon = mMLWrapper->retrieveItemIcon( itemIndex, tbContextType );
        if ( itemIcon ) {
            emit iconAvailable( itemIndex, itemIcon, GlxTBContextGrid );
        }
        itemIcon = itemIconCache[ itemIndex ];
    }
    return itemIcon;

}

HbIcon* GlxMediaModel::GetFsIconItem( int itemIndex, GlxTBContextType tbContextType ) const
{
    HbIcon* itemIcon = NULL; //Initialize icon from the Cache will be NULL if Item not present

    bool drmProtected = mMLWrapper->IsDrmProtected( itemIndex );
    if ( drmProtected ) {
        QString imagePath = mMLWrapper->retrieveItemUri( itemIndex );
        bool rightValid = mMLWrapper->IsDrmValid( itemIndex );
        if ( !rightValid ) {
            //show error note here
            if ( itemIndex == mFocusIndex ) {
                mDRMUtilityWrapper->ShowRightsInfo( imagePath );
            }
            return itemIcon;
        }
        else {
            //consumeDrmRights(imagePath);
            mDRMUtilityWrapper->ConsumeRights( imagePath );
            bool rightValid = mDRMUtilityWrapper->ItemRightsValidityCheck( imagePath, true );
            mMLWrapper->setDrmValid( itemIndex, rightValid );
        }
    }
    
    itemIcon = itemFsIconCache[ itemIndex ]; //Initialize icon from the Cache will be NULL if Item not present
    if ( !itemIcon ) {
        itemIcon = mMLWrapper->retrieveItemIcon( itemIndex, tbContextType );
        if ( itemIcon ) {
            emit iconAvailable( itemIndex, itemIcon, tbContextType );
        }
        itemIcon = itemFsIconCache[ itemIndex ];
    }
    return itemIcon;
}

HbIcon GlxMediaModel::gridToFullscreenIcon( HbIcon * icon, int itemIndex ) const
{
    qDebug( "GlxMediaModel::gridToFullscreenIcon" );
    QPixmap tempPixmap = icon->qicon().pixmap( 128, 128 );
    QSize itemSize = mMLWrapper->retrieveItemDimension( itemIndex );
    QSize sz = ( mContextMode == GlxContextLsFs ) ? QSize( 640, 360 ) : QSize( 360, 640 );
    if ( !( ( itemSize.width() < sz.width() ) && ( itemSize.height() < sz.height() ) ) ) {
        itemSize.scale( sz, Qt::KeepAspectRatio );
    }
    tempPixmap = tempPixmap.scaled( itemSize, Qt::IgnoreAspectRatio );
    return HbIcon( QIcon( tempPixmap ) );
}

HbIcon GlxMediaModel::getFullScreenIcon( int itemIndex ) const
{
    HbIcon* itemIcon = NULL;
    if ( mContextMode == GlxContextLsFs ) {
        itemIcon = GetFsIconItem( itemIndex, GlxTBContextLsFs );
    }
    else if ( mContextMode == GlxContextPtFs ) {
        itemIcon = GetFsIconItem( itemIndex, GlxTBContextPtFs );
    }

    if ( itemIcon == NULL ) {
        HbIcon* tempIcon = GetGridIconItem( itemIndex, GlxTBContextGrid );
        if ( tempIcon && !tempIcon->isNull() ) {
            HbIcon tmp = gridToFullscreenIcon( tempIcon, itemIndex );
            if ( !tmp.isNull() ) {
                return tmp;
            }
        }
    }

    if ( itemIcon == NULL || itemIcon->isNull() ) {
        itemIcon = getCorruptDefaultIcon( index( itemIndex, 0 ) );
    }
    return *itemIcon;
}

QImage GlxMediaModel::getFullScreenImage( int itemIndex ) const
{
    QImage itemImage;
    if ( mContextMode == GlxContextLsFs ) {
        itemImage = mMLWrapper->retrieveItemImage( itemIndex, GlxTBContextLsFs );
    }
    else {
        itemImage = mMLWrapper->retrieveItemImage( itemIndex, GlxTBContextPtFs );
    }
    if ( !itemImage.isNull() ) {
        return itemImage;
    }
    else {
        itemImage = mMLWrapper->retrieveItemImage( itemIndex, GlxTBContextGrid );
        if ( !itemImage.isNull() ) {
            QSize sz = ( mContextMode == GlxContextLsFs ) ? QSize( 640, 360 ) : QSize( 360, 640 );
            itemImage = itemImage.scaled( sz, Qt::KeepAspectRatio );
        }
        return itemImage;
    }    
}

void GlxMediaModel::itemUpdated( int itemIndex, GlxTBContextType tbContextType )
{
    qDebug( "GlxMediaModel::itemUpdated %d", itemIndex );
    //clear the grid and FS cache if they have any icons with them for that index
    if ( tbContextType == GlxTBContextGrid ) {
        if ( !thumbnailPopulatedFlag ) {
            thumbnailPopulatedCheck( itemIndex );
        }
        itemIconCache.remove( itemIndex );
        emit dataChanged( index( itemIndex, 0 ), index( itemIndex, 0 ) );
    }
    if ( tbContextType == GlxTBContextLsFs || tbContextType == GlxTBContextPtFs ) {
        itemFsIconCache.remove( itemIndex );
        emit fullScreenDataChanged( index( itemIndex, 0 ), index( itemIndex, 0 ) );
    }
}

void GlxMediaModel::itemCorrupted( int itemIndex )
{
    qDebug( "GlxMediaModel::itemCorrupted %d", itemIndex );
    if ( !thumbnailPopulatedFlag ) {
        thumbnailPopulatedCheck( itemIndex );
    }
    emit dataChanged( index( itemIndex, 0 ), index( itemIndex, 0 ) );
    emit fullScreenDataChanged( index( itemIndex, 0 ), index( itemIndex, 0 ) );
}

void GlxMediaModel::modelpopulated()
{
    if ( mTempVisibleWindowIndex != -1 ) {
        //Set the visible Window index only ff the index stored in the activity manager is not out of range
        if ( rowCount() > mTempVisibleWindowIndex && mTempVisibleWindowIndex > 0 ) {
            mMLWrapper->setVisibleWindowIndex( mTempVisibleWindowIndex );
        }
        else {
            mMLWrapper->setVisibleWindowIndex( 0 );
        }
        mTempVisibleWindowIndex = -1;
    }
    emit populated();
}

//------------------------------------------------------------------------------------------------------------
//updateDetailItems
//------------------------------------------------------------------------------------------------------------
void GlxMediaModel::updateDetailItems()
{
    qDebug( "GlxMediaModel::updateDetailItems" );
    emit updateDetailsView();
}

void GlxMediaModel::itemsAdded( int startIndex, int endIndex )
{
    qDebug( "GlxMediaModel::itemsAdded %d %d", startIndex, endIndex );
    mFocusIndex = -1;
    beginInsertRows( QModelIndex(), startIndex, endIndex );
    itemIconCache.clear();
    itemFsIconCache.clear();
    endInsertRows();
}

void GlxMediaModel::itemsRemoved( int startIndex, int endIndex )
{
    qDebug( "GlxMediaModel::itemsRemoved %d %d", startIndex, endIndex );
    mFocusIndex = -1;
    beginRemoveRows( QModelIndex(), startIndex, endIndex );
    itemIconCache.clear();
    itemFsIconCache.clear();
    endRemoveRows();
}

void GlxMediaModel::updateItemIcon( int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType )
{
    qDebug( "GlxMediaModel::updateItemIcon %d", itemIndex );
    if ( tbContextType == GlxTBContextGrid ) {
        itemIconCache.insert( itemIndex, itemIcon );
    }
    else if ( tbContextType == GlxTBContextLsFs || tbContextType == GlxTBContextPtFs ) {
        itemFsIconCache.insert( itemIndex, itemIcon );
    }
}

void GlxMediaModel::albumTitleUpdated( QString aTitle )
{
    emit albumTitleAvailable( aTitle );
}

void GlxMediaModel::setFocusIndex( const QModelIndex &index )
{
    qDebug( "GlxMediaModel::setFocusIndex%d", index.row() );

    int itemIndex = index.row();
    if ( itemIndex < rowCount() && itemIndex >= 0 ) {
        mMLWrapper->setFocusIndex( itemIndex );
        mFocusIndex = itemIndex;
    }
    else if ( rowCount() > 0 ){
       
        mMLWrapper->setFocusIndex( 0 );
        mFocusIndex = itemIndex = 0;
    }

    if ( mSubState != IMAGEVIEWER_S ) {
        if ( mMLWrapper->IsDrmProtected( itemIndex ) && ( !mMLWrapper->IsDrmValid( itemIndex ) ) ) {
            QString imagePath = mMLWrapper->retrieveItemUri( itemIndex );
            mDRMUtilityWrapper->ShowRightsInfo( imagePath );
        }
    }
    else {
        CGlxImageViewerManager* CGlxImageViewerManager = CGlxImageViewerManager::InstanceL();
        const TDesC& title = CGlxImageViewerManager->ImageUri()->Des();
        QString imagePath = QString::fromUtf16( title.Ptr(), title.Length() );
        mDRMUtilityWrapper->ShowRightsInfo( imagePath );
    }
}

QModelIndex GlxMediaModel::getFocusIndex() const
{
    int itemIndex = mFocusIndex;
    if ( mFocusIndex == -1 ) {
        itemIndex = mMLWrapper->getFocusIndex();
    }

    return index( itemIndex, 0 );
}

void GlxMediaModel::setSelectedIndex( const QModelIndex &index )
{
    int itemIndex = index.row();
    qDebug( "GlxMediaModel::setSelectedIndex()%d", itemIndex );
    mMLWrapper->setSelectedIndex( itemIndex );
}

HbIcon * GlxMediaModel::getCorruptDefaultIcon( const QModelIndex &index ) const
{
    if ( mMLWrapper->isCorruptedImage( index.row() ) ) {
        return m_CorruptIcon;
    }
    return m_DefaultIcon;
}

void GlxMediaModel::thumbnailPopulatedCheck( int index )
{
    int count = rowCount() - DELTA_IMAGE;
    int maxRange = INITIAL_POPULATE_IMAGE_COUNT + DELTA_IMAGE;
    if ( index >= count || ( index >= INITIAL_POPULATE_IMAGE_COUNT && index < maxRange ) ) {
        thumbnailPopulatedFlag = TRUE;
        emit thumbnailPopulated();
    }
}

bool GlxMediaModel::setData( const QModelIndex & idx, const QVariant & value, int role )
{
    Q_UNUSED( idx )

    if ( GlxContextRole == role ) {
        if ( value.isValid() && value.canConvert<int> () ) {
            setContextMode( (GlxContextMode) value.value<int> () );
            return TRUE;
        }
    }

    if ( GlxRemoveContextRole == role ) {
        if ( value.isValid() && value.canConvert<int> () ) {
            removeContextMode( (GlxContextMode) value.value<int> () );
            return TRUE;
        }
    }

    if ( GlxFocusIndexRole == role ) {
        if ( value.isValid() && value.canConvert<int> () ) {
            setFocusIndex( index( value.value<int> (), 0 ) );
            return TRUE;
        }
    }

    if ( GlxVisualWindowIndex == role ) {
        if ( value.isValid() && value.canConvert<int> () ) {
            mMLWrapper->setVisibleWindowIndex( value.value<int> () );
            return TRUE;
        }
    }

    if ( GlxSelectedIndexRole == role ) {
        qDebug( "GlxMediaModel::setData::GlxSelectedIndexRole()::BEFORE%d", idx.row() );
        if ( value.isValid() && value.canConvert<int> () ) {
            qDebug( "GlxMediaModel::setData()::GlxSelectedIndexRole%d", idx.row() );
            setSelectedIndex( index( value.value<int> (), 0 ) );
            return TRUE;
        }
    }

    if ( role == GlxSubStateRole && value.isValid() && value.canConvert<int> () ) {
        mSubState = value.value<int> ();
        return TRUE;
    }
    if ( GlxTempVisualWindowIndex == role ) {
        if ( value.isValid() && value.canConvert<int> () ) {
            mTempVisibleWindowIndex = value.value<int> ();
            return TRUE;
        }
    }

    return FALSE;
}

QModelIndex GlxMediaModel::index( int row, int column, const QModelIndex &parent ) const
{
    Q_UNUSED(parent)
    qDebug( "GlxMediaModel::index row %d column %d", row, column );

    if ( ( row < 0 ) || ( row >= rowCount() ) || ( column < 0 ) || ( column >= columnCount() ) ) {
        return QModelIndex();
    }

    return QAbstractItemModel::createIndex( row, column );
}

