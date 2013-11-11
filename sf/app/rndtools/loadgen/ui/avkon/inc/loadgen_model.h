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

// FORWARD DECLARATIONS
class CLoadGenMainContainer;
class CLoadGenGraphsContainer;
class CEikonEnv;
class CLoadBase;


typedef CArrayFixSeg<CLoadBase*> CLoadItemList;

const TUint KMaxCPUs = 4;

// CLASS DECLARATIONS

class TLoadGenSettings
    {
public:
    };

class CLoadGenModel : public CActive
    {
private:
    enum TContainerDrawState
        {
        EDrawStateInvalid = -1,
        EDrawStateMain
        };

public:
    static CLoadGenModel* NewL();
    ~CLoadGenModel();
    void ActivateModelL();
    void DeActivateModelL();

private:
    void RunL();
    void DoCancel();
        
private:
    CLoadGenModel();
    void ConstructL();
    void LoadSettingsL();
    void EditLoadL(CLoadBase* aItem);
    void DoStartNewLoadL(TInt aCommand);
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
    void ShowItemActionMenuL();
    void StopSelectedOrHighlightedItemsL();
    void SuspendOrResumeSelectedOrHighlightedItemsL();

    CDesCArray* GenerateListBoxItemTextArrayL();
    void StartNewLoadL(TInt aCommand);
    void SaveSettingsL();
    void SetMainContainer(CLoadGenMainContainer* aContainer);
    TInt LaunchSettingsDialogL();
    inline TLoadGenSettings& Settings() { return iSettings; }
    inline CEikonEnv* EikonEnv() { return iEnv; }
    inline RApaLsSession& LsSession() { return iLs; }
    inline CLoadGenMainContainer* MainContainer() { return iMainContainer; }
    inline TBool LoadItemsExists() { return iLoadItemList->Count() > 0; } 


private:
    RTimer                          iTimer;
    CLoadGenMainContainer*          iMainContainer;
    CEikonEnv*                      iEnv;
    TLoadGenSettings                iSettings;
    RApaLsSession                   iLs;
    TInt                            iDrawState;
    CLoadItemList*                  iLoadItemList;
    TInt                            iReferenceNumber;
    TCPULoadAttributes              iCpuLoadAttributes;
    TMemoryEatAttributes            iMemoryEatAttributes;
    TPhoneCallAttributes            iPhoneCallAttributes;
    TNetConnAttributes              iNetConnAttributes;
    TKeyPressAttributes             iKeyPressAttributes;
    TMessageAttributes              iMessageAttributes;
    TApplicationsAttributes			iApplicationsAttributes;
    TPhotoCaptureAttributes         iPhotoCaptureAttributes;
    TBluetoothAttributes            iBluetoothAttributes;
    TPointerEventAttributes         iPointerEventAttributes;
    static TInt64                   iRandomNumberSeed;
    };
 

#endif
