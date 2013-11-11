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

#ifndef ENGINEWRAPPER_H
#define ENGINEWRAPPER_H

#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <engine.h>
#include "loadgen.hrh"

#include <QObject>

class HbMainWindow;
class HbAction;
class MainView; 
class SettingsView;
class CPULoadAttributes;
class MemoryEatAttributes;
class PhoneCallAttributes;
class NetConnAttributes;
class KeyPressAttributes;
class PointerEventAttributes;
class MessageAttributes;
class ApplicationsAttributes;
class PhotoCaptureAttributes;
class BluetoothAttributes;

/**
 * class that is used for communicating between Symbian and Qt code.
 */
class EngineWrapper : public QObject
 { 
	Q_OBJECT

public:
    
    /**
     * Constructor
     */
    EngineWrapper(HbMainWindow &mainWindow, MainView &mainView);
    
    /**
     * Destructor
     */
    ~EngineWrapper();
    
    /**
     * Initializes Engine Wrapper
     * @return true if engine was started successfully
     */
    bool init();
		
public slots:
    void StopLoadYesNoDialogClosed(HbAction *action);
	void loadAddedOrEdited(TLoadGenCommandIds cmdId);
	void loadSettingsCanclled();	

public:

	/* Functions that are called from UI: */
    
	/**
	* Start new load
	*/
	bool startNewLoad(int commandId);

	/**
	* Launch Performance Monitor application.
	*/
	void launchPerfMonApp();
	
	/**
	* Edit operation for load settings
	*/
	void loadEdit(int rowIndex);
	
	/**
	* CPU load attributes
	*/
	CPULoadAttributes getCpuLoadAttributes();
	
	/**
	* Memory eat attributes
	*/
	MemoryEatAttributes getMemoryEatAttributes();

	/**
	* Phone call attributes
	*/
	PhoneCallAttributes getPhoneCallAttributes();
	
	/**
	* Network connection attributes
	*/
	NetConnAttributes getNetConnAttributes();
	
	/**
	* Key presses attributes
	*/
	KeyPressAttributes getKeyPressAttributes();

	/**
	* Pointer event attributes
	*/
	PointerEventAttributes getPointerEventAttributes();
	
	/**
	* Message attributes
	*/
	MessageAttributes getMessageAttributes();
	
	/**
	* Applications attributes
	*/
	ApplicationsAttributes getApplicationsAttributes();
	
	/**
	* Photo capture attributes
	*/
	PhotoCaptureAttributes getPhotoCaptureAttributes();
	
	/**
	* BT attributes
	*/
	BluetoothAttributes getBluetoothAttributes();
	
	/**
	* Set attributes for CPU load
	*/
	void setCpuLoadAttributes(const CPULoadAttributes& attributes);
	
	/**
	* Set attributes for memory eat
	*/
	void setMemoryEatAttributes(const MemoryEatAttributes& attributes);

	/**
	* Set attributes for phone call
	*/	
	void setPhoneCallAttributes(const PhoneCallAttributes& attributes);

	/**
	* Set attributes for network connection
	*/		
	void setNetConnAttributes(const NetConnAttributes& attributes);
	
	/**
	* Set attributes for key presses
	*/
	void setKeyPressAttributes(const KeyPressAttributes& attributes);
	
	/**
	* Set pointer event for messages
	*/
	void setPointerEventAttributes(const PointerEventAttributes& attributes);

	/**
	* Set message attributes
	*/
	void setMessageAttributes(const MessageAttributes& attributes);
	
	/**
	* Set applications attributes
	*/
	void setApplicationsAttributes(const ApplicationsAttributes& attributes);
	
	/**
	* Set photo capture attributes
	*/
	void setPhotoCaptureAttributes(const PhotoCaptureAttributes& attributes);
	
	/**
	* Set BT attributes
	*/	
	void setBluetoothAttributes(const BluetoothAttributes& attributes);
	
	/**
	* Stop load
	*/
	bool stopLoad(bool stopAll);
	
	/**
	* Resume all loads
	*/ 
	void resumeAllLoadItems();

	/**
	* Suspend all loads
	*/ 	
	void suspendAllLoadItems();
	
	/**
	* Suspend or resume selected/highlighted item from load list.
	*/
	void suspendOrResumeSelectedOrHighlightedItems();
    
public:

    /* Functions that are called from engine: */
    
	/** 
     * uses Notifications class to show error message 
     */
    void ShowErrorMessage(const TDesC& aErrorMessage);

    /**
	* uses Notifications class to show user note
	*/
	void ShowNote(const TDesC& aNoteMessage);
	
	/**
	* Query item index
	* @return selected row in load list view
	*/
	TInt QueryCurrentItemIndex();
	
	/**
	* selected indexes
	*/
	const CArrayFix<TInt>*  QueryListSelectedIndexesOrCurrentItemL();

	/**
	* Open settings for load.
	*/
	void OpenLoadSettings(TInt loadType, bool editExistingLoad);
	
	void ReFreshView(CDesCArray* aTextArray);
	/**
	* Clear load list selection(s) in Ui
	*/
	void ClearSelection();
	
	/**
	* Set current index in load list
	*/
	void SetCurrentItemIndex(TInt aIndex);
	
public: 
	inline MainView& ViewReference() { return mMainView; };	
	inline HbMainWindow& WindowReference() { return mMainWindow; };
	
private:

	enum LoadTypes {
		Cpu,
		EatMemory,
		PhoneCalls,
		Messages,
		NWConnections,
		KeyPresses,
		Apps,
		Photos,
		BTAct,
		PointerEvents
	};
    
    
    /* LoadGen engine */
    CEngine* mEngine;
    
	MainView &mMainView;
	HbMainWindow &mMainWindow;
	bool mEditExistingLoad;
};

#endif //ENGINEWRAPPER_H
