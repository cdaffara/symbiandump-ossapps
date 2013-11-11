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


#ifndef PERFMON_MODEL_H
#define PERFMON_MODEL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <apgcli.h>
#include <gdi.h>

#include "perfmon_engine.h"

// FORWARD DECLARATIONS
class CPerfMonValuesContainer;
class CPerfMonGraphsContainer;
class CPerfMonDataPopupContainer;

class CPerfMonModel : public CPerfMonEngine
	{
private:
    enum TContainerDrawState
    	{
    	EDrawStateInvalid = -1,
    	EDrawStateValues,
    	EDrawStateGraphs
    	};

public:
	static CPerfMonModel* NewL();
	void ActivateModelL();
	void DeActivateModelL();

private:
	void ConstructL();
	void SendDrawEventToContainersL();
	void HandleSettingsChangeL();
    
public:
    void SetValuesContainer(CPerfMonValuesContainer* aValuesContainer);
    void SetGraphsContainer(CPerfMonGraphsContainer* aGraphsContainer);
    TInt LaunchSettingsDialogL();
    TInt SampleEntryPosToSettingPos(TInt aSampleEntryPos);

    inline CPerfMonValuesContainer*     ValuesContainer()       { return iValuesContainer; }
    inline CPerfMonGraphsContainer*     GraphsContainer()       { return iGraphsContainer; }
    inline CPerfMonDataPopupContainer*  DataPopupContainer()    { return iDataPopupContainer; } 

private:
    CPerfMonValuesContainer*        iValuesContainer;
    CPerfMonGraphsContainer*        iGraphsContainer;
    CPerfMonDataPopupContainer*     iDataPopupContainer;
    TInt                            iDrawState;
    };
 

#endif
