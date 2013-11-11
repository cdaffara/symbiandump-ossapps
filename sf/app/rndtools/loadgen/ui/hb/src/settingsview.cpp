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

#include "settingsview.h"
#include "enginewrapper.h"
#include "loadgen.hrh"
#include "loadgen_loadattributes.h"
#include <hbmainwindow.h>
#include <hbview.h>
//#include <hbformitem.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmessagebox.h>
//#include <hbcommonnote.h>
#include <hbtoolbar.h>
#include <hbdataform.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbdataformviewitem.h>
#include <hbabstractviewitem.h>
#include <hbradiobuttonlist.h>


#include <QStringList>

QStringList PRIORITIES = (QStringList() << "Much less (-20)" << "Less (-10)" << "Normal (0)" << "More (10)" 
						 << "Much more (20)" << "Real time (30)" << "Abs. very low (100)" << "Abs. low (200)" 
						 << "Abs. backgr. (300)" << "Abs. foregr. (400)" << "Abs. high (500)");
QStringList LOADMODES = (QStringList() << "Yielding" << "Blocking");
QStringList CPULOADTYPES = (QStringList() << "Continuous" << "Periodic");		   
QStringList MEMSOURCES = (QStringList() << "RAM" << "C:" << "D:" << "E:" << "F:" << "G:" << "H:" << "J:" << "K:" 
										<< "L:" << "M:" << "N:");
QStringList MEMEATTYPES = (QStringList() << "Memory to eat" << "Memory to be left" << "Alternate min/max");
QStringList KEYPRESSTYPES = (QStringList() << "None" << "Arrow keys");
QStringList MESSAGETYPES = (QStringList() << "SMS" << "MMS");

		   
SettingsView::SettingsView(HbView &mainView, HbMainWindow &parent, EngineWrapper &engine)
                          
    : mMainWindow(parent),
      mMainView(mainView),
      mEngineWrapper(engine),
	  mAmount(NULL),
	  mMinAmountToBeLeft(NULL),
	  mMaxAmountToBeLeft(NULL),
	  mLength(NULL),
	  mIdle(NULL),
	  mRandomVar(NULL)
{
}

SettingsView::~SettingsView()
{

}

// TODO HbDataFormModelItem data operations does not work correctly with WK38.
// Item data does not get updated correctly as set by initial settings(HbDataFormModelItem::RadioButtonListItem) 
// or user settings.(HbDataFormModelItem::TextItem)

// ---------------------------------------------------------------------------

void SettingsView::open(const QString &settingsName, TLoadGenCommandIds cmdId)
{
	mCmdId = cmdId;
    // Remove main view from main window and add this view to main window
    mMainWindow.addView(this);
    mMainWindow.setCurrentView(this);
    setTitle(settingsName);

    //create toolbar showing launch popup
    HbToolBar *toolBar = this->toolBar();
    HbAction *actionOk = new HbAction("Ok", toolBar);
    HbAction *actionCancel = new HbAction("Cancel", toolBar);
    toolBar->addAction( actionOk );
    toolBar->addAction( actionCancel );	

    //create setting form
    mSettingForm = new HbDataForm();
    
    //create a model class
    mModel = new HbDataFormModel(this);
	createItems(mModel, 0);
    // Set created model to form
    mSettingForm->setModel(mModel);
    setWidget(mSettingForm);
	loadAttributes();	
	
    connect(actionOk, SIGNAL(triggered()), this, SLOT(okExit()));
	connect(actionCancel, SIGNAL(triggered()), this, SLOT(cancelled()));
	connect(this, SIGNAL(aboutToClose()), this, SLOT(backButtonPressed()));
	//connect(mSettingForm, SIGNAL(activated(const QModelIndex)), this, SLOT(dataItemDisplayed(const QModelIndex)));
	show();
}

// ---------------------------------------------------------------------------

void SettingsView::createItems(HbDataFormModel *model, 
							   HbDataFormModelItem *parent)
{
	switch (mCmdId) {
		case ELoadGenCmdNewLoadCPULoad: {
			createCpuLoadItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadEatMemory: {
			createEatMemoryItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadPhoneCall: {
			createPhoneCallItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadNetConn: {
			createNetConnItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadKeyPress:
		case ELoadGenCmdNewLoadPointerEvent: {
			createKeyOrPointerItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadMessages: {
			createMessageItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadApplications: {
			createAppsItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadPhotoCaptures: {
			createPhotoItems(model, parent);
			break;
		}
		case ELoadGenCmdNewLoadBluetooth: {
			createBtItems(model, parent);
			break;
		}
		default: {
			break;
		}
	}
}

// ---------------------------------------------------------------------------

void SettingsView::createCpuLoadItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{

	mLoadSettings = model->appendDataFormGroup(QString("CPU load settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	// DataFormItem for load mode selection
	mLoadMode = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Load mode"), 
										  mLoadSettings);
	mLoadMode->setContentWidgetData(QString("items"), LOADMODES);		

	// DataFormItem for load type selection
	mType = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
									  QString("Type"), 
									  mLoadSettings);
	mType->setContentWidgetData(QString("items"), CPULOADTYPES);

	mSettingForm->addConnection(mType,SIGNAL(itemSelected(int)),this,SLOT(selectionChanged(int)));
}

// ---------------------------------------------------------------------------

void SettingsView::createEatMemoryItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{
	mLoadSettings = model->appendDataFormGroup(QString("Eat memory settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	// DataFormItem for memory eat type selection
	mSource = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
									  QString("Source"), 
									  mLoadSettings);
	mSource->setContentWidgetData(QString("items"), MEMSOURCES);

	
	// DataFormItem for memory eat type selection
	mType = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
									  QString("Type"), 
									  mLoadSettings);
	mType->setContentWidgetData(QString("items"), MEMEATTYPES);

	// DataFormItem for idle length selection
	mBuffer = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Buffer (B)"), 
									  mLoadSettings);
	mBuffer->setContentWidgetData("maximum" , 999999999);
	mBuffer->setContentWidgetData("minimum", 0);	
	
	// DataFormItem for idle length selection
	mIdle = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Idle after buffer (ms)"), 
									  mLoadSettings);
	mIdle->setContentWidgetData("maximum" , 999999999);
	mIdle->setContentWidgetData("minimum", 0);			

	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
	
	mSettingForm->addConnection(mType,SIGNAL(itemSelected(int)),this,SLOT(selectionChanged(int)));
}	

// ---------------------------------------------------------------------------

void SettingsView::createPhoneCallItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{

	mLoadSettings = model->appendDataFormGroup(QString("Phone calls settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	mDestination = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
										QString("Phone number:"), 
										mLoadSettings);
	mDestination->setContentWidgetData("maximum" , 99999999999999999999.0 );
	mDestination->setContentWidgetData("minimum", 0);

	mLength = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
										QString("Length (ms)"), 
										mLoadSettings);
	mLength->setContentWidgetData("maximum" , 999999999);
	mLength->setContentWidgetData("minimum", 0);
	
	// DataFormItem for idle length selection
	mIdle = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Idle (ms)"), 
									  mLoadSettings);
	mIdle->setContentWidgetData("maximum" , 999999999);
	mIdle->setContentWidgetData("minimum", 0);			

	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
}

// ---------------------------------------------------------------------------

void SettingsView::createNetConnItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{

	mLoadSettings = model->appendDataFormGroup(QString("Network conn. settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	mDestination = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
										QString("Destination URL:"), 
										mLoadSettings);
	
	// DataFormItem for idle length selection
	mIdle = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Idle (ms)"), 
									  mLoadSettings);
	mIdle->setContentWidgetData("maximum" , 999999999);
	mIdle->setContentWidgetData("minimum", 0);			

	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
}

// ---------------------------------------------------------------------------

void SettingsView::createKeyOrPointerItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{
	QString settingsName;
	if (mCmdId == ELoadGenCmdNewLoadPointerEvent) {
		settingsName = "Pointer events settings";
	}
	else {
		settingsName = "Key presses settings";
	}
	mLoadSettings = model->appendDataFormGroup(settingsName, parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	
	// DataFormItem for heartbeat length selection
	mHeartBeat = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Heartbeat (ms)"), 
									  mLoadSettings);
	mHeartBeat->setContentWidgetData("maximum" , 999999999);
	mHeartBeat->setContentWidgetData("minimum", 0);		
	
	// Random variance
	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
}

// ---------------------------------------------------------------------------

void SettingsView::createMessageItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{
	mLoadSettings = model->appendDataFormGroup(QString("Messages settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	
	mType = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
									  QString("Type"), 
									  mLoadSettings);
	mType->setContentWidgetData(QString("items"), MESSAGETYPES);	


	mDestination = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
										QString("Phone number:"), 
										mLoadSettings);
	mDestination->setContentWidgetData("maximum" , 99999999999999999999.0 );
	mDestination->setContentWidgetData("minimum", 0);
	
	mAmount	= mModel->insertDataFormItem(3, HbDataFormModelItem::TextItem, 
									  QString("Maximum amount of messages"), 
									  mLoadSettings);
	mAmount->setContentWidgetData("maximum" , 1000);
	mAmount->setContentWidgetData("minimum", 0);

	mLength = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
										QString("Message length"), 
										mLoadSettings);
	mLength->setContentWidgetData("maximum" , 999999999);
	mLength->setContentWidgetData("minimum", 0);

	// DataFormItem for idle length selection
	mIdle = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Idle (ms)"), 
									  mLoadSettings);
	mIdle->setContentWidgetData("maximum" , 999999999);
	mIdle->setContentWidgetData("minimum", 0);			

	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
}

// ---------------------------------------------------------------------------

void SettingsView::createAppsItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{
	mLoadSettings = model->appendDataFormGroup(QString("Applications settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	mMaxParallelApps = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Max parallel applications"), 
									  mLoadSettings);
	mMaxParallelApps->setContentWidgetData("maximum" , 100);
	mMaxParallelApps->setContentWidgetData("minimum", 0);	


	// DataFormItem for launching interval length selection
	mLaunchingInterval = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Launching interval (ms)"), 
									  mLoadSettings);
	mLaunchingInterval->setContentWidgetData("maximum" , 999999999);
	mLaunchingInterval->setContentWidgetData("minimum", 0);	
	
	// DataFormItem for key press type selection
	mKeyPressType = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Key press type"), 
										  mLoadSettings);
	mKeyPressType->setContentWidgetData(QString("items"), KEYPRESSTYPES);	

	// DataFormItem for heartbeat length selection
	mHeartBeat = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Key press interval (ms)"), 
									  mLoadSettings);
	mHeartBeat->setContentWidgetData("maximum" , 999999999);
	mHeartBeat->setContentWidgetData("minimum", 0);		
	
	// Random variance
	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
}

// ---------------------------------------------------------------------------

void SettingsView::createPhotoItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{
	mLoadSettings = model->appendDataFormGroup(QString("Camera settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	
	// DataFormItem for idle length selection
	mIdle = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Idle (ms)"), 
									  mLoadSettings);
	mIdle->setContentWidgetData("maximum" , 999999999);
	mIdle->setContentWidgetData("minimum", 0);		
	
	// Random variance
	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
}

// ---------------------------------------------------------------------------

void SettingsView::createBtItems(HbDataFormModel *model, HbDataFormModelItem *parent)
{
	mLoadSettings = model->appendDataFormGroup(QString("Bluetooth settings"), parent);
	
	// DataFormItem for priority selection
	mPriority = model->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
										  QString("Priority"), 
										  mLoadSettings);
	mPriority->setContentWidgetData(QString("items"), PRIORITIES);

	
	// DataFormItem for idle length selection
	mIdle = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
									  QString("Idle (ms)"), 
									  mLoadSettings);
	mIdle->setContentWidgetData("maximum" , 999999999);
	mIdle->setContentWidgetData("minimum", 0);		
	
	// Random variance
	mRandomVar = model->appendDataFormItem(HbDataFormModelItem::TextItem, 
										   QString("Random variance (%)"), 
										   mLoadSettings);
	mRandomVar->setContentWidgetData("maximum" , 100);
	mRandomVar->setContentWidgetData("minimum", 0);	
}

// ---------------------------------------------------------------------------
// Load attributes from engine & set the data into each form model item:

void SettingsView::loadAttributes()
{
	switch (mCmdId)
	{
		case ELoadGenCmdNewLoadCPULoad: {
			mCPULoadAttributes = mEngineWrapper.getCpuLoadAttributes();
			mPriority->setContentWidgetData(QString("selected"), mCPULoadAttributes.mPriority);
			mLoadMode->setContentWidgetData(QString("selected"), mCPULoadAttributes.mMode);
			mType->setContentWidgetData(QString("selected"), mCPULoadAttributes.mType);
			
			if (mCPULoadAttributes.mType == ECpuLoadTypePeriodic) {	//if (mCPULoadAttributes.mMode == ECpuLoadTypePeriodic) {
			// Create and append to model these items: Length, Idle, Random variance, if load type is peridodic.
				
				// DataFormItem for load length selection
				mLength = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
													QString("Length (ms)"), 
													mLoadSettings);
				mLength->setContentWidgetData("maximum" , 999999999);
				mLength->setContentWidgetData("minimum", 0);
				
				
				// DataFormItem for load length selection
				mIdle = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
												  QString("Idle (ms)"), 
												  mLoadSettings);
				mIdle->setContentWidgetData("maximum" , 999999999);
				mIdle->setContentWidgetData("minimum", 0);			

				mRandomVar = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, 
													   QString("Random variance (%)"), 
													   mLoadSettings);
				mRandomVar->setContentWidgetData("maximum" , 100);
				mRandomVar->setContentWidgetData("minimum", 0);	
			
				mLength->setContentWidgetData(QString("text"), mCPULoadAttributes.mLength);
				mIdle->setContentWidgetData(QString("text"), mCPULoadAttributes.mIdle);
				mRandomVar->setContentWidgetData(QString("text"), mCPULoadAttributes.mRandomVariance);
			}

			// if there's more than one cpu available in the system, 
			// insert possibility to select which cpu the load is generated to
			if (mCPULoadAttributes.mCpuCount > 1)
            {
				QStringList cpuArray;
				cpuArray.append("Free scheduling");
				// add cpus to list:
				for (int i = 0; i < mCPULoadAttributes.mCpuCount; i++) 
					{
					QString cpu = QString("CPU%1").arg(i);
					cpuArray.append(cpu);
					}    
				// "All CPUs" means that one thread is started for each CPU.	
				cpuArray.append("All CPUs");

				mCpu = mModel->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem, 
													 QString("CPU"), 
													 mLoadSettings);
				mCpu->setContentWidgetData(QString("items"), cpuArray);				

				if (mCPULoadAttributes.mCpu < 0) {
					mCpu->setContentWidgetData(QString("selected"), 0);
				}				

				else if (mCPULoadAttributes.mCpu == KCPUSelection_AllCPUs) {
					mCpu->setContentWidgetData(QString("selected"), cpuArray.count()-1);
				}
				// CPU is defined, set the correct CPU as default selection in query list
				else {
				// mCpu starts from zero (i.e. CPU0 is first CPU), but index zero
				// in settings Ui form item is reserved for "Free scheduling", hence iCpu+1.
					mCpu->setContentWidgetData(QString("selected"), mCPULoadAttributes.mCpu+1);
				}
            }
			break;
		}
		case ELoadGenCmdNewLoadEatMemory: {
			mMemoryEatAttributes = mEngineWrapper.getMemoryEatAttributes();
			mPriority->setContentWidgetData(QString("selected"), mMemoryEatAttributes.mPriority);
			mSource->setContentWidgetData(QString("selected"), mMemoryEatAttributes.mSource);
			mType->setContentWidgetData(QString("selected"), mMemoryEatAttributes.mType);
			
			if (mMemoryEatAttributes.mType == EMemoryEatTypeWavy) {
				mMinAmountToBeLeft	= mModel->insertDataFormItem(3, HbDataFormModelItem::TextItem, 
												  QString("Min to be left (B)"), 
												  mLoadSettings);
				mMinAmountToBeLeft->setContentWidgetData("maximum" , 99999999999999);
				mMinAmountToBeLeft->setContentWidgetData("minimum", 0);
				
				mMaxAmountToBeLeft	= mModel->insertDataFormItem(4, HbDataFormModelItem::TextItem, 
												  QString("Max to be left (B)"), 
												  mLoadSettings);
				mMaxAmountToBeLeft->setContentWidgetData("maximum" , 99999999999999);
				mMaxAmountToBeLeft->setContentWidgetData("minimum", 0);

				mMinAmountToBeLeft->setContentWidgetData(QString("text"), mMemoryEatAttributes.mRandomMinDes);
				mMaxAmountToBeLeft->setContentWidgetData(QString("text"), mMemoryEatAttributes.mRandomMaxDes);
			}
			else if (mMemoryEatAttributes.mType == EMemoryEatTypeMemoryToEat ||
			         mMemoryEatAttributes.mType == EMemoryEatTypeMemoryToBeLeft ) {
				mAmount	= mModel->insertDataFormItem(3, HbDataFormModelItem::TextItem, 
												  QString("Amout(B)"), 
												  mLoadSettings);
				mAmount->setContentWidgetData("maximum" , 99999999999999);
				mAmount->setContentWidgetData("minimum", 0);
				
				mAmount->setContentWidgetData(QString("text"), mMemoryEatAttributes.mAmount);	 
			}
			mBuffer->setContentWidgetData(QString("text"), mMemoryEatAttributes.mBuffer);
			mIdle->setContentWidgetData(QString("text"), mMemoryEatAttributes.mIdle);
			mRandomVar->setContentWidgetData(QString("text"), mMemoryEatAttributes.mRandomVariance);
			break;
		}
		case ELoadGenCmdNewLoadPhoneCall: {
			mPhoneCallAttributes = mEngineWrapper.getPhoneCallAttributes();
			mPriority->setContentWidgetData(QString("selected"), mPhoneCallAttributes.mPriority);
			mDestination->setContentWidgetData(QString("text"), mPhoneCallAttributes.mDestination);
			mLength->setContentWidgetData(QString("text"), mPhoneCallAttributes.mLength);
			mIdle->setContentWidgetData(QString("text"), mPhoneCallAttributes.mIdle);
			mRandomVar->setContentWidgetData(QString("text"), mPhoneCallAttributes.mRandomVariance);
			break;
		}
		case ELoadGenCmdNewLoadNetConn: {
			mNetConnAttributes = mEngineWrapper.getNetConnAttributes();
			mPriority->setContentWidgetData(QString("selected"), mNetConnAttributes.mPriority);
			mDestination->setContentWidgetData(QString("text"), mNetConnAttributes.mDestination);
			mIdle->setContentWidgetData(QString("text"), mNetConnAttributes.mIdle);
			mRandomVar->setContentWidgetData(QString("text"), mNetConnAttributes.mRandomVariance);
			break;
		}
		case ELoadGenCmdNewLoadKeyPress: {
			mKeyPressAttributes = mEngineWrapper.getKeyPressAttributes();
			mPriority->setContentWidgetData(QString("selected"), mKeyPressAttributes.mPriority);
			mHeartBeat->setContentWidgetData(QString("text"), mKeyPressAttributes.mHeartBeat);
			mRandomVar->setContentWidgetData(QString("text"), mKeyPressAttributes.mRandomVariance);			
			break;
		}
		case ELoadGenCmdNewLoadPointerEvent: { 
			mPointerEventAttributes = mEngineWrapper.getPointerEventAttributes();
			mPriority->setContentWidgetData(QString("selected"), mPointerEventAttributes.mPriority);
			mHeartBeat->setContentWidgetData(QString("text"), mPointerEventAttributes.mHeartBeat);
			mRandomVar->setContentWidgetData(QString("text"), mPointerEventAttributes.mRandomVariance);				
			break;
		}
		case ELoadGenCmdNewLoadMessages: {
			mMessageAttributes = mEngineWrapper.getMessageAttributes();
			mPriority->setContentWidgetData(QString("selected"), mMessageAttributes.mPriority);
			mType->setContentWidgetData(QString("selected"), mMessageAttributes.mMessageType);
			mDestination->setContentWidgetData(QString("text"), mMessageAttributes.mDestination);
			mAmount->setContentWidgetData(QString("text"), mMessageAttributes.mAmount);	 
			mLength->setContentWidgetData(QString("text"), mMessageAttributes.mLength);
			mIdle->setContentWidgetData(QString("text"), mMessageAttributes.mIdle);
			mRandomVar->setContentWidgetData(QString("text"), mMessageAttributes.mRandomVariance);
			break;
		}
		case ELoadGenCmdNewLoadApplications: {
			mApplicationsAttributes = mEngineWrapper.getApplicationsAttributes();
			mPriority->setContentWidgetData(QString("selected"), mApplicationsAttributes.mPriority);
			mMaxParallelApps->setContentWidgetData(QString("text"), mApplicationsAttributes.mMaxOpen);
			mHeartBeat->setContentWidgetData(QString("text"), mApplicationsAttributes.mHeartBeat);
			mLaunchingInterval->setContentWidgetData(QString("text"), mApplicationsAttributes.mLaunchingInterval);
			mKeyPressType->setContentWidgetData(QString("selected"), mApplicationsAttributes.mKeyPressType);
			mRandomVar->setContentWidgetData(QString("text"), mApplicationsAttributes.mRandomVariance);						
			break;
		}
		case ELoadGenCmdNewLoadPhotoCaptures: {
			mPhotoCaptureAttributes = mEngineWrapper.getPhotoCaptureAttributes();
			mPriority->setContentWidgetData(QString("selected"), mPhotoCaptureAttributes.mPriority);
			mIdle->setContentWidgetData(QString("text"), mPhotoCaptureAttributes.mIdle);
			mRandomVar->setContentWidgetData(QString("text"), mPhotoCaptureAttributes.mRandomVariance);
			
			// if there's any cameras avaialable in system, construct form item for it/them:
			if (mPhotoCaptureAttributes.mCameraCount > 0)
            {
				QStringList devices;
				// add cameras to list:
				for (int i = 0; i < mPhotoCaptureAttributes.mCameraCount; i++) 
					{
					QString cam = QString("Camera %1").arg(i);
					devices.append(cam);
					}    
				mCamera = mModel->insertDataFormItem(1, HbDataFormModelItem::RadioButtonListItem, 
													 QString("Device"), 
													 mLoadSettings);
				mCamera->setContentWidgetData(QString("items"), devices);
				mCamera->setContentWidgetData(QString("selected"), mPhotoCaptureAttributes.mCamera);
            }
			break;
		}
		case ELoadGenCmdNewLoadBluetooth: {
			mBluetoothAttributes = mEngineWrapper.getBluetoothAttributes();
			mPriority->setContentWidgetData(QString("selected"), mBluetoothAttributes.mPriority);
			mIdle->setContentWidgetData(QString("text"), mBluetoothAttributes.mIdle);
			mRandomVar->setContentWidgetData(QString("text"), mBluetoothAttributes.mRandomVariance);	
			break;
		}
		
		default: {
			break;
		}
	}
}

// ---------------------------------------------------------------------------

void SettingsView::setDefaultSettings()
{

}

// ---------------------------------------------------------------------------

void SettingsView::backButtonPressed()
{
	okExit();	
}

// ---------------------------------------------------------------------------

void SettingsView::saveSettings()
{
	switch (mCmdId) {
		case ELoadGenCmdNewLoadCPULoad: {
			saveCpuLoadSettings();
			break;
		}
		case ELoadGenCmdNewLoadEatMemory: {
			saveEatMemorySettings();
			break;
		}
		case ELoadGenCmdNewLoadPhoneCall: {
			savePhoneCallSettings();
			break;
		}
		case ELoadGenCmdNewLoadNetConn: {
			saveNetConnSettings();
			break;
		}
		case ELoadGenCmdNewLoadKeyPress: {
			saveKeyPressSettings();
			break;
		}
		case ELoadGenCmdNewLoadPointerEvent: {
			savePointerEventSettings();
			break;
		}
		case ELoadGenCmdNewLoadMessages: {
			saveMessageSettings();
			break;
		}
		case ELoadGenCmdNewLoadApplications: {
			saveApplicationsSettings();
			break;
		}
		case ELoadGenCmdNewLoadPhotoCaptures: {
			savePhotoSettings();
			break;
		}
		case ELoadGenCmdNewLoadBluetooth: {
			saveBtSettings();
			break;
		}
		default: {
			break;
		}
	}
}

// ---------------------------------------------------------------------------	
	
void SettingsView::saveCpuLoadSettings()
{
	mCPULoadAttributes.mPriority = mPriority->contentWidgetData("selected").toInt();
	mCPULoadAttributes.mMode = mLoadMode->contentWidgetData("selected").toInt();
	mCPULoadAttributes.mType = mType->contentWidgetData("selected").toInt();
	if (mType->contentWidgetData("selected").toInt() == ECpuLoadTypePeriodic) {
		// TODO: remove temp checks if (mLength != NULL) etc. when HbDataFormModelItem bugs fixed.
		if (mLength != NULL) {
			mCPULoadAttributes.mLength = mLength->contentWidgetData("text").toInt();
		}
		if (mIdle != NULL) {			
			mCPULoadAttributes.mIdle = mIdle->contentWidgetData("text").toInt();
		}
		if (mRandomVar != NULL) {
			int randVar = mRandomVar->contentWidgetData("text").toInt();
			if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
				{
				mCPULoadAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
				}
			else
				{
				mCPULoadAttributes.mRandomVariance = mRandomVar->contentWidgetData("maximum").toInt();
				}
		}
	}

	// we are running in SMP environment
	if (mCPULoadAttributes.mCpuCount > 1)
		{
		TInt currentValueIndex = mCpu->contentWidgetData("selected").toInt();
		// user selected a specific CPU in which the load thread should be run in.
		if (currentValueIndex == 0)
			{
			// User selected "Free scheduling"
			mCPULoadAttributes.mCpu = KCPUSelection_FreeScheduling;
			}            
		else if( currentValueIndex == (mCpu->childCount()-1) )
			{
			// User selected "All CPUs", which is the last item in the form radio button selection list
			mCPULoadAttributes.mCpu = KCPUSelection_AllCPUs;
			}
		else
			{                
			// mCpu should start from zero (CPU0 is the first cpu)
			// but zero in Ui form item index means "Free scheduling". CPU0 in
			// index of form radio button list is at index 1, hence -1.
			mCPULoadAttributes.mCpu = currentValueIndex - 1;
			}
		}

	
}

// ---------------------------------------------------------------------------

void SettingsView::saveEatMemorySettings()
{
	mMemoryEatAttributes.mPriority = mPriority->contentWidgetData("selected").toInt();
	mMemoryEatAttributes.mSource = mSource->contentWidgetData("selected").toInt();
	mMemoryEatAttributes.mType = mType->contentWidgetData("selected").toInt();
	
	if (mType->contentWidgetData("selected").toInt() == EMemoryEatTypeWavy) {
		// TODO: remove temp checks if (mLength != NULL) etc. when HbDataFormModelItem bugs fixed.
		if (mMinAmountToBeLeft != NULL) {
			mMemoryEatAttributes.mRandomMin = 
				mMinAmountToBeLeft->contentWidgetData("text").toInt();
		}
		if (mMaxAmountToBeLeft != NULL) {
			mMemoryEatAttributes.mRandomMax = 
				mMaxAmountToBeLeft->contentWidgetData("text").toInt();
		}
	}
	else if (mType->contentWidgetData("selected").toInt() == EMemoryEatTypeMemoryToEat ||
	         mType->contentWidgetData("selected").toInt() == EMemoryEatTypeMemoryToBeLeft) {
		if (mAmount != NULL) {
			mMemoryEatAttributes.mAmount = mAmount->contentWidgetData("text").toInt();
		}
	}
	mMemoryEatAttributes.mBuffer = mBuffer->contentWidgetData("text").toInt();
	mMemoryEatAttributes.mIdle = mIdle->contentWidgetData("text").toInt();
	
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mMemoryEatAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mMemoryEatAttributes.mRandomVariance = mRandomVar->contentWidgetData("maximum").toInt();
		}
	
    // swap min- & max values if they are in wrong order
	if (mMemoryEatAttributes.mRandomMin > mMemoryEatAttributes.mRandomMax)
		{
		qint64 temp(mMemoryEatAttributes.mRandomMin);
		mMemoryEatAttributes.mRandomMin = mMemoryEatAttributes.mRandomMax;
		mMemoryEatAttributes.mRandomMax = temp; 
		}	
}

// ---------------------------------------------------------------------------	
	
void SettingsView::savePhoneCallSettings()
{
	mPhoneCallAttributes.mPriority = mPriority->contentWidgetData("selected").toInt();
	mPhoneCallAttributes.mLength   = mLength->contentWidgetData("text").toInt();
	mPhoneCallAttributes.mIdle     = mIdle->contentWidgetData("text").toInt();
	
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mPhoneCallAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mPhoneCallAttributes.mRandomVariance = mRandomVar->contentWidgetData("maximum").toInt();
		}
	
	
	mPhoneCallAttributes.mDestination    = mDestination->contentWidgetData("text").toString();
}

// ---------------------------------------------------------------------------	
	
void SettingsView::saveNetConnSettings()
{
	mPhoneCallAttributes.mPriority = mPriority->contentWidgetData("selected").toInt();
	mPhoneCallAttributes.mIdle     = mIdle->contentWidgetData("text").toInt();
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mPhoneCallAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mPhoneCallAttributes.mRandomVariance = mRandomVar->contentWidgetData("maximum").toInt();
		}
	mPhoneCallAttributes.mDestination    = mDestination->contentWidgetData("text").toString();
}

// ---------------------------------------------------------------------------

void SettingsView::saveKeyPressSettings()
{
	mKeyPressAttributes.mPriority  = mPriority->contentWidgetData("selected").toInt();
	mKeyPressAttributes.mHeartBeat = mHeartBeat->contentWidgetData("text").toInt();
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mKeyPressAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mKeyPressAttributes.mRandomVariance =  mRandomVar->contentWidgetData("maximum").toInt();
		}
	
}

// ---------------------------------------------------------------------------

void SettingsView::savePointerEventSettings()
{
	mPointerEventAttributes.mPriority  = mPriority->contentWidgetData("selected").toInt();
	mPointerEventAttributes.mHeartBeat = mHeartBeat->contentWidgetData("text").toInt();
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mPointerEventAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mPointerEventAttributes.mRandomVariance = mRandomVar->contentWidgetData("maximum").toInt();
		}
	
}

// ---------------------------------------------------------------------------

void SettingsView::saveMessageSettings()
{
	mMessageAttributes.mPriority    = mPriority->contentWidgetData("selected").toInt();
	mMessageAttributes.mMessageType = mType->contentWidgetData("selected").toInt();
	mMessageAttributes.mDestination = mDestination->contentWidgetData("text").toString();
	int amount = mAmount->contentWidgetData("text").toInt();
	if( amount  >=0 && amount <= mAmount->contentWidgetData("maximum").toInt() )
		{
		mMessageAttributes.mAmount = mAmount->contentWidgetData("text").toInt();
		}
	else
		{
		mMessageAttributes.mAmount = mAmount->contentWidgetData("maximum").toInt();
		}
	mMessageAttributes.mLength = mLength->contentWidgetData("text").toInt();
	mMessageAttributes.mIdle = mIdle->contentWidgetData("text").toInt();
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mMessageAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mMessageAttributes.mRandomVariance =  mRandomVar->contentWidgetData("maximum").toInt();
		}

}

// ---------------------------------------------------------------------------

void SettingsView::saveApplicationsSettings()
{
	mApplicationsAttributes.mPriority  = mPriority->contentWidgetData("selected").toInt();
	int maxApps = mMaxParallelApps->contentWidgetData("text").toInt();
	if( maxApps >=0 && maxApps <= mMaxParallelApps->contentWidgetData("maximum").toInt() )
		{
		mApplicationsAttributes.mMaxOpen   = mMaxParallelApps->contentWidgetData("text").toInt();
		}
	else
		{
		mApplicationsAttributes.mMaxOpen   = mMaxParallelApps->contentWidgetData("maximum").toInt();
		}
	mApplicationsAttributes.mLaunchingInterval   = mLaunchingInterval->contentWidgetData("text").toInt();
	mApplicationsAttributes.mKeyPressType  = mKeyPressType->contentWidgetData("selected").toInt();
	mPointerEventAttributes.mHeartBeat = mHeartBeat->contentWidgetData("text").toInt();
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mApplicationsAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mApplicationsAttributes.mRandomVariance = mRandomVar->contentWidgetData("maximum").toInt();
		}
}

// ---------------------------------------------------------------------------

void SettingsView::savePhotoSettings()
{
	mPhotoCaptureAttributes.mPriority  = mPriority->contentWidgetData("selected").toInt();
	mPhotoCaptureAttributes.mIdle = mIdle->contentWidgetData("text").toInt();
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mPhotoCaptureAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mPhotoCaptureAttributes.mRandomVariance = mRandomVar->contentWidgetData("maximum").toInt();
		}
	
	// update only if there's more than one camera device available in the system, 
	// for use case of one camera available, the selection cannot be changed by user.
	if (mPhotoCaptureAttributes.mCameraCount > 1)
	{
	mPhotoCaptureAttributes.mCamera = mCamera->contentWidgetData("selected").toInt();
	}
}

// ---------------------------------------------------------------------------

void SettingsView::saveBtSettings()
{
	mBluetoothAttributes.mPriority  = mPriority->contentWidgetData("selected").toInt();
	mBluetoothAttributes.mIdle = mIdle->contentWidgetData("text").toInt();
	int randVar = mRandomVar->contentWidgetData("text").toInt();
	if( randVar >=0 && randVar <= mRandomVar->contentWidgetData("maximum").toInt() )
		{
		mBluetoothAttributes.mRandomVariance = mRandomVar->contentWidgetData("text").toInt();
		}
	else
		{
		mBluetoothAttributes.mRandomVariance =  mRandomVar->contentWidgetData("maximum").toInt();
		}
	
}

// ---------------------------------------------------------------------------

void SettingsView::cancelled()
{
	connect(this, 
		SIGNAL(loadCanclled()), 
		&mEngineWrapper, 
		SLOT(loadSettingsCanclled()));
	emit loadCanclled();
	
    mMainWindow.removeView(this);
    mMainWindow.setCurrentView(&mMainView, true);
    deleteLater();
}

// ---------------------------------------------------------------------------

void SettingsView::okExit()
{
	saveSettings();
	setLoadAttributes();
    connect(this, 
			SIGNAL(loadCompleted(TLoadGenCommandIds)), 
			&mEngineWrapper, 
			SLOT(loadAddedOrEdited(TLoadGenCommandIds)));
	emit loadCompleted(mCmdId);
			
    mMainWindow.removeView(this);
    mMainWindow.setCurrentView(&mMainView, true);
    deleteLater();
}

// ---------------------------------------------------------------------------

void SettingsView::setLoadAttributes()
{
	switch (mCmdId) {
		case ELoadGenCmdNewLoadCPULoad: {
			mEngineWrapper.setCpuLoadAttributes(mCPULoadAttributes);
			break;
		}
		case ELoadGenCmdNewLoadEatMemory: {
			mEngineWrapper.setMemoryEatAttributes(mMemoryEatAttributes);
			break;
		}
		case ELoadGenCmdNewLoadPhoneCall: {
			mEngineWrapper.setPhoneCallAttributes(mPhoneCallAttributes);
			break;
		}
		case ELoadGenCmdNewLoadNetConn: {
			mEngineWrapper.setNetConnAttributes(mNetConnAttributes);
			break;
		}
		case ELoadGenCmdNewLoadKeyPress: {
			mEngineWrapper.setKeyPressAttributes(mKeyPressAttributes);
			break;
		}
		case ELoadGenCmdNewLoadPointerEvent: {
			mEngineWrapper.setPointerEventAttributes(mPointerEventAttributes);
			break;
		}
		case ELoadGenCmdNewLoadMessages: {
			mEngineWrapper.setMessageAttributes(mMessageAttributes);
			break;
		}
		case ELoadGenCmdNewLoadApplications: {
			mEngineWrapper.setApplicationsAttributes(mApplicationsAttributes);
			break;
		}
		case ELoadGenCmdNewLoadPhotoCaptures: {
			mEngineWrapper.setPhotoCaptureAttributes(mPhotoCaptureAttributes); 
			break;		
		}
		case ELoadGenCmdNewLoadBluetooth: {
			mEngineWrapper.setBluetoothAttributes(mBluetoothAttributes);
			break;
		}
		default: {
			break;
		}
	}
}

// ---------------------------------------------------------------------------

void SettingsView::dataItemDisplayed(const QModelIndex &index)
{
	HbDataFormViewItem *item = static_cast<HbDataFormViewItem*>(mSettingForm->itemByIndex(index)); 
	HbWidget *contentWidget = static_cast<HbWidget*>(item->dataItemContentWidget());
	switch(mCmdId){
		case ELoadGenCmdNewLoadCPULoad: {
			if (index.row() == 2) { // Cpu load type selection is in this row: periodic vs. continuous
				mModes = static_cast<HbRadioButtonList*>(contentWidget);
				connect(mModes , SIGNAL(itemSelected(int)), this, SLOT(selectionChanged(int)));
			}
			break;
		}
		case ELoadGenCmdNewLoadEatMemory: {
				// Memory eat type is in this row: mem to eat/ mem to be left vs. alternate min/max
				if (index.row() == 2) { 
				mTypes = static_cast<HbRadioButtonList*>(contentWidget );
				connect(mTypes ,SIGNAL(itemSelected(int)), this,SLOT(selectionChanged(int)));
				}
			break;
		}
	}
}

// ---------------------------------------------------------------------------

void SettingsView::selectionChanged(int index)
{
	switch (mCmdId) {
		case ELoadGenCmdNewLoadCPULoad: {
			if (index == ECpuLoadTypePeriodic) {
			// if periodic selected, add Length, Idle and Random variance to settings,
			// if they does not exist yet.
				if (mLength == NULL) {
					mLength = mModel->insertDataFormItem(3, 
														 HbDataFormModelItem::TextItem, 
														 QString("Length (ms)"), 
														 mLoadSettings);
					mLength->setContentWidgetData("maximum" , 999999999);
					mLength->setContentWidgetData("minimum", 0);
					mLength->setContentWidgetData(QString("text"), mCPULoadAttributes.mLength);
					}
				if (mIdle == NULL) {
					// DataFormItem for load length selection
					mIdle = mModel->insertDataFormItem(4,
					                                   HbDataFormModelItem::TextItem, 
													   QString("Idle (ms)"), 
													   mLoadSettings);
					mIdle->setContentWidgetData("maximum" , 999999999);
					mIdle->setContentWidgetData("minimum", 0);			
					mIdle->setContentWidgetData(QString("text"), mCPULoadAttributes.mIdle);
					}
				if (mRandomVar == NULL) {
					mRandomVar = mModel->insertDataFormItem(5,
					                                        HbDataFormModelItem::TextItem,  
														    QString("Random variance (%)"), 
														    mLoadSettings);
					mRandomVar->setContentWidgetData("maximum" , 100);
					mRandomVar->setContentWidgetData("minimum", 0);	
					mRandomVar->setContentWidgetData(QString("text"), mCPULoadAttributes.mRandomVariance);
				}
			}
			else if (index == ECpuLoadTypeContinuous) {
				// if continuous selected, remove existing Length, Idle and Random variance from settings view:
				// if they does not exist do nothing.
				if (mLength != NULL) {
					mModel->removeItem(mLength);
					mLength = NULL;
				}
				if (mIdle != NULL) {
					mModel->removeItem(mIdle);
					mIdle = NULL;
				}
				if (mRandomVar != NULL) {
					mModel->removeItem(mRandomVar);
					mRandomVar = NULL;
				}	
			}
			break;
		}
		case ELoadGenCmdNewLoadEatMemory: {
			if (index == EMemoryEatTypeWavy) {
				// remove one item, if exists. 
				if (mAmount != NULL) {
					mModel->removeItem(mAmount);
					mAmount = NULL;
				}
				// insert two new item rows, if not exists.
				if (mMinAmountToBeLeft == NULL) {
					mMinAmountToBeLeft	= mModel->insertDataFormItem(3, HbDataFormModelItem::TextItem, 
													  QString("Min to be left (B)"), 
													  mLoadSettings);
					mMinAmountToBeLeft->setContentWidgetData("maximum" , 99999999999999);
					mMinAmountToBeLeft->setContentWidgetData("minimum", 0);
					mMinAmountToBeLeft->setContentWidgetData( QString("text"),mMemoryEatAttributes.mRandomMin );
				}
				if (mMaxAmountToBeLeft == NULL) {
					mMaxAmountToBeLeft	= mModel->insertDataFormItem(4, HbDataFormModelItem::TextItem, 
													  QString("Max to be left (B)"), 
													  mLoadSettings);
					mMaxAmountToBeLeft->setContentWidgetData("maximum" , 99999999999999);
					mMaxAmountToBeLeft->setContentWidgetData("minimum", 0);				
					mMaxAmountToBeLeft->setContentWidgetData( QString("text"),mMemoryEatAttributes.mRandomMax );
				}
			}
			else if (index == EMemoryEatTypeMemoryToEat || index == EMemoryEatTypeMemoryToBeLeft) {
				// remove two item rows, if exists.
				if (mMinAmountToBeLeft != NULL) {
					mModel->removeItem(mMinAmountToBeLeft);
					mMinAmountToBeLeft = NULL;
				}
				if (mMaxAmountToBeLeft != NULL) {
					mModel->removeItem(mMaxAmountToBeLeft);
					mMaxAmountToBeLeft = NULL;
				}
				// insert one item, if not exists.
				if (mAmount == NULL) {
					mAmount	= mModel->insertDataFormItem(3, HbDataFormModelItem::TextItem, 
													  QString("Amout(B)"), 
													  mLoadSettings);
					mAmount->setContentWidgetData("maximum" , 99999999999999);
					mAmount->setContentWidgetData("minimum", 0);
					mAmount->setContentWidgetData( QString("text"),mMemoryEatAttributes.mAmount );
				}				
			}
		}
		default: {
			break;
		}
	}
}

// ---------------------------------------------------------------------------
