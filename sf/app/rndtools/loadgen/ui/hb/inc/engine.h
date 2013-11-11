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


#ifndef LOADGEN_MODEL_H
#define LOADGEN_MODEL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <apgcli.h>

#include "loadgen_loadattributes.h"
#include "loadgen_loadbase.h"

// FORWARD DECLARATIONS
class EngineWrapper;
class MainView;
class CLoadGenGraphsContainer;
class CEikonEnv;
//class CLoadBase;


typedef CArrayFixSeg<CLoadBase*> CLoadItemList;

const TUint KMaxCPUs = 4;

// CLASS DECLARATIONS

class TLoadGenSettings
    {
public:
    };

class CEngine : public CActive
    {
private:
    enum TContainerDrawState
        {
        EDrawStateInvalid = -1,
        EDrawStateMain
        };

public:
    static CEngine* NewL(EngineWrapper *aEngineWrapper);
    ~CEngine();
    void ActivateEngineL();
    void DeActivateEngineL();
	void LaunchPerfMonL();
    void EditLoadL(TInt aIndex);
	void ExistingLoadEditedL();
	void ExistingLoadEditCancelled();
	
private:
    void RunL();
    void DoCancel();
        
private:
    CEngine();
    void ConstructL(EngineWrapper *aEngineWrapper);
    void LoadSettingsL();
    void AppendToLoadItemListL(CLoadBase* aItem);
    void DeleteFromLoadItemListL(TInt aIndex);
    void SuspendOrResumeFromLoadItemListL(TInt aIndex);
    void DeleteAllLoadItems();
    void RefreshViewL(TBool aClearSelection=ETrue);
    CDesCArray* ListOfAllAppsL();    
    
public:
    void StopAllLoadItemsL();
    void SuspendAllLoadItemsL();
    void ResumeAllLoadItemsL();
            
    TInt LoadItemCount() const;
    void StopSelectedOrHighlightedItemsL(const CArrayFix<TInt>* aSelectionIndexes);
    void SuspendOrResumeSelectedOrHighlightedItemsL();

    CDesCArray* GenerateListBoxItemTextArrayL();
    void StartNewLoadL(TInt aCommand);
	void DoStartNewLoadL(TInt aCommand);
    void SaveSettingsL();
    TInt LaunchSettingsDialogL();
	TCPULoadAttributes GetCPULoadAttributes();
	TMemoryEatAttributes GetMemoryEatAttributes();
	TPhoneCallAttributes GetPhoneCallAttributes();
	TNetConnAttributes GetNetConnAttributes();
	TKeyPressAttributes GetKeyPressAttributes();
	TMessageAttributes GetMessageAttributes();
	TApplicationsAttributes GetApplicationsAttributes();
	TPhotoCaptureAttributes GetPhotoCaptureAttributes();
	TBluetoothAttributes    GetBluetoothAttributes();
	TPointerEventAttributes GetPointerEventAttributes();
	void ChangeCPULoadAttributes(const TCPULoadAttributes& aAttributes); 
	void ChangeMemoryEatAttributes(const TMemoryEatAttributes& aAttributes);
	void ChangePhoneCallAttributes(const TPhoneCallAttributes& aAttributes);
	void ChangeNetConnAttributes(const TNetConnAttributes& aAttributes);
	void ChangeKeyPressAttributes(const TKeyPressAttributes& aAttributes);
	void ChangeMessageAttributes(const TMessageAttributes& aAttributes);
	void ChangeApplicationsAttributes(const TApplicationsAttributes& aAttributes);
	void ChangePhotoCaptureAttributes(const TPhotoCaptureAttributes& aAttributes);
	void ChangeBluetoothAttributes(const TBluetoothAttributes& aAttributes);
	void ChangePointerEventAttributes(const TPointerEventAttributes& aAttributes);
	
    inline TLoadGenSettings& Settings() { return iSettings; }
    inline CEikonEnv* EikonEnv() 		{ return iEnv; }
    inline RApaLsSession& LsSession() 	{ return iLs; }
    inline TBool LoadItemsExists() 		{ return iLoadItemList->Count() > 0; } 


private:
    RTimer                  iTimer;
    CEikonEnv*              iEnv;
    TLoadGenSettings        iSettings;
    RApaLsSession           iLs;
    CLoadItemList*          iLoadItemList;
    TInt                    iReferenceNumber;
    TCPULoadAttributes      iCpuLoadAttributes;
    TMemoryEatAttributes    iMemoryEatAttributes;
    TPhoneCallAttributes    iPhoneCallAttributes;
    TNetConnAttributes      iNetConnAttributes;
    TKeyPressAttributes     iKeyPressAttributes;
    TMessageAttributes      iMessageAttributes;
    TApplicationsAttributes	iApplicationsAttributes;
    TPhotoCaptureAttributes iPhotoCaptureAttributes;
    TBluetoothAttributes    iBluetoothAttributes;
    TPointerEventAttributes iPointerEventAttributes;
	EngineWrapper*          iEngineWrapper; // used for communicating between QT and Symbian
	TInt 					iCurrentItemIndex;
	TBool                   iEditExistingLoad;
	CLoadBase::TLoadState   iLoadStatusBeforeEdit;
    };
 

#endif

