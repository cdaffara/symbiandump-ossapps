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


#include <hbmainwindow.h>
//#include <hbcommonnote.h>
#include <hbpopup.h>
#include <hbaction.h>

#include <QString>
 #include <QStringList>
 
#include <e32std.h>
#include <e32base.h>

#include "notifications.h"
#include "enginewrapper.h"
#include "settingsview.h"
#include "engine.h"
#include "mainview.h"

QStringList LOADTYPES = (QStringList() << "CPU load" << "Memory eat" << "Phone calls" << "Messages" 
									   << "Network conn." << "Key presses" << "Applications" << "Photo captures" 
									   << "Bluetooth actions" << "Pointer events");

// ---------------------------------------------------------------------------

EngineWrapper::EngineWrapper(HbMainWindow &mainWindow, MainView &mainView)
: mEngine(0), 
  mMainView(mainView),
  mMainWindow(mainWindow),
  mEditExistingLoad(false)
{
}

// ---------------------------------------------------------------------------

EngineWrapper::~EngineWrapper()
{
    if (mEngine != 0) {
		TRAP_IGNORE(mEngine->DeActivateEngineL());
        delete mEngine;
    }
}

// ---------------------------------------------------------------------------

bool EngineWrapper::init()
{
	TInt err = KErrNone;
    TRAP(err, mEngine = CEngine::NewL(this));
    if(err != KErrNone) {
        return false;
    }
    else {
		TRAP(err, mEngine->ActivateEngineL());
		if (err != KErrNone) {
			return false;
		}
		else {
			return true;
		}
    }
}

// ---------------------------------------------------------------------------	

bool EngineWrapper::startNewLoad(int commandId)
{
	TRAPD(err, mEngine->StartNewLoadL(commandId));

	// error handling
    if(err != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::launchPerfMonApp()
{
	TRAPD(err, mEngine->LaunchPerfMonL());
	// error handling
    if(err != KErrNone) {
        Notifications::error("Perf Mon launch failed.");
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::loadAddedOrEdited(TLoadGenCommandIds cmdId)
{
	if (mEditExistingLoad == false) {
		TRAP_IGNORE(mEngine->DoStartNewLoadL(cmdId));
	}
	else {
		TRAP_IGNORE(mEngine->ExistingLoadEditedL());
	}
}

// ---------------------------------------------------------------------------

void EngineWrapper::loadSettingsCanclled()
{
	if (mEditExistingLoad == true) {
		TRAP_IGNORE(mEngine->ExistingLoadEditCancelled());
	}
}

// ---------------------------------------------------------------------------

void EngineWrapper::loadEdit(int rowIndex)
{
	TRAPD(err, mEngine->EditLoadL(rowIndex));
	// error handling
    if(err != KErrNone) {
        Notifications::error("Load edit failed.");
    }
}

void EngineWrapper::StopLoadYesNoDialogClosed(HbAction *action)
{
    if( action && !action->text().compare("yes", Qt::CaseInsensitive) ){
        const CArrayFix<TInt>* selectionIndexes = NULL;
        try{
            QT_TRAP_THROWING( selectionIndexes = QueryListSelectedIndexesOrCurrentItemL() );
            if(selectionIndexes)
                QT_TRAP_THROWING( mEngine->StopSelectedOrHighlightedItemsL(selectionIndexes) );
        }
        catch(...){
            Notifications::error("Stop load failed.");
        }
        if(selectionIndexes)
            delete selectionIndexes;
    }
}

// ---------------------------------------------------------------------------
bool EngineWrapper::stopLoad(bool stopAll)
{
	TInt err = KErrNone;
	if (stopAll == true) {
		TRAP(err, mEngine->StopAllLoadItemsL());
		// error handling
        if(err != KErrNone) {
            return false;
        }
        else {
            return true;
        }   
	}
	else {
        // by default use selected items
        QList<int> listIndices = mMainView.listSelectionIndexes();
        if (listIndices.count() > 0)
            {
            QString message = QString("Stop %1 selections?").arg( listIndices.count() );
            HbMessageBox::question(message, this, SLOT(StopLoadYesNoDialogClosed(HbAction *)), HbMessageBox::Yes|HbMessageBox::No);
            }
        else{
             TInt currentItemIndex = mMainView.currentItemIndex();
             if (mEngine->LoadItemCount() > currentItemIndex && currentItemIndex >= 0)
                 {
                 QString message("Stop highlighted selections?");
                 HbMessageBox::question(message, this, SLOT(StopLoadYesNoDialogClosed(HbAction *)), HbMessageBox::Yes|HbMessageBox::No);
                 }
            }
        return true;
	}
	
}

// ---------------------------------------------------------------------------

void EngineWrapper::resumeAllLoadItems()
{
	TRAPD(err, mEngine->ResumeAllLoadItemsL());
	// error handling
    if(err != KErrNone) {
        Notifications::error("Error in loads resume.");
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::suspendAllLoadItems()
{
	TRAPD(err, mEngine->SuspendAllLoadItemsL());
	// error handling
    if(err != KErrNone) {
        Notifications::error("Error in loads resume.");
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::suspendOrResumeSelectedOrHighlightedItems()
{
	TRAPD(err, mEngine->SuspendOrResumeSelectedOrHighlightedItemsL());
	// error handling
    if(err != KErrNone) {
        Notifications::error("Error to show item action menu.");
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowErrorMessage(const TDesC& aErrorMessage)
{
    QString errorMessage((QChar*)aErrorMessage.Ptr(),aErrorMessage.Length());
    Notifications::error(errorMessage);
 
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowNote(const TDesC& aNoteMessage)
{
    QString note((QChar*)aNoteMessage.Ptr(),aNoteMessage.Length());
    Notifications::showGlobalNote(note, HbMessageBox::MessageTypeInformation, HbPopup::StandardTimeout);
}
 
// ---------------------------------------------------------------------------

TInt EngineWrapper::QueryCurrentItemIndex()
{
	return mMainView.currentItemIndex();
}

// ---------------------------------------------------------------------------

const CArrayFix<TInt>*  EngineWrapper::QueryListSelectedIndexesOrCurrentItemL()
{
	QList<int> listIndices = mMainView.listSelectionIndexes();
	TInt cnt = listIndices.count();
    CArrayFix<TInt>* indices = new(ELeave)CArrayFixFlat<TInt>( cnt > 0 ? cnt : 1 );
    CleanupDeletePushL(indices);
    if(cnt > 0){
        for (TInt i = 0; i < cnt; i++) {
            indices->AppendL(listIndices.at(i));
        }
    }
    else{
        indices->AppendL(mMainView.currentItemIndex());
    }
    CleanupStack::Pop(indices);
    return static_cast<const CArrayFix<TInt>*>(indices);	
}

// ---------------------------------------------------------------------------

void EngineWrapper::OpenLoadSettings(TInt loadType, bool editExistingLoad)
{
	SettingsView* settings = new SettingsView((HbView&)ViewReference(), 
											  WindowReference(), 
											  *this);
	mEditExistingLoad = editExistingLoad;

	switch (loadType) {
		case ELoadGenCmdNewLoadCPULoad: {
			settings->open(LOADTYPES.at(Cpu), 
			               (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadEatMemory: {
			settings->open(LOADTYPES.at(EatMemory), 
			               (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadPhoneCall: {
			settings->open(LOADTYPES.at(PhoneCalls), 
			               (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadNetConn: {
			settings->open(LOADTYPES.at(NWConnections),
                           (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadKeyPress: {
			settings->open(LOADTYPES.at(KeyPresses), 
			               (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadMessages: {
			settings->open(LOADTYPES.at(Messages), 
			               (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadApplications: {
			settings->open(LOADTYPES.at(Apps), 
			               (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadPhotoCaptures: {
			settings->open(LOADTYPES.at(Photos), 
			               (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadBluetooth: {
			settings->open(LOADTYPES.at(BTAct), 
						   (TLoadGenCommandIds)loadType);
			break;
		}
		case ELoadGenCmdNewLoadPointerEvent: {
			settings->open(LOADTYPES.at(PointerEvents),
						   (TLoadGenCommandIds)loadType);
			break;
		}
		default: {
			User::Panic(_L("Wrong new load"), 111);
			break;
		}
	}
}

// ---------------------------------------------------------------------------	

CPULoadAttributes EngineWrapper::getCpuLoadAttributes()
{
    TCPULoadAttributes tCpuLoadAttributes = mEngine->GetCPULoadAttributes();
    CPULoadAttributes attributes;
	
	attributes.mId = tCpuLoadAttributes.iId;
	attributes.mPriority = tCpuLoadAttributes.iPriority;
	attributes.mMode = tCpuLoadAttributes.iMode;
	attributes.mType = tCpuLoadAttributes.iType;
	attributes.mLength = tCpuLoadAttributes.iLength;
	attributes.mIdle = tCpuLoadAttributes.iIdle;
	attributes.mRandomVariance = tCpuLoadAttributes.iRandomVariance;
	attributes.mCpu = tCpuLoadAttributes.iCpu;
	attributes.mCpuCount = tCpuLoadAttributes.iCpuCount;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setCpuLoadAttributes(const CPULoadAttributes& attributes)
{
    TCPULoadAttributes tCpuLoadAttributes;

	tCpuLoadAttributes.iId = attributes.mId;
	tCpuLoadAttributes.iPriority = attributes.mPriority;
	tCpuLoadAttributes.iMode = attributes.mMode;
	tCpuLoadAttributes.iType = attributes.mType;
	tCpuLoadAttributes.iLength = attributes.mLength;
	tCpuLoadAttributes.iIdle = attributes.mIdle;
	tCpuLoadAttributes.iRandomVariance = attributes.mRandomVariance;
	tCpuLoadAttributes.iCpu = attributes.mCpu;
	tCpuLoadAttributes.iCpuCount = attributes.mCpuCount;	
	
	mEngine->ChangeCPULoadAttributes(tCpuLoadAttributes);
}

// ---------------------------------------------------------------------------	

MemoryEatAttributes EngineWrapper::getMemoryEatAttributes()
{

    TMemoryEatAttributes tMemoryEatAttributes = mEngine->GetMemoryEatAttributes();
    MemoryEatAttributes attributes;

    // Convert TMemoryEatAttributes to MemoryEatAttributes 
    attributes.mId         = tMemoryEatAttributes.iId;
    attributes.mPriority   = tMemoryEatAttributes.iPriority; 
	attributes.mSource     = tMemoryEatAttributes.iSource;
	attributes.mType       = tMemoryEatAttributes.iType;
	attributes.mBuffer     = tMemoryEatAttributes.iBuffer;
	attributes.mIdle       = tMemoryEatAttributes.iIdle;
	attributes.mAmount     = tMemoryEatAttributes.iAmount;
	attributes.mRandomMin  = tMemoryEatAttributes.iRandomMin;
	attributes.mRandomMax  = tMemoryEatAttributes.iRandomMax;

    TBuf<64> amount;
    amount.Copy(tMemoryEatAttributes.iAmountDes);
    attributes.mAmountDes = QString((QChar*)amount.Ptr(), amount.Length()); 

    TBuf<64> min;
    min.Copy(tMemoryEatAttributes.iRandomMinDes);
    attributes.mAmountDes = QString((QChar*)min.Ptr(), min.Length()); 

    TBuf<64> max;
    max.Copy(tMemoryEatAttributes.iRandomMaxDes);
    attributes.mAmountDes = QString((QChar*)max.Ptr(), max.Length());
	
	attributes.mRandomVariance  = tMemoryEatAttributes.iRandomVariance;
    return attributes;
}	

// ---------------------------------------------------------------------------	

void EngineWrapper::setMemoryEatAttributes(const MemoryEatAttributes& attributes)
{
	TMemoryEatAttributes tMemoryEatAttributes = mEngine->GetMemoryEatAttributes();//TMemoryEatAttributes tMemoryEatAttributes;

	tMemoryEatAttributes.iId = attributes.mId;
	tMemoryEatAttributes.iPriority = attributes.mPriority;
	tMemoryEatAttributes.iSource = attributes.mSource;
	tMemoryEatAttributes.iType = attributes.mType;
	tMemoryEatAttributes.iBuffer = attributes.mBuffer;
	tMemoryEatAttributes.iIdle = attributes.mIdle;
	tMemoryEatAttributes.iAmount = attributes.mAmount;
	tMemoryEatAttributes.iRandomMin = attributes.mRandomMin;
	tMemoryEatAttributes.iRandomMax = attributes.mRandomMax;

    TBuf<64> amount = attributes.mAmountDes.utf16();
    tMemoryEatAttributes.iAmountDes.Copy(amount);	
	
    TBuf<64> min = attributes.mRandomMinDes.utf16();
    tMemoryEatAttributes.iRandomMinDes.Copy(min);
      
    TBuf<64> max = attributes.mRandomMaxDes.utf16();
    tMemoryEatAttributes.iRandomMaxDes.Copy(max);
	
	tMemoryEatAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangeMemoryEatAttributes(tMemoryEatAttributes);
}

// ---------------------------------------------------------------------------	

PhoneCallAttributes EngineWrapper::getPhoneCallAttributes()
{
    TPhoneCallAttributes tPhoneCallAttributes = mEngine->GetPhoneCallAttributes();
    PhoneCallAttributes attributes;
	
	attributes.mId = tPhoneCallAttributes.iId;
	attributes.mPriority = tPhoneCallAttributes.iPriority;

    TBuf<128> dest;
    dest.Copy(tPhoneCallAttributes.iDestination);
    attributes.mDestination = QString((QChar*)dest.Ptr(), dest.Length()); 	
	
	attributes.mLength = tPhoneCallAttributes.iLength;
	attributes.mIdle = tPhoneCallAttributes.iIdle;
	attributes.mRandomVariance = tPhoneCallAttributes.iRandomVariance;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setPhoneCallAttributes(const PhoneCallAttributes& attributes)
{
	TPhoneCallAttributes tPhoneCallAttributes = mEngine->GetPhoneCallAttributes(); //TPhoneCallAttributes tPhoneCallAttributes;

	tPhoneCallAttributes.iId = attributes.mId;
	tPhoneCallAttributes.iPriority = attributes.mPriority;

    TBuf<128> dest = attributes.mDestination.utf16();
    tPhoneCallAttributes.iDestination.Copy(dest);	

	tPhoneCallAttributes.iLength = attributes.mLength;
	tPhoneCallAttributes.iIdle = attributes.mIdle;
	tPhoneCallAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangePhoneCallAttributes(tPhoneCallAttributes);
}

// ---------------------------------------------------------------------------	

NetConnAttributes EngineWrapper::getNetConnAttributes()
{
    TNetConnAttributes tNetConnAttributes = mEngine->GetNetConnAttributes();
    NetConnAttributes attributes;
	
	attributes.mId = tNetConnAttributes.iId;
	attributes.mPriority = tNetConnAttributes.iPriority;

    attributes.mDestination = QString::fromUtf16(tNetConnAttributes.iDestination.Ptr(), tNetConnAttributes.iDestination.Length()); 	

	attributes.mIdle = tNetConnAttributes.iIdle;
	attributes.mRandomVariance = tNetConnAttributes.iRandomVariance;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setNetConnAttributes(const NetConnAttributes& attributes)
{
	TNetConnAttributes tNetConnAttributes = mEngine->GetNetConnAttributes(); //TNetConnAttributes tNetConnAttributes;

	tNetConnAttributes.iId = attributes.mId;
	tNetConnAttributes.iPriority = attributes.mPriority;

    TBuf<128> dest = attributes.mDestination.utf16();
    tNetConnAttributes.iDestination.Copy(dest);	

	tNetConnAttributes.iIdle = attributes.mIdle;
	tNetConnAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangeNetConnAttributes(tNetConnAttributes);
}

// ---------------------------------------------------------------------------	

KeyPressAttributes EngineWrapper::getKeyPressAttributes()
{
    TKeyPressAttributes tKeyPressAttributes = mEngine->GetKeyPressAttributes();
    KeyPressAttributes attributes;
	
	attributes.mId = tKeyPressAttributes.iId;
	attributes.mPriority = tKeyPressAttributes.iPriority;
	attributes.mHeartBeat = tKeyPressAttributes.iHeartBeat;	
	attributes.mRandomVariance = tKeyPressAttributes.iRandomVariance;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setKeyPressAttributes(const KeyPressAttributes& attributes)
{
	TKeyPressAttributes tKeyPressAttributes = mEngine->GetKeyPressAttributes(); //TKeyPressAttributes tKeyPressAttributes;

	tKeyPressAttributes.iId = attributes.mId;
	tKeyPressAttributes.iPriority = attributes.mPriority;
	tKeyPressAttributes.iHeartBeat = attributes.mHeartBeat;
	tKeyPressAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangeKeyPressAttributes(tKeyPressAttributes);
}

// ---------------------------------------------------------------------------	

PointerEventAttributes EngineWrapper::getPointerEventAttributes()
{
    TPointerEventAttributes tPointerEventAttributes = mEngine->GetPointerEventAttributes();
    PointerEventAttributes attributes;
	
	attributes.mId = tPointerEventAttributes.iId;
	attributes.mPriority = tPointerEventAttributes.iPriority;
	attributes.mHeartBeat = tPointerEventAttributes.iHeartBeat;	
	attributes.mRandomVariance = tPointerEventAttributes.iRandomVariance;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setPointerEventAttributes(const PointerEventAttributes& attributes)
{
	TPointerEventAttributes tPointerEventAttributes = mEngine->GetPointerEventAttributes();//TPointerEventAttributes tPointerEventAttributes;

	tPointerEventAttributes.iId = attributes.mId;
	tPointerEventAttributes.iPriority = attributes.mPriority;
	tPointerEventAttributes.iHeartBeat = attributes.mHeartBeat;
	tPointerEventAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangePointerEventAttributes(tPointerEventAttributes);
}

// ---------------------------------------------------------------------------	

MessageAttributes EngineWrapper::getMessageAttributes()
{
    TMessageAttributes tMessageAttributes = mEngine->GetMessageAttributes();
    MessageAttributes attributes;
	
	attributes.mId = tMessageAttributes.iId;
	attributes.mMessageType = tMessageAttributes.iMessageType;

    TBuf<128> dest;
    dest.Copy(tMessageAttributes.iDestination);
    attributes.mDestination = QString((QChar*)dest.Ptr(), dest.Length()); 	
	
	attributes.mPriority = tMessageAttributes.iPriority;
	attributes.mAmount = tMessageAttributes.iAmount;
	attributes.mLength = tMessageAttributes.iLength;
	attributes.mIdle = tMessageAttributes.iIdle;
	attributes.mRandomVariance = tMessageAttributes.iRandomVariance;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setMessageAttributes(const MessageAttributes& attributes)
{
	TMessageAttributes tMessageAttributes = mEngine->GetMessageAttributes();//TMessageAttributes tMessageAttributes;

	tMessageAttributes.iId = attributes.mId;
	tMessageAttributes.iMessageType = attributes.mMessageType;
    
	TBuf<128> dest = attributes.mDestination.utf16();
    tMessageAttributes.iDestination.Copy(dest);	
	
	tMessageAttributes.iPriority = attributes.mPriority;
	tMessageAttributes.iAmount = attributes.mAmount;
	tMessageAttributes.iLength = attributes.mLength;
	tMessageAttributes.iIdle = attributes.mIdle;
	tMessageAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangeMessageAttributes(tMessageAttributes);
}

// ---------------------------------------------------------------------------	

ApplicationsAttributes EngineWrapper::getApplicationsAttributes()
{
    TApplicationsAttributes tApplicationsAttributes = mEngine->GetApplicationsAttributes();
    ApplicationsAttributes attributes;
	
	attributes.mId = tApplicationsAttributes.iId;
	attributes.mPriority = tApplicationsAttributes.iPriority;
	attributes.mLaunchingInterval = tApplicationsAttributes.iLaunchingInterval;
	attributes.mKeyPressType = tApplicationsAttributes.iKeyPressType;
	attributes.mMaxOpen = tApplicationsAttributes.iMaxOpen;
	attributes.mHeartBeat = tApplicationsAttributes.iHeartBeat;
	attributes.mRandomVariance = tApplicationsAttributes.iRandomVariance;

	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setApplicationsAttributes(const ApplicationsAttributes& attributes)
{
	TApplicationsAttributes tApplicationsAttributes = mEngine->GetApplicationsAttributes(); //TApplicationsAttributes tApplicationsAttributes;

	tApplicationsAttributes.iId = attributes.mId;
	tApplicationsAttributes.iPriority = attributes.mPriority;
	tApplicationsAttributes.iLaunchingInterval = attributes.mLaunchingInterval;
	tApplicationsAttributes.iKeyPressType = attributes.mKeyPressType;
	tApplicationsAttributes.iMaxOpen = attributes.mMaxOpen;
	tApplicationsAttributes.iHeartBeat = attributes.mHeartBeat;	
	tApplicationsAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangeApplicationsAttributes(tApplicationsAttributes);
}

    QStringList mAppsArray;

// ---------------------------------------------------------------------------	

PhotoCaptureAttributes EngineWrapper::getPhotoCaptureAttributes()
{
    TPhotoCaptureAttributes tPhotoCaptureAttributes = mEngine->GetPhotoCaptureAttributes();
    PhotoCaptureAttributes attributes;
	
	attributes.mId = tPhotoCaptureAttributes.iId;
	attributes.mPriority = tPhotoCaptureAttributes.iPriority;
	attributes.mCameraCount = tPhotoCaptureAttributes.iCameraCount;	
	attributes.mCamera = tPhotoCaptureAttributes.iCamera;
	attributes.mIdle = tPhotoCaptureAttributes.iIdle;
	attributes.mRandomVariance = tPhotoCaptureAttributes.iRandomVariance;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setPhotoCaptureAttributes(const PhotoCaptureAttributes& attributes)
{
	TPhotoCaptureAttributes tPhotoCaptureAttributes;

	tPhotoCaptureAttributes.iId = attributes.mId;
	tPhotoCaptureAttributes.iPriority = attributes.mPriority;
	tPhotoCaptureAttributes.iCameraCount = attributes.mCameraCount;	
	tPhotoCaptureAttributes.iCamera = attributes.mCamera;
	tPhotoCaptureAttributes.iIdle = attributes.mIdle;
	tPhotoCaptureAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangePhotoCaptureAttributes(tPhotoCaptureAttributes);
}

// ---------------------------------------------------------------------------	

BluetoothAttributes EngineWrapper::getBluetoothAttributes()
{
    TBluetoothAttributes tBluetoothAttributes = mEngine->GetBluetoothAttributes();
    BluetoothAttributes attributes;
	
	attributes.mId = tBluetoothAttributes.iId;
	attributes.mPriority = tBluetoothAttributes.iPriority;
	attributes.mIdle = tBluetoothAttributes.iIdle;
	attributes.mRandomVariance = tBluetoothAttributes.iRandomVariance;
	attributes.mBluetoothSupported = tBluetoothAttributes.iBluetoothSupported ? true : false;
	return attributes;
}

// ---------------------------------------------------------------------------	

void EngineWrapper::setBluetoothAttributes(const BluetoothAttributes& attributes)
{
    TBluetoothAttributes tBluetoothAttributes;

	tBluetoothAttributes.iId = attributes.mId;
	tBluetoothAttributes.iPriority = attributes.mPriority;
	tBluetoothAttributes.iIdle = attributes.mIdle;
	tBluetoothAttributes.iRandomVariance = attributes.mRandomVariance;
	
	mEngine->ChangeBluetoothAttributes(tBluetoothAttributes);
}

// ---------------------------------------------------------------------------

void EngineWrapper::ReFreshView(CDesCArray* aTextArray) 
{
	QStringList itemList;
	TInt arrayCnt = aTextArray->Count();
	for (int i = 0; i < arrayCnt; i++) {
		itemList.append(QString::fromUtf16(
				  aTextArray->MdcaPoint(i).Ptr(),
				  aTextArray->MdcaPoint(i).Length()));
	}
	mMainView.setLoadListData(itemList);
}

// ---------------------------------------------------------------------------	

void EngineWrapper::ClearSelection()
{
	mMainView.clearListSelection();
}

// ---------------------------------------------------------------------------	

void EngineWrapper::SetCurrentItemIndex(TInt aIndex) 
{
	mMainView.setCurrentItemIndex(aIndex); 
}

// ---------------------------------------------------------------------------	
