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

#include "hswallpaperselectionstate.h"
#include "hsspinnerdialog.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswallpaper.h"
#include "hsconfiguration.h"

#ifdef Q_OS_SYMBIAN
#include "hsimagehandlingclient.h"
#else
#include "xqaiwgetimageclient.h"
#endif

// Helper macros for connecting state entry and exit actions.
#define ENTRY_ACTION(state, action) \
    connect(state, SIGNAL(entered()), SLOT(action()));
#define EXIT_ACTION(state, action) \
    connect(state, SIGNAL(exited()), SLOT(action()));

/*!
    \class HsWallpaperSelectionState
    \ingroup group_hshomescreenstateplugin
    \brief 
*/

/*!

*/
HsWallpaperSelectionState::HsWallpaperSelectionState(QState *parent)
  : QState(parent),    
    mImageHandler(0),
    mWallpaper(0),    
    mWaitDialog(0)
{
    setupStates();
}

/*!

*/
HsWallpaperSelectionState::~HsWallpaperSelectionState()
{

}

/*!

*/
void HsWallpaperSelectionState::setupStates()
{
    // States
    
    QState *state_processing = new QState(this);
    QState *state_selectingImage = new QState(state_processing);
    QState *state_editingImage = new QState(state_processing);
    QState *state_assigningImage = new QState(state_processing);
    QState *state_errorMessage = new QState(this);
    
    setInitialState(state_processing);
    state_processing->setInitialState(state_selectingImage);
    
    // Transitions
#if defined (Q_OS_SYMBIAN) && !defined (__WINSCW__)
    state_selectingImage->addTransition(
        this, SIGNAL(event_assignImage()), state_editingImage);
#else
    state_selectingImage->addTransition(
        this, SIGNAL(event_assignImage()), state_assigningImage);
#endif // Q_OS_SYMBIAN
    
    state_processing->addTransition(
        this, SIGNAL(event_error()), state_errorMessage);

    // Actions

    ENTRY_ACTION(state_selectingImage, action_selectingImage_start)
    EXIT_ACTION(state_selectingImage, action_imageHandler_cleanup)

    ENTRY_ACTION(state_editingImage, action_editingImage_start)
    EXIT_ACTION(state_editingImage, action_imageHandler_cleanup)

    ENTRY_ACTION(state_assigningImage, action_assigningImage_showWaitDialog)
    ENTRY_ACTION(state_assigningImage, action_assigningImage_start)
    EXIT_ACTION(state_assigningImage, action_assigningImage_cleanup)
    EXIT_ACTION(state_assigningImage, action_assigningImage_hideWaitDialog)

    ENTRY_ACTION(state_errorMessage, action_errorMessage_show)
}

/*!

*/
void HsWallpaperSelectionState::action_selectingImage_start()
{
#ifdef Q_OS_SYMBIAN
    mImageHandler = new HsImageHandlingClient;
#else    
    mImageHandler = new XQAIWGetImageClient;
#endif

    connect(mImageHandler, SIGNAL(fetchCompleted(const QString&)),
            this, SLOT(onFetchCompleted(const QString&)));
    connect(mImageHandler, SIGNAL(fetchFailed(int, const QString&)),
            this, SLOT(onFetchFailed(int, const QString&)));

    mImageHandler->fetch();
}
 
/*!

*/
void HsWallpaperSelectionState::action_imageHandler_cleanup()
{
    mImageHandler->disconnect(this);
    mImageHandler->deleteLater();
    mImageHandler = 0;
}

/*!

*/
void HsWallpaperSelectionState::action_assigningImage_showWaitDialog()
{
    mWaitDialog = new HsSpinnerDialog;
    mWaitDialog->start();
}
  
/*!

*/
void HsWallpaperSelectionState::action_assigningImage_start()
{
    HsScene *scene = HsScene::instance();

    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::SceneWallpaper) {
        mWallpaper = scene->wallpaper();
    } else {
        mWallpaper = scene->activePage()->wallpaper();
    }

    connect(mWallpaper, SIGNAL(imageSet()), SLOT(onImageSet()));
    connect(mWallpaper, SIGNAL(imageSetFailed()), SLOT(onImageSetFailed()));
    
    mWallpaper->setImage(mImagePath);
}
 
/*!

*/
void HsWallpaperSelectionState::action_assigningImage_cleanup()
{
    mWallpaper->disconnect(this);
    mWallpaper = 0;
}

/*!

*/
void HsWallpaperSelectionState::action_assigningImage_hideWaitDialog()
{
    mWaitDialog->stop();
    mWaitDialog = 0;
}

/*!

*/
void HsWallpaperSelectionState::action_errorMessage_show()
{
    // TODO: Waiting for UX decision.
    emit event_waitInput();
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!

*/
void HsWallpaperSelectionState::action_editingImage_start()
{
#ifdef Q_OS_SYMBIAN    
    mImageHandler = new HsImageHandlingClient; 

    connect(mImageHandler, SIGNAL(editorCompleted()),
            this, SLOT(onEditorCompleted()));
    connect(mImageHandler, SIGNAL(editorFailed(int, const QString&)),
            this, SLOT(onFetchFailed(int, const QString&)));

    mImageHandler->edit(mImagePath);
#endif //Q_OS_SYMBIAN
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!

*/
void HsWallpaperSelectionState::onFetchCompleted(const QString &imagePath)
{
    mImagePath = imagePath;
    emit event_assignImage();
}
 
/*!

*/
void HsWallpaperSelectionState::onFetchFailed(int errorCode, const QString &errorMessage)
{
    Q_UNUSED(errorCode)
    Q_UNUSED(errorMessage)
    emit event_error();
}

/*!

*/
void HsWallpaperSelectionState::onImageSet()
{
    emit event_waitInput();
}
 
/*!

*/
void HsWallpaperSelectionState::onImageSetFailed()
{
    emit event_error();
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!

*/
void HsWallpaperSelectionState::onEditorCompleted()
{    
    emit event_waitInput();
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
