/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <f32file.h>
#include <hbmainwindow.h>
#include <hbview.h>

class EngineWrapper;
class HbDataFormModelItem;
class HbDataFormModel;
class HbDataForm;
class HbDataFormViewItem;
    
const QStringList CAPTUREMODES = (QStringList() << "Single capture" << "Sequential capture" << "Video capture");
const QStringList KEYS = (QStringList() << "Send key" << "Power key" << "Side key" << "Camera key");
const QStringList VIDEOFORMATS = (QStringList() << "Animated GIF");
const QStringList IMAGEFORMATS = (QStringList() << "PNG" << "JPEG high qual." << "JPEG low qual." << "BMP" << "GIF");
const QStringList MEMORYMODES = (QStringList() << "Phone memory" << "Memory card");


/**
 * Settings class that is used for 
 */
class GrabSettings
    {
public:
    int             mCaptureMode;
    
    int             mSingleCaptureHotkey;
    int             mSingleCaptureImageFormat;
    int             mSingleCaptureMemoryInUseMultiDrive;
    QString         mSingleCaptureFileName;

    int             mSequantialCaptureHotkey;
    int             mSequantialCaptureImageFormat;
    int             mSequantialCaptureDelay;
    int             mSequantialCaptureMemoryInUseMultiDrive;
    QString         mSequantialCaptureFileName;

    int             mVideoCaptureHotkey;
    int             mVideoCaptureVideoFormat;
    int             mVideoCaptureMemoryInUseMultiDrive;
    QString         mVideoCaptureFileName;
    };

/**
 * Class that creates settings view of Screen Grabber
 */
class SettingsView : public HbView
{
    Q_OBJECT

    enum CaptureMode{ SINGLE, SEQUENTIAL, VIDEO };

public:
     /**
     * Constuctor
     * @param mainView pointer to main view
     * @param mainWindow pointer to main window
     */
    SettingsView(HbView &mainView, HbMainWindow &mainWindow, EngineWrapper &engineWrapper);

    /**
    * destructor
    */
    ~SettingsView();


    /**
     * Loads settings from engine and formats those settings into form items
     */
    void loadSettings();
    
    
private slots:
  
    /**
     * saves settings and closes view.
     */
    void saveAndClose();
    
    /**
     * closes setting view
     */
    void close();

    /**
     * Slot that is called each time capture mode is changed
     */
//    void updateShownItems();
private:
    
    /**
     * Creates single capture related components
     */
    void createSingleCaptureComponents(HbDataFormModel *model, HbDataFormModelItem *parent);
    
    /*
     * Creates sequential capture related components.
     */
    void createSequentialCaptureComponents(HbDataFormModel *model, HbDataFormModelItem *parent);
    
    /**
     * creates video capture related components
     */
    void createVideoCaptureComponents(HbDataFormModel *model, HbDataFormModelItem *parent);
    
    
    
    /**
     * Reads form items and saves them into member variable mSettings
     */
    void readFormItems();

private:
    
    /* Settings */
    GrabSettings mSettings;
    
    /* EngineWrapper */
    EngineWrapper &mEngineWrapper;
    
    /* Main Window of Screen Grabber */
    HbMainWindow &mMainWindow;
    
    /* Main View of Screen Grabber */
    HbView &mMainView;
    
    /* settingform */
    HbDataForm *mSettingForm;
    
    /* model for settingform */
    HbDataFormModel *mModel;
    
    /* model's components: */   
    
    /* mode selection item */
    HbDataFormModelItem *mModeItem;
    
    /* groups that contain mode specific settings */
    HbDataFormModelItem *mGroupImageCapture;
    HbDataFormModelItem *mGroupSeguantialCapture;
    HbDataFormModelItem *mGroupVideoCapture;
    
    /* Still image components */
    HbDataFormModelItem *mImageHotKeyItem;
    HbDataFormModelItem *mImageFormatItem;
    HbDataFormModelItem *mImageMemoryInUseItem;
    HbDataFormModelItem *mImageFileNameItem;
       
    /* Sequential image components */
    HbDataFormModelItem *mSequantialHotKeyItem;
    HbDataFormModelItem *mSequantialFormatItem;
    HbDataFormModelItem *mSequantialDelayItem;
    HbDataFormModelItem *mSequantialMemoryInUseItem;
    HbDataFormModelItem *mSequantialFileNameItem;
    
    /* Video capture components */
    HbDataFormModelItem *mVideoHotKeyItem;
    HbDataFormModelItem *mVideoFormatItem;
    HbDataFormModelItem *mVideoMemoryInUseItem;
    HbDataFormModelItem *mVideoFileNameItem;
    

    
    
};

#endif
