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


#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <hbmainwindow.h>
#include <hbview.h>
#include "loadgen.hrh"

class EngineWrapper;
class HbDataFormModelItem;
class HbDataFormModel;
class HbDataForm;
class HbDataFormViewItem;
class HbRadioButtonList;

class CPULoadAttributes
    {
public:
    uint mId;        
    uint mPriority;        
    uint mMode;        
    uint mType;        
    uint mLength;        
    uint mIdle;        
    uint mRandomVariance;
    int  mCpu;
    uint mCpuCount;
    };

class MemoryEatAttributes
    {
public:
    uint    mId;        
    uint    mPriority;        
    uint    mSource;        
    uint    mType;
    uint    mBuffer;
    uint    mIdle;
    qint64  mAmount;
    qint64  mRandomMin;
    qint64  mRandomMax;
    QString mAmountDes;
    QString mRandomMinDes;
    QString mRandomMaxDes;
    uint    mRandomVariance;        
    }; 

class PhoneCallAttributes
    {
public:
    uint    mId;        
    uint    mPriority;        
    QString mDestination;
    uint    mLength;        
    uint    mIdle;        
    uint    mRandomVariance;        
    };
	
class NetConnAttributes
    {
public:
    uint    mId;        
    uint    mPriority;        
    QString mDestination;
    uint    mIdle;        
    uint    mRandomVariance;        
    };
	
class KeyPressAttributes
    {
public:
    uint mId;        
    uint mPriority;        
    uint mHeartBeat;        
    uint mRandomVariance;        
    };	
	
class PointerEventAttributes
    {
public:
    uint mId;        
    uint mPriority;        
    uint mHeartBeat;        
    uint mRandomVariance;        
    };

class MessageAttributes
    {
public:
    uint    mId;
    int     mMessageType;
    QString mDestination;
    uint    mPriority; 
    uint    mAmount;
    int     mLength;    
    uint    mIdle;    
    uint    mRandomVariance;        
    };

class ApplicationsAttributes
    {
public:
    uint        mId;        
    uint        mPriority;
    uint        mLaunchingInterval;
    uint        mKeyPressType;
    uint        mMaxOpen;
    uint        mHeartBeat;
    uint        mRandomVariance;
    QStringList mAppsArray;

    };


class PhotoCaptureAttributes
    {
public:
    uint mId;    
    int  mPriority;
    int  mCameraCount;
    int  mCamera;
    uint mIdle;    
    uint mRandomVariance;        
    };

class BluetoothAttributes
    {
public:
    uint mId;    
    int mPriority;    
    uint mIdle;    
    uint mRandomVariance;        
    bool mBluetoothSupported;
    };
	

class SettingsView : public HbView
{
    Q_OBJECT
    
public:
    SettingsView(HbView &mainView, HbMainWindow &parent, EngineWrapper &engine);
    ~SettingsView();
    void open(const QString &settingsName, TLoadGenCommandIds cmdId);

signals:
	void loadCompleted(TLoadGenCommandIds cmdId);
	void loadCanclled();
	
private slots:
   void backButtonPressed();
   void okExit();
   void cancelled();
   void selectionChanged(int index);
   void dataItemDisplayed(const QModelIndex &index);
   
private:
    void createItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createCpuLoadItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createEatMemoryItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createPhoneCallItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createNetConnItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createKeyOrPointerItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createMessageItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createAppsItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createPhotoItems(HbDataFormModel *model, HbDataFormModelItem *parent);
	void createBtItems(HbDataFormModel *model, HbDataFormModelItem *parent);
    void loadAttributes();
    void saveSettings();
	void saveCpuLoadSettings();
	void saveEatMemorySettings();
	void savePhoneCallSettings();
	void saveNetConnSettings();
	void saveKeyPressSettings();
	void saveMessageSettings();
	void savePointerEventSettings();
	void saveApplicationsSettings();
	void savePhotoSettings();
	void saveBtSettings();
    void exit();
    void setDefaultSettings();
	void setLoadAttributes();
    
private:
    HbMainWindow &mMainWindow;
    HbView &mMainView;
    EngineWrapper &mEngineWrapper;
	TLoadGenCommandIds mCmdId;

    /* Setting form */
    HbDataForm *mSettingForm;
    
    /* Model for Setting form */
    HbDataFormModel *mModel;
    
    /* Components for the model: */   

    /* grouped load settings */
    HbDataFormModelItem *mLoadSettings;
    
    HbDataFormModelItem *mPriority;
    HbDataFormModelItem *mLoadMode;
	HbDataFormModelItem *mSource;
    HbDataFormModelItem *mType;
	HbDataFormModelItem *mAmount;
	HbDataFormModelItem *mMinAmountToBeLeft;
	HbDataFormModelItem *mMaxAmountToBeLeft;
	HbDataFormModelItem *mBuffer;
    HbDataFormModelItem *mLength;
	HbDataFormModelItem *mDestination;
	HbDataFormModelItem *mIdle;
	HbDataFormModelItem *mHeartBeat;
	HbDataFormModelItem *mRandomVar;
	HbDataFormModelItem *mMaxParallelApps;
	HbDataFormModelItem *mLaunchingInterval;
	HbDataFormModelItem *mKeyPressType;
	HbDataFormModelItem *mCamera;
	HbDataFormModelItem *mCpu;
	
	HbRadioButtonList *mModes;
	HbRadioButtonList *mTypes;
	
	/* load attributes for each load type*/
	CPULoadAttributes      mCPULoadAttributes;
	MemoryEatAttributes    mMemoryEatAttributes;
	PhoneCallAttributes	   mPhoneCallAttributes;
	NetConnAttributes	   mNetConnAttributes;
	KeyPressAttributes     mKeyPressAttributes;
	PointerEventAttributes mPointerEventAttributes;
	MessageAttributes      mMessageAttributes;
	ApplicationsAttributes mApplicationsAttributes;
	PhotoCaptureAttributes mPhotoCaptureAttributes;
	BluetoothAttributes    mBluetoothAttributes;
};

#endif // SETTINGSVIEW_H
