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

#include "unittest_mediamodel.h"
#include "glxmediamodel.h"
#include "glxmlwrapper.h"
#include "glxmodelparm.h"
#include "glxcollectionpluginall.hrh"
#include "hbicon.h"


// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestGlxMediaModel::initTestCase()
{    
    GlxModelParm modelParm;
    modelParm.setCollection( KGlxCollectionPluginAllImplementationUid );
    modelParm.setDepth(0);
    modelParm.setContextMode( GlxContextLsGrid ) ;
    mModel = new GlxMediaModel( modelParm );
    QTest::qWait(2000);
    
    QVERIFY( mModel );
    QVERIFY( mModel->mMLWrapper );
    QVERIFY( mModel->mDRMUtilityWrapper );
    QVERIFY( mModel->m_DefaultIcon );
    QVERIFY( mModel->m_CorruptIcon );
    QCOMPARE( mModel->thumbnailPopulatedFlag, false );
    QCOMPARE( mModel->mPaintPage, true );
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestGlxMediaModel::init()
{
    
}

void TestGlxMediaModel::rowCountTestCase()
{
    QVERIFY( mModel->rowCount() >= 0 );
}

void TestGlxMediaModel::columnCountTestCase()
{
    QCOMPARE( mModel->columnCount(), 1 );
}

void TestGlxMediaModel::parentTestCase()
{
    QModelIndex index = mModel->parent( mModel->index( 0, 0 ) );
    QCOMPARE( index.isValid(), false);
}

void TestGlxMediaModel::itemImageDataInvalidIndexTestCase()
{
    QVariant variant = mModel->itemImageData( QModelIndex(), GlxFsImageRole );
    QCOMPARE( variant.isValid(), false );
    
    variant = mModel->itemImageData( mModel->createIndex( mModel->rowCount(), 0 ), GlxFsImageRole );
    QCOMPARE( variant.isValid(), false );
}

void TestGlxMediaModel::itemImageDataGridImagetestCase()
{
    mModel->itemIconCache.insert( 0, NULL );
    QVariant variant = mModel->itemImageData( mModel->index( 0, 0), Qt::DecorationRole );
    QCOMPARE( variant.isValid(), true );
    
    mModel->itemIconCache.insert( 0, new HbIcon() );
    variant = mModel->itemImageData( mModel->index( 0, 0), Qt::DecorationRole );
    QCOMPARE( variant.isValid(), true );
}

void TestGlxMediaModel::itemImageDataHdmiBitmapTestCase()
{
    QVariant variant = mModel->itemImageData( mModel->index( 0, 0), GlxHdmiBitmap );
    QCOMPARE( variant.isValid(), true );
}

void TestGlxMediaModel::ItemPropertyViewTitleTestCase()
{
   QVariant variant = mModel->collectionProperty( GlxViewTitle );
   QCOMPARE( variant.isValid(), true );
}

void TestGlxMediaModel::ItemPropertyImageTitleTestCase()
{
   QVariant variant = mModel->collectionProperty( GlxImageTitle );
   QCOMPARE( variant.isValid(), false );
}

void TestGlxMediaModel::ItemPropertyDateTestCase()
{
   QVariant variant = mModel->collectionProperty( GlxDateRole );
   QCOMPARE( variant.isValid(), false );
}

void TestGlxMediaModel::ItemPropertyCorruptImageTestCase()
{
   QVariant variant = mModel->collectionProperty( GlxImageCorruptRole );
   QCOMPARE( variant.isValid(), false );
}

void TestGlxMediaModel::ItemPropertyTimeTestCase()
{
   QVariant variant = mModel->collectionProperty( GlxTimeRole );
   QCOMPARE( variant.isValid(), false );
}

void TestGlxMediaModel::ItemPropertySizeTestCase()
{
   QVariant variant = mModel->collectionProperty( GlxSizeRole );
   QCOMPARE( variant.isValid(), false );
}

void TestGlxMediaModel::ItemPropertyDescTestCase()
{
   QVariant variant = mModel->collectionProperty( GlxDescRole );
   QCOMPARE( variant.isValid(), false );
}

void TestGlxMediaModel::indexTestCase()
{
    QModelIndex index = mModel->index( -1, 0 );
    QCOMPARE( index.isValid(), false);
    
    index = mModel->index( 0, -1 );
    QCOMPARE( index.isValid(), false);
    
    index = mModel->index( mModel->rowCount(), 1 );
    QCOMPARE( index.isValid(), false);

    index = mModel->index( 0, 0 );
    QCOMPARE( index.isValid(), true);
}

void TestGlxMediaModel::setDataContextRoleTestCase()
{
    mModel->setData( mModel->index( 0, 0 ), GlxContextPtGrid, GlxContextRole );
    QCOMPARE( mModel->mContextMode, GlxContextPtGrid );
        
    mModel->setData( mModel->index( 0, 0), QVariant( QVariant::Invalid ), GlxContextRole );
    QCOMPARE( mModel->mContextMode, GlxContextPtGrid );
}

void TestGlxMediaModel::setDataRemoveContextRoleTestCase()
{
    mModel->setData( mModel->index( 0, 0), GlxContextPtGrid, GlxContextRole );
        
    mModel->setData( mModel->index( 0, 0), QVariant( QVariant::Invalid ), GlxRemoveContextRole );
    QCOMPARE( mModel->mContextMode, GlxContextPtGrid );
}

void TestGlxMediaModel::setDataFocusIndexRoleTestCase()
{
    mModel->setData( mModel->index( 0, 0), 1, GlxFocusIndexRole );
    QCOMPARE( mModel->mFocusIndex, 1 );
        
    mModel->setData( mModel->index( 0, 0), QVariant( QVariant::Invalid ), GlxFocusIndexRole );
    QCOMPARE( mModel->mFocusIndex, 1 );
}

void TestGlxMediaModel::setDataVisualIndexTestCase()
{
    mModel->setData( mModel->index( 0, 0), 1, GlxVisualWindowIndex );
    int visualIndex = mModel->mMLWrapper->getVisibleWindowIndex();
    QCOMPARE( visualIndex, 1 );    

    mModel->setData( mModel->index( 0, 0), QVariant( QVariant::Invalid ), GlxVisualWindowIndex );
    visualIndex = mModel->mMLWrapper->getVisibleWindowIndex();
    QCOMPARE( visualIndex, 1 );    
}

void TestGlxMediaModel::setDataSelectedIndexTestCase()
{
    bool result = mModel->setData( mModel->index( 0, 0), 1, GlxSelectedIndexRole  );
    QCOMPARE( result, true );    
    
    result = mModel->setData( mModel->index( 0, 0), QVariant( QVariant::Invalid ), GlxSelectedIndexRole );
    QCOMPARE( result, false );  
}

void TestGlxMediaModel::setDataSubStateTestCase()
{
    bool result = mModel->setData( mModel->index( 0, 0), 5, GlxSubStateRole );
    QCOMPARE( result, true );
    QCOMPARE( mModel->mSubState, 5 );

    result = mModel->setData( mModel->index( 0, 0), QVariant( QVariant::Invalid ), GlxSubStateRole  );
    QCOMPARE( result, false );
    QCOMPARE( mModel->mSubState, 5 );
}

void TestGlxMediaModel::setDataTempVisualIndexTestCase()
{
    bool result = mModel->setData( mModel->index( 0, 0), 2, GlxTempVisualWindowIndex );
    QCOMPARE( result, true );
    QCOMPARE( mModel->mTempVisibleWindowIndex,  2 );
    
    result = mModel->setData( mModel->index( 0, 0), QVariant( QVariant::Invalid ), GlxTempVisualWindowIndex  );
    QCOMPARE( result, false );
    QCOMPARE( mModel->mTempVisibleWindowIndex, 2 );    
}

void TestGlxMediaModel::GetGridIconItemTestCase()
{
    QTest::qWait( 2000 );
    
    mModel->itemIconCache.insert( 0, NULL );
    HbIcon *icon = mModel->GetGridIconItem( 0, GlxTBContextGrid );
    QVERIFY( icon || icon == NULL );
    
    mModel->itemIconCache.insert( 0, new HbIcon() );
    icon = mModel->GetGridIconItem( 0, GlxTBContextGrid );
    QVERIFY( icon );
}
    
void TestGlxMediaModel::GetFsIconItemTestCase()
{
    mModel->setContextMode( GlxContextPtFs );
    HbIcon *icon = mModel->GetFsIconItem( 0, GlxTBContextPtFs );
    QTest::qWait( 2000 );
    QVERIFY( icon || icon == NULL );
    mModel->setContextMode( GlxContextLsGrid );
}

void TestGlxMediaModel::getFullScreenIconTestCase()
{
    HbIcon icon = mModel->getFullScreenIcon( 0 );
    QCOMPARE( icon.isNull(), false );
    
    mModel->itemIconCache.insert( 0, NULL );
    icon = mModel->getFullScreenIcon( 0 );
    QCOMPARE( icon.isNull(), false );
    
    mModel->itemIconCache.insert( 0, new HbIcon() );
    icon = mModel->getFullScreenIcon( 0 );
    QCOMPARE( icon.isNull(), false );
}

void TestGlxMediaModel::getFullScreenImagetestCase()
{
    mModel->setContextMode( GlxContextLsFs );
    QTest::qWait( 2000 );
    QImage img = mModel->getFullScreenImage( 0 );
    QVERIFY( img.isNull() == true || img.isNull() == false );

    mModel->setContextMode( GlxContextPtFs );
    QTest::qWait( 2000 );
    img = mModel->getFullScreenImage( 0 );
    QVERIFY( img.isNull() == true || img.isNull() == false );  
}

void TestGlxMediaModel::setContextModeTestCase()
{
    mModel->setContextMode( GlxContextPtFs );
    QCOMPARE( mModel->mContextMode, GlxContextPtFs );
    
    mModel->setContextMode( GlxContextComment );
    QCOMPARE( mModel->mContextMode, GlxContextPtFs ); 
    mModel->removeContextMode( GlxContextComment );
    
    mModel->setContextMode( GlxContextLsGrid );
    QCOMPARE( mModel->mContextMode, GlxContextLsGrid );
    QCOMPARE( mModel->itemFsIconCache.size(), 0 );
    QCOMPARE( mModel->itemIconCache.size(), 0 );
}

void TestGlxMediaModel::removeContextModeTestCase()
{
    mModel->removeContextMode( GlxContextComment );
    QCOMPARE( mModel->mContextMode, GlxContextLsGrid );
}

void TestGlxMediaModel::setFocusIndexTestCase()
{
    mModel->setFocusIndex( mModel->index( 1, 0 ) );
    QModelIndex modelIndex = mModel->getFocusIndex();
    QCOMPARE( mModel->mFocusIndex, 1 );
    QCOMPARE( 1, modelIndex.row() );    
    
    int index = mModel->rowCount() - 1;
    mModel->setFocusIndex( mModel->index( index, 0 ) );
    modelIndex = mModel->getFocusIndex();
    QCOMPARE( mModel->mFocusIndex, modelIndex.row() );
    QCOMPARE( index, modelIndex.row() );
    
    index = mModel->rowCount();
    mModel->setFocusIndex( mModel->index( index, 0 ) );
    modelIndex = mModel->getFocusIndex();
    QCOMPARE( mModel->mFocusIndex, modelIndex.row() );
    QCOMPARE( 0, modelIndex.row() );    
}

void TestGlxMediaModel::getFocusIndexTestcase()
{
    mModel->mFocusIndex = -1;
    mModel->setFocusIndex( mModel->index( 1, 0 ) );
    QModelIndex index = mModel->getFocusIndex();
    QCOMPARE( 1, index.row() );
    
    mModel->mFocusIndex = 2;
    mModel->setFocusIndex( mModel->index( 1, 0 ) );
    index = mModel->getFocusIndex();
    QCOMPARE( 1, index.row() );
}

void TestGlxMediaModel::setSelectedIndexTestCase()
{
    mModel->setSelectedIndex( mModel->index( 1, 0 ) );
}

void TestGlxMediaModel::getCorruptDefaultIconTestCase()
{
    HbIcon *icon = mModel->getCorruptDefaultIcon( mModel->index( 0, 0 ) );
    QVERIFY( icon );
}

void TestGlxMediaModel::thumbnailPopulatedCheckTestCase()
{
    mModel->thumbnailPopulatedFlag = false;
    mModel->thumbnailPopulatedCheck( 1 );
    QCOMPARE( mModel->thumbnailPopulatedFlag, false );
    
    mModel->thumbnailPopulatedCheck( 30 );
    QCOMPARE( mModel->thumbnailPopulatedFlag, true );
}

void TestGlxMediaModel::itemUpdatedTestCase()
{
    mModel->itemUpdated( 0, GlxTBContextGrid );
    QVERIFY( mModel->itemIconCache[0] == NULL );

    mModel->thumbnailPopulatedFlag = false;
    mModel->itemUpdated( 0, GlxTBContextGrid );
    QVERIFY( mModel->itemIconCache[0] == NULL );
    
    mModel->itemUpdated( 0, GlxTBContextPtFs );
    QVERIFY( mModel->itemFsIconCache[0] == NULL );
    
    mModel->itemUpdated( 0, GlxTBContextPtFs );
    QVERIFY( mModel->itemFsIconCache[0] == NULL );
}

void TestGlxMediaModel::itemsAddedTestCase()
{
    mModel->itemsAdded( 1, 5 );
    QCOMPARE( mModel->mFocusIndex, -1 );
    QCOMPARE( mModel->itemIconCache.size(), 0 );
    QCOMPARE( mModel->itemFsIconCache.size(), 0 );
}

void TestGlxMediaModel::itemsRemovedTestCase()
{
    mModel->itemsRemoved( 1, 5 );
    QCOMPARE( mModel->mFocusIndex, -1 );
    QCOMPARE( mModel->itemIconCache.size(), 0 );
    QCOMPARE( mModel->itemFsIconCache.size(), 0 );
}

void TestGlxMediaModel::itemCorruptedTestCase()
{
    mModel->thumbnailPopulatedFlag = false;
    mModel->itemCorrupted( 1 );
    QCOMPARE( mModel->thumbnailPopulatedFlag, false );
    
    mModel->itemCorrupted( 30 );
    QCOMPARE( mModel->thumbnailPopulatedFlag, true );    
}

void TestGlxMediaModel::albumTitleUpdatedTestCase()
{
    mModel->albumTitleUpdated( QString( "Album" ) );
}

void TestGlxMediaModel::modelpopulatedTestCase()
{
    int visualIndex = mModel->mMLWrapper->getVisibleWindowIndex();
    mModel->mTempVisibleWindowIndex = -1;
    mModel->modelpopulated();
    QCOMPARE( mModel->mTempVisibleWindowIndex, -1 );
    QCOMPARE( visualIndex,  mModel->mMLWrapper->getVisibleWindowIndex() );
    
    mModel->mTempVisibleWindowIndex = mModel->rowCount() - 1;
    mModel->modelpopulated();
    visualIndex = mModel->mMLWrapper->getVisibleWindowIndex();
    QCOMPARE( mModel->mTempVisibleWindowIndex, -1 );
    QCOMPARE( visualIndex, mModel->rowCount() - 1 );
    
    mModel->mTempVisibleWindowIndex = mModel->rowCount();
    mModel->modelpopulated();
    visualIndex = mModel->mMLWrapper->getVisibleWindowIndex();
    QCOMPARE( mModel->mTempVisibleWindowIndex, -1 );
    QCOMPARE( visualIndex, 0 );
}

void TestGlxMediaModel::updateDetailItemsTestCase()
{
    mModel->updateDetailItems();
}

void TestGlxMediaModel::updateItemIconTestCase()
{
    mModel->updateItemIcon( 1, new HbIcon(), GlxTBContextGrid );
    QVERIFY( mModel->itemIconCache[1] );

    mModel->updateItemIcon( 1, new HbIcon(), GlxTBContextLsFs );
    QVERIFY( mModel->itemFsIconCache[1] );

    mModel->updateItemIcon( 1, new HbIcon(), GlxTBContextPtFs );
    QVERIFY( mModel->itemFsIconCache[1] );  
}



// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestGlxMediaModel::cleanup()
{
    
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestGlxMediaModel::cleanupTestCase()
{
    if( mModel ) {        
        delete mModel;
        mModel = 0;
    }  
}

QTEST_MAIN(TestGlxMediaModel)
#include "moc_unittest_mediamodel.cpp"
