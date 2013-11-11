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

#include "unittest_medialistwrapper.h"
#include "glxmlwrapper_p.h"
#include "glxmlwrapper.h"
#include "glxmedialist.h"
#include <glxcollectionpluginall.hrh>
#include <glxmodelparm.h>
#include <hbicon.h>
#include <qdatetime.h>
#include <QString>

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::initTestCase()
{
    mTestObject = 0;
    mTestObject = new GlxMLWrapper( KGlxCollectionPluginAllImplementationUid, 0, EGlxFilterImage );
    mListTestObject = new GlxMLWrapper( KGlxAlbumsMediaId, 0, EGlxFilterImage );
    QVERIFY( mTestObject );
    QVERIFY( mListTestObject );
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::init()
{

}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::cleanup()
{

}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::cleanupTestCase()
{
    if ( mTestObject ) {
        delete mTestObject;
        mTestObject = 0;
    }
    if ( mListTestObject ) {
        delete mListTestObject;
        mListTestObject = 0;
    }
}

void TestGlxMLWrapper::testRetrieveItemDateIsNull()
{
    mTestObject->setContextMode( GlxContextPtGrid );
    QTest::qWait( 3000 );    
    int count = mTestObject->getItemCount();
    QDate date = mTestObject->retrieveItemDate( count - 1 );
    qDebug ( "TestGlxMLWrapper::testRetrieveItemDateIsNull =%d", date.isNull() );
    QVERIFY( date.isNull() == 1 );
}

void TestGlxMLWrapper::testRetrieveItemIconIsNull()
{
    int itemIndex = mTestObject->getItemCount() - 1;
    // grid icon should be NULL
    HbIcon* icon = mTestObject->retrieveItemIcon( itemIndex, GlxTBContextGrid );
    QVERIFY(icon == NULL);

    // fullscreen icon should be NULL
    icon = mTestObject->retrieveItemIcon( itemIndex, GlxTBContextLsFs );
    QVERIFY(icon == NULL);
}

void TestGlxMLWrapper::testSetContextModeGrid()
{
    // After adding grid context should get grid icon 
    mTestObject->setContextMode( GlxContextPtGrid );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == TRUE );
    
    mTestObject->setContextMode( GlxContextLsFs );
    mTestObject->setContextMode( GlxContextLsGrid );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == TRUE );    
}

void TestGlxMLWrapper::testSetContextModeFullScreen()
{
    mTestObject->setContextMode( GlxContextPtFs );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == TRUE ); 
    QVERIFY( mTestObject->mMLWrapperPrivate->iPtFsThumbnailContext );
    QVERIFY( mTestObject->mMLWrapperPrivate->iFocusFsThumbnailContext );
    QVERIFY( mTestObject->mMLWrapperPrivate->iFocusGridThumbnailContext );
    QVERIFY( mTestObject->mMLWrapperPrivate->iPtFsContextActivated == TRUE ); 
    mTestObject->removeContextMode( GlxContextPtFs );
}

void TestGlxMLWrapper::testSetContextModeList()
{
    mListTestObject->setContextMode( GlxContextSelectionList );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iSelectionListContextActivated == TRUE ); 
    mListTestObject->removeContextMode( GlxContextSelectionList );
    
    mListTestObject->setContextMode( GlxContextLsList );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iPtListContextActivated == TRUE ); 
    mListTestObject->removeContextMode( GlxContextLsList );
}

void TestGlxMLWrapper::testSetContextModeFavorite()
{
    mTestObject->setContextMode( GlxContextFavorite );
    QVERIFY( mTestObject->mMLWrapperPrivate->iFavouriteContext );
    mTestObject->removeContextMode( GlxContextFavorite );
}

void TestGlxMLWrapper::testSetContextModeComment()
{
    mTestObject->setContextMode( GlxContextComment );
    QVERIFY( mTestObject->mMLWrapperPrivate->iDescContext );
    QVERIFY( mTestObject->mMLWrapperPrivate->iDetailsContextActivated == TRUE ); 
    mTestObject->removeContextMode( GlxContextComment );
}

void TestGlxMLWrapper::testRemoveContextModeGrid()
{
    // After adding grid context should get grid icon 
    mTestObject->setContextMode( GlxContextPtGrid );
    
    mTestObject->removeContextMode( GlxContextPtGrid );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == NULL );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == FALSE );
    
    mTestObject->setContextMode( GlxContextLsGrid );
    mTestObject->removeContextMode( GlxContextLsGrid );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == NULL );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == FALSE );
}

void TestGlxMLWrapper::testRemoveContextModeFullScreen()
{
    mTestObject->setContextMode( GlxContextPtFs );
    mTestObject->removeContextMode( GlxContextPtFs );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated );
    QVERIFY( mTestObject->mMLWrapperPrivate->iGridContextActivated == TRUE ); 
    QVERIFY( mTestObject->mMLWrapperPrivate->iPtFsThumbnailContext == NULL );
    QVERIFY( mTestObject->mMLWrapperPrivate->iFocusFsThumbnailContext == NULL );
    QVERIFY( mTestObject->mMLWrapperPrivate->iFocusGridThumbnailContext == NULL );
    QVERIFY( mTestObject->mMLWrapperPrivate->iPtFsContextActivated == FALSE ); 
}

void TestGlxMLWrapper::testRemoveContextModeList()
{
    mListTestObject->setContextMode( GlxContextSelectionList );
    mListTestObject->removeContextMode( GlxContextSelectionList );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext == NULL );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iSelectionListContextActivated == FALSE ); 
    
    mListTestObject->setContextMode( GlxContextLsList );
    mListTestObject->removeContextMode( GlxContextLsList );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext == NULL );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext == NULL );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iTitleAttributeContext == NULL );
    QVERIFY( mListTestObject->mMLWrapperPrivate->iSelectionListContextActivated == FALSE ); 
    mListTestObject->setContextMode( GlxContextLsList );
}

void TestGlxMLWrapper::testRemoveContextModeFavorite()
{
    mTestObject->setContextMode( GlxContextFavorite );
    mTestObject->removeContextMode( GlxContextFavorite );
    QVERIFY( mTestObject->mMLWrapperPrivate->iFavouriteContext == NULL );
}

void TestGlxMLWrapper::testRemoveContextComment()
{
    mTestObject->setContextMode( GlxContextComment );
    mTestObject->removeContextMode( GlxContextComment );
    QVERIFY( mTestObject->mMLWrapperPrivate->iDescContext == NULL );
    QVERIFY( mTestObject->mMLWrapperPrivate->iDetailsContextActivated == FALSE );    
}

// -----------------------------------------------------------------------------
// testGetItemCount
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::testGetItemCount()
{
    mTestObject->setContextMode( GlxContextPtGrid );
    QTest::qWait( 5000 );
    int count = mTestObject->getItemCount();
    qDebug( "TestGlxMLWrapper::testGetItemCount() count %d", count );
    QVERIFY( count >= 0 );
}

void TestGlxMLWrapper::testGetFocusIndex()
{
    int focusIndex = mTestObject->getFocusIndex();
    //QVERIFY(focusIndex == -1);
    mTestObject->setFocusIndex( mTestObject->getItemCount() - 1 );
    focusIndex = mTestObject->getFocusIndex();
    QVERIFY( focusIndex == mTestObject->getItemCount()-1 );
}

void TestGlxMLWrapper::testSetFocusIndex()
{
    mTestObject->setFocusIndex( mTestObject->getItemCount() - 1 );
    int focusIndex = mTestObject->getFocusIndex();
    QVERIFY( focusIndex == mTestObject->getItemCount()-1 );
}

void TestGlxMLWrapper::testSetSelectedIndex()
{
    mTestObject->setSelectedIndex( 0 );
    QCOMPARE( mTestObject->mMLWrapperPrivate->iMediaList->IsSelected( 0 ), 1 );
}

void TestGlxMLWrapper::testRetrieveListTitle()
{
    QTest::qWait( 5000 );
    QString title = mListTestObject->retrieveListTitle( 0 );
    QVERIFY( title.isEmpty() == TRUE );
}

void TestGlxMLWrapper::testRetrieveListSubTitle()
{
    QString title = mListTestObject->retrieveListSubTitle( 0 );
    QVERIFY( title.isEmpty() == TRUE );    
}

void TestGlxMLWrapper::testRetrieveListDesc()
{
    QString desc = mTestObject->retrieveListDesc( 0 );
    QVERIFY( desc.isEmpty() == TRUE );       
}

void TestGlxMLWrapper::testGetVisibleWindowIndex()
{
    int visibleIndex = mTestObject->getVisibleWindowIndex();
    QVERIFY(visibleIndex == 0);

    int itemIndex = mTestObject->getItemCount() - 1;
    mTestObject->setVisibleWindowIndex( itemIndex );
    visibleIndex = mTestObject->getVisibleWindowIndex();
    QVERIFY(visibleIndex == itemIndex);
}

void TestGlxMLWrapper::testSetVisibleWindowIndex()
{
    int itemIndex = mTestObject->getItemCount() - 1;
    mTestObject->setVisibleWindowIndex( itemIndex );
    int visibleIndex = mTestObject->getVisibleWindowIndex();
    QVERIFY(visibleIndex == itemIndex);
}

void TestGlxMLWrapper::testRetrieveItemUri()
{
    qDebug( "TestGlxMLWrapper::testRetrieveItemUri enter" );

    QString uri = mTestObject->retrieveItemUri( 0 );
    qDebug( "TestGlxMLWrapper::testRetrieveItemUri =%d", uri.isEmpty() );
    QVERIFY( uri.isEmpty() == 0 || uri.isEmpty() == 1 );
}

void TestGlxMLWrapper::testRetrieveItemUriName()
{
    qDebug( "TestGlxMLWrapper::testRetrieveItemUriName enter" );
 
    QString uri = mTestObject->retrieveItemUri( 0 );
    QString imageName = uri.section( '\\', -1 );
    qDebug( "TestGlxMLWrapper::testRetrieveItemUriName =%d", uri.isEmpty() );
    QVERIFY( imageName.isEmpty() == 0 || imageName.isEmpty() == 1 );
}

void TestGlxMLWrapper::testRetrieveItemSize()
{
    int itemSize = mTestObject->retrieveItemSize( 0 );
    QVERIFY( itemSize >= 0 );
}

void TestGlxMLWrapper::testRetrieveItemIcon()
{
    int itemIndex = mTestObject->getItemCount() - 1;

    // Should get fullscreen icon 
    mTestObject->setContextMode( GlxContextLsFs );
    QTest::qWait( 4000 );
    HbIcon* icon = mTestObject->retrieveItemIcon( itemIndex, GlxTBContextLsFs );
    QVERIFY( icon != NULL || icon == NULL );

    // Should get grid icon
    mTestObject->setContextMode( GlxContextPtGrid );
    QTest::qWait( 2000 );
    icon = mTestObject->retrieveItemIcon( itemIndex, GlxTBContextGrid );
    QVERIFY(icon != NULL || icon == NULL );
}

void TestGlxMLWrapper::testRetrieveItemDateIsNotNull()
{
    qDebug( "TestGlxMLWrapper::testRetrieveItemDateIsNotNull enter" );
    int count = mTestObject->getItemCount();

    QDate date = mTestObject->retrieveItemDate( count - 1 );
    qDebug( "TestGlxMLWrapper::testRetrieveItemDateIsNotNull =%d", date.isNull() );
    QVERIFY( date.isNull() == 0 || date.isNull() == 1 );
}

void TestGlxMLWrapper::testRetrieveItemDateIsValid()
{
    qDebug( "TestGlxMLWrapper::testRetrieveItemDateIsValid enter" );
    int index = mTestObject->getItemCount() -1;
    QDate date = mTestObject->retrieveItemDate( index );
    qDebug( "TestGlxMLWrapper::testRetrieveItemDateIsValid IsNull=%d", date.isNull() );
    qDebug( "TestGlxMLWrapper::testRetrieveItemDateIsValid IsValid=%d", date.isValid() );

    QVERIFY( date.isNull() == 0 || date.isNull() == 1 );
    QVERIFY( date.isValid() == 1 || date.isValid() == 0 );
}

void TestGlxMLWrapper::testRetrieveItemDateValidate()
{
    qDebug( "TestGlxMLWrapper::testRetrieveItemDateValidate enter" );
    int index = mTestObject->getItemCount() -1;
    QDate date = mTestObject->retrieveItemDate( index );
    qDebug( "TestGlxMLWrapper::testRetrieveItemDateValidate =%d", date.isValid( date.year(), date.month(), date.day() ) );
    QVERIFY( date.isValid( date.year(), date.month(), date.day() ) == 1 || date.isValid( date.year(), date.month(), date.day() ) == 0 );
}

void TestGlxMLWrapper::testRetrieveItemTime()
{
    int index = mTestObject->getItemCount() -1;
    QTime time = mTestObject->retrieveItemTime( index );
    QVERIFY( time.isNull() == FALSE  || time.isNull() == TRUE );
    QVERIFY( time.isValid() == TRUE || time.isValid() == FALSE );
}

void TestGlxMLWrapper::testRetrieveViewTitle()
{
    QString title = mTestObject->retrieveViewTitle();
    QVERIFY( title.isEmpty() == TRUE ); 
}

void TestGlxMLWrapper::testIsDrmProtected()
{
    int index = mTestObject->getItemCount() -1;
    bool isDrm = mTestObject->IsDrmProtected( index );
    QVERIFY( isDrm == FALSE );       
}

void TestGlxMLWrapper::testIsDrmValid()
{
    int index = mTestObject->getItemCount() -1;
    bool isDrm = mTestObject->IsDrmValid( index );
    QVERIFY( isDrm == FALSE );       
}

void TestGlxMLWrapper::testSetDrmVaild()
{
    int index = mTestObject->getItemCount() -1;
    mTestObject->setDrmValid( index, FALSE );
    bool isDrm = mTestObject->IsDrmValid( index );
    QVERIFY( isDrm == FALSE );       

    mTestObject->setDrmValid( index, TRUE );
    isDrm = mTestObject->IsDrmValid( index );
    QVERIFY( isDrm == TRUE );           
}

void TestGlxMLWrapper::testRetrieveBitmap()
{
    int index = mTestObject->getItemCount() -1;
    QVariant var = mTestObject->RetrieveBitmap( index );
    QVERIFY( var.isNull() == FALSE );
    QVERIFY( var.isValid() == TRUE );
}

void TestGlxMLWrapper::testRetrieveListItemCount()
{
    int count = mListTestObject->retrieveListItemCount( 0 );
    QVERIFY( count >= 0 );
}

void TestGlxMLWrapper::testIsSystemItem()
{
    bool isSys = mListTestObject->isSystemItem( 0 );
    QVERIFY( isSys == TRUE );
    
    isSys = mListTestObject->isSystemItem( 2 );
    QVERIFY( isSys == FALSE );
}

void TestGlxMLWrapper::testIsCorruptedImage()
{
    int index = mTestObject->getItemCount() -1;
    bool isCorrupt = mTestObject->isCorruptedImage( index );
    QVERIFY( isCorrupt == FALSE );
}

void TestGlxMLWrapper::testItemsAdded()
{
    QSignalSpy spysignal( mTestObject, SIGNAL(insertItems(int ,int )) );
    QVERIFY( spysignal.count() == 0 );

    int index = mTestObject->getItemCount();
    spysignal.clear();

    mTestObject->itemsAdded( index, index + 5 );
    qDebug( "Signal Count %d", spysignal.count() );
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == index);
    QVERIFY(spysignal.value(0).at(1).toInt() == index+5);

    mTestObject->itemsAdded( index + 6, index + 6 );
    qDebug( "Signal #Count %d", spysignal.count() );
    QVERIFY(spysignal.count() == 2);
    QVERIFY(spysignal.value(1).at(0).toInt() == index+6);
    QVERIFY(spysignal.value(1).at(1).toInt() == index+6);
}

void TestGlxMLWrapper::testItemsRemoved()
{
    QSignalSpy spysignal( mTestObject, SIGNAL(removeItems(int ,int )) );
    QVERIFY(spysignal.count() == 0);

    int index = mTestObject->getItemCount();
    spysignal.clear();

    mTestObject->itemsRemoved( index, index + 5 );
    qDebug( "Signal Count %d", spysignal.count() );
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == index);
    QVERIFY(spysignal.value(0).at(1).toInt() == index+5);

    mTestObject->itemsRemoved( index + 6, index + 6 );
    qDebug( "Signal #Count %d", spysignal.count() );
    QVERIFY(spysignal.count() == 2);
    QVERIFY(spysignal.value(1).at(0).toInt() == index+6);
    QVERIFY(spysignal.value(1).at(1).toInt() == index+6);
}

void TestGlxMLWrapper::testHandleReceivedIcon()
{
    qRegisterMetaType<GlxTBContextType> ( "GlxTBContextType" );
    QSignalSpy spysignal( mTestObject, SIGNAL(updateItem(int , GlxTBContextType )) );
    QVERIFY(spysignal.count() == 0);

    int count = mTestObject->getItemCount();
    spysignal.clear();

    mTestObject->handleReceivedIcon( count - 1, GlxTBContextGrid );
    qDebug( "Signal Count %d", spysignal.count() );
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == count-1);
    QVERIFY(spysignal.value(0).at(1).toInt() == GlxTBContextGrid);
    spysignal.clear();
}

void TestGlxMLWrapper::testHandleIconCorrupt()
{
    QSignalSpy spysignal( mTestObject, SIGNAL(itemCorrupted(int )) );
    QVERIFY(spysignal.count() == 0);

    int index = mTestObject->getItemCount();
    spysignal.clear();

    mTestObject->handleIconCorrupt( index - 1 );
    qDebug( "Signal Count %d", spysignal.count() );
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == index-1);

}

void TestGlxMLWrapper::testHandleListItemAvailable()
{
    qRegisterMetaType<GlxTBContextType> ( "GlxTBContextType" );
    QSignalSpy spysignal( mTestObject, SIGNAL(updateItem(int , GlxTBContextType )) );
    QVERIFY(spysignal.count() == 0);

    int count = mTestObject->getItemCount();
    spysignal.clear();

    mTestObject->handleListItemAvailable( count - 1 );
    qDebug( "Signal Count %d", spysignal.count() );
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == count-1);
}


QTEST_MAIN(TestGlxMLWrapper)
#include "moc_unittest_medialistwrapper.cpp"
