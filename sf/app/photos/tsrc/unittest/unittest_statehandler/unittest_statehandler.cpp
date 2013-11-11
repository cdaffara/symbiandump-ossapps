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

#include "unittest_statehandler.h"
#include "glxstatemanager.h"
#include "glxbasestate.h"
#include "glxgridstate.h"
#include "glxfullscreenstate.h"
#include <glxcollectionpluginall.hrh>
#include "glxmediaid.h"


// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::initTestCase()
{    
    mStateManager = new GlxStateManager();
    //mStateManager->setupItems();
    mStateManager->mCurrentState = mStateManager->createState( GLX_GRIDVIEW_ID );
    mStateManager->mCurrentState->setState( ALL_ITEM_S );
    
    QVERIFY( mStateManager );
    QVERIFY( mStateManager->mViewManager );
    QVERIFY( mStateManager->mActionHandler == 0 );
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::init()
{
    
}

void TestGlxStateManager::removeModelGridTestCase()
{
    mStateManager->createGridModel( ALL_ITEM_S, NO_DIR );
    mStateManager->removeCurrentModel();
    QVERIFY( mStateManager->mAllMediaModel == 0 );   
}

void TestGlxStateManager::removeModelAlbumGridTestCase()
{
    mStateManager->createGridModel( ALBUM_ITEM_S, FORWARD_DIR );
    mStateManager->removeCurrentModel();
    QVERIFY( mStateManager->mAlbumGridMediaModel == 0 );   
}

void TestGlxStateManager::removeModelAlbumListTestCase()
{
    mStateManager->createModel( GLX_LISTVIEW_ID );
    mStateManager->removeCurrentModel();
    QVERIFY( mStateManager->mAlbumMediaModel == 0 );    
}

void TestGlxStateManager::cleanModelGridTestCase()
{
    mStateManager->createGridModel( ALL_ITEM_S, NO_DIR );
    mStateManager->createGridModel( ALBUM_ITEM_S, FORWARD_DIR );
    mStateManager->cleanAllModel();
    QVERIFY( mStateManager->mAllMediaModel == 0 ); 
    QVERIFY( mStateManager->mAlbumGridMediaModel == 0 ); 
    QVERIFY( mStateManager->mAlbumMediaModel == 0 );
    QVERIFY( mStateManager->mImageviewerMediaModel == 0 );
    QVERIFY( mStateManager->mCurrentModel == 0 );    
}

void TestGlxStateManager::cleanModelAlbumListTestCase()
{
    mStateManager->createModel( GLX_LISTVIEW_ID );
    mStateManager->cleanAllModel();
    QVERIFY( mStateManager->mAllMediaModel == 0 ); 
    QVERIFY( mStateManager->mAlbumGridMediaModel == 0 ); 
    QVERIFY( mStateManager->mAlbumMediaModel == 0 );
    QVERIFY( mStateManager->mImageviewerMediaModel == 0 );
    QVERIFY( mStateManager->mCurrentModel == 0 );    
}

void TestGlxStateManager::createStateGridTestCase()
{
    GlxState * state = mStateManager->createState( GLX_GRIDVIEW_ID );
    QVERIFY(state);
    QCOMPARE( state->id(), GLX_GRIDVIEW_ID );
    delete state;
    state = NULL; 
}

void TestGlxStateManager::createStateListTestCase()
{
    GlxState * state = mStateManager->createState( GLX_LISTVIEW_ID );
    QVERIFY(state);
    QCOMPARE( state->id(), GLX_LISTVIEW_ID );
    delete state;
    state = NULL; 
}

void TestGlxStateManager::createStateSettingTestCase()
{
    GlxState * state = mStateManager->createState( GLX_SLIDESHOWSETTINGSVIEW_ID );
    QVERIFY(state);
    QCOMPARE( state->id(), GLX_SLIDESHOWSETTINGSVIEW_ID );
    delete state;
    state = NULL; 
}

void TestGlxStateManager::createStateDetailTestCase()
{
    GlxState * state = mStateManager->createState( GLX_DETAILSVIEW_ID );
    QVERIFY(state);
    QCOMPARE( state->id(), GLX_DETAILSVIEW_ID );
    delete state;
    state = NULL; 
}

void TestGlxStateManager::createStateNoneTestCase()
{
    GlxState * state = mStateManager->createState( 0xFFFFFFFF );
    QVERIFY( state == 0 );
}

void TestGlxStateManager::createModelAlbumListTestCase()
{
    mStateManager->createModel( GLX_LISTVIEW_ID );
    QVERIFY( mStateManager->mAlbumMediaModel );
    mStateManager->removeCurrentModel();
}

void TestGlxStateManager::createModelImageViewerTestCase()
{
    mStateManager->mCurrentState = mStateManager->createState( GLX_FULLSCREENVIEW_ID );
    mStateManager->mCurrentState->setState( IMAGEVIEWER_S );
    mStateManager->createModel( GLX_FULLSCREENVIEW_ID );    
    QVERIFY( mStateManager->mImageviewerMediaModel );
    
    mStateManager->cleanAllModel();
    GlxState *state = mStateManager->mCurrentState;
    mStateManager->mCurrentState = state->previousState();
    delete state;
    state = NULL;
}

void TestGlxStateManager::createModelAlbumSlideShowTestCase()
{
    mStateManager->mCurrentState = mStateManager->createState( GLX_SLIDESHOWVIEW_ID );
    mStateManager->mCurrentState->setState( SLIDESHOW_ALBUM_ITEM_S );
    mStateManager->createModel( GLX_SLIDESHOWVIEW_ID );    
    QVERIFY( mStateManager->mAlbumGridMediaModel );
    
    mStateManager->cleanAllModel();
    GlxState *state = mStateManager->mCurrentState;
    mStateManager->mCurrentState = state->previousState();
    delete state;
    state = NULL;
}

void TestGlxStateManager::createGridModelAllTestCase()
{
    mStateManager->createGridModel(ALL_ITEM_S, NO_DIR);
    QCOMPARE(mStateManager->mCollectionId, KGlxCollectionPluginAllImplementationUid);
    QVERIFY(mStateManager->mAllMediaModel);
    mStateManager->removeCurrentModel();
}

void TestGlxStateManager::createGridModelAlbumTestCase()
{
    mStateManager->createGridModel(ALBUM_ITEM_S, FORWARD_DIR);
    QCOMPARE(mStateManager->mCollectionId, ( int )KGlxAlbumsMediaId );
    QVERIFY(mStateManager->mAlbumGridMediaModel); 
    mStateManager->removeCurrentModel();
}

void TestGlxStateManager::gridInternalStateTestCase()
{
    GlxState * state = mStateManager->createState( GLX_GRIDVIEW_ID );
    QVERIFY( state );
    
    state->setState( ALL_ITEM_S );    
    QCOMPARE( state->state(), (int )ALL_ITEM_S );
    
    state->setState( FETCHER_ALBUM_ITEM_S );    
    QCOMPARE( state->state(), (int )FETCHER_ALBUM_ITEM_S );  
    delete state;
}

void TestGlxStateManager::gridTransitionParameterTestCase()
{
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;
    
    GlxState * state = mStateManager->createState( GLX_GRIDVIEW_ID );
    QVERIFY( state );
    
    state->setTranstionParameter( FORWARD_DIR, effect, viewEffect );
    QCOMPARE( effect, ALBUMLIST_TO_GRID ); 
    QCOMPARE( viewEffect, BOTH_VIEW ); 
    
    state->setTranstionParameter( BACKWARD_DIR, effect, viewEffect );
    QCOMPARE( effect, GRID_TO_ALBUMLIST ); 
    QCOMPARE( viewEffect, BOTH_VIEW ); 
    
    delete state;
}

void TestGlxStateManager::listInternalStateTestCase()
{
    GlxState * state = mStateManager->createState( GLX_LISTVIEW_ID );
    QVERIFY( state );
    
    state->setState( NO_LIST_S );    
    QCOMPARE( state->state(), (int )NO_LIST_S );
    
    state->setState( FETCHER_ALBUM_S );    
    QCOMPARE( state->state(), (int )FETCHER_ALBUM_S );
    delete state;
}

void TestGlxStateManager::listTransitionParameterTestCase()
{
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;
    
    GlxState * state = mStateManager->createState( GLX_LISTVIEW_ID );
    QVERIFY( state );
    
    state->setTranstionParameter( NO_DIR, effect, viewEffect );
    QCOMPARE( effect, GRID_TO_ALBUMLIST ); 
    QCOMPARE( viewEffect, BOTH_VIEW ); 
    
    state->setTranstionParameter( BACKWARD_DIR, effect, viewEffect );
    QCOMPARE( effect, NO_EFFECT ); 
    QCOMPARE( viewEffect, NO_VIEW ); 
    
    delete state;
}

void TestGlxStateManager::fullScreenInternalStateTestCase()
{
    GlxState * state = mStateManager->createState( GLX_FULLSCREENVIEW_ID );
    QVERIFY( state );
    
    state->setState( ALBUM_FULLSCREEN_S );    
    QCOMPARE( state->state(), (int )ALBUM_FULLSCREEN_S );
    
    state->setState( IMAGEVIEWER_S );    
    QCOMPARE( state->state(), (int )IMAGEVIEWER_S );
    delete state;    
}

void TestGlxStateManager::fullScreenTransitionParameterTestCase()
{
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;
    
    GlxState * state = mStateManager->createState( GLX_FULLSCREENVIEW_ID );
    QVERIFY( state );
    
    state->setTranstionParameter( FORWARD_DIR, effect, viewEffect );
    QCOMPARE( effect, GRID_TO_FULLSCREEN ); 
    QCOMPARE( viewEffect, BOTH_VIEW ); 
    
    state->setTranstionParameter( BACKWARD_DIR, effect, viewEffect );
    QVERIFY( effect == FULLSCREEN_TO_GRID_PORTRAIT || effect == FULLSCREEN_TO_GRID );
    QCOMPARE( viewEffect, LAUNCH_VIEW ); 
    
    delete state;
}

void TestGlxStateManager::slideShowInternalStateTestCase()
{
    GlxState * state = mStateManager->createState( GLX_SLIDESHOWVIEW_ID );
    QVERIFY( state );
    
    state->setState( SLIDESHOW_GRID_ITEM_S );    
    QCOMPARE( state->state(), (int )SLIDESHOW_GRID_ITEM_S );
    
    state->setState( SLIDESHOW_GRID_ITEM_S );    
    QCOMPARE( state->state(), (int )SLIDESHOW_GRID_ITEM_S );
    delete state;       
}

void TestGlxStateManager::detailInternalStateTestCase()
{
    GlxState * state = mStateManager->createState( GLX_DETAILSVIEW_ID );
    QVERIFY( state );
    
    state->setState( NO_DETAIL_S );    
    QCOMPARE( state->state(), (int )NO_DETAIL_S );
    
    state->setState( IMAGEVIEWER_DETAIL_S );    
    QCOMPARE( state->state(), (int )IMAGEVIEWER_DETAIL_S );
    delete state;    
}

void TestGlxStateManager::detailTransitionParameterNegativeTestCase()
{
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;
    
    GlxState * state = mStateManager->createState( GLX_DETAILSVIEW_ID );
    QVERIFY( state );
    
    state->setTranstionParameter( FORWARD_DIR, effect, viewEffect );
    QCOMPARE( effect, NO_EFFECT ); 
    QCOMPARE( viewEffect, NO_VIEW ); 
    delete state;
}

void TestGlxStateManager::detailTransitionParameterTestCase()
{
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;
    
    mStateManager->mCurrentState = mStateManager->createState( GLX_FULLSCREENVIEW_ID );
    GlxState * state = mStateManager->createState( GLX_DETAILSVIEW_ID );
    QVERIFY( state );
    
    state->setTranstionParameter( FORWARD_DIR, effect, viewEffect );
    QCOMPARE( effect, FULLSCREEN_TO_DETAIL ); 
    QCOMPARE( viewEffect, BOTH_VIEW ); 
    
    state->setTranstionParameter( BACKWARD_DIR, effect, viewEffect );
    QCOMPARE( effect, DETAIL_TO_FULLSCREEN ); 
    QCOMPARE( viewEffect, BOTH_VIEW ); 
    
    delete state;
    state = mStateManager->mCurrentState;
    mStateManager->mCurrentState = state->previousState();
    delete state;
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::cleanup()
{
    
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::cleanupTestCase()
{
    if(mStateManager)
    {
        QCoreApplication::processEvents(); //To:Do remove it once mainwindow delete hang problem will resolve
        delete mStateManager;
        mStateManager = 0;
    }  
}

QTEST_MAIN(TestGlxStateManager)
#include "moc_unittest_statehandler.cpp"
