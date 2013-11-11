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



#ifndef PROFILER_GUI_APPUI_H
#define PROFILER_GUI_APPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include "profiler_gui_std.h"

// profiler engine inclusions
#include <piprofiler/ProfilerConfig.h>

// FORWARD DECLARATIONS
class CProfilerGuiModel;

// CLASS DECLARATIONS

class CProfilerGuiAppUi : public CAknViewAppUi
    {
public: // constructors and destructor
    void ConstructL();
    ~CProfilerGuiAppUi();
    
    // profiler specific 
	void StartProfilerL();
	void StopProfilerL();
private: // From MEikMenuObserver
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

private:
    void HandleCommandL(TInt aCommand);
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void HandleSystemEventL(const TWsEvent& aEvent);
    
    void SaveSettingsL();
	void LoadSettingsL();
	TInt RunConfQueryL( const TDesC* aOverrideText ); 
private: //Data
    CProfilerGuiModel*              iModel;
    };

#endif

// End of File
