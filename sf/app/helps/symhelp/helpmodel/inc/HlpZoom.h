// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __HLPZOOM_H__
#define __HLPZOOM_H__

// System includes
#include <e32std.h>

// Classes referenced
class MGraphicsDeviceMap;


//
// ----> MHlpZoomStateManager (header)
//
class MHlpZoomStateManager
/**
@internalComponent
@released
*/
	{
public: // Zoom state
	enum THlpZoomState
		{
		EHlpZoomStateSmall = -1,
		EHlpZoomStateNormal = 0,
		EHlpZoomStateLarge = 1
		};

public: // From MHlpZoomStateManager
	virtual void SetZoomState(THlpZoomState aState) = 0;
	virtual THlpZoomState ZoomState() const = 0;

public: // From MHlpZoomStateManager
	virtual MGraphicsDeviceMap& ZoomDeviceMap() const = 0;
	virtual TInt ZoomFactorValueForZoomState(THlpZoomState aState) const = 0;
	};



//
// ----> MHlpZoomStateObserver (header)
//
class MHlpZoomStateObserver
/**
@internalComponent
@released
*/
	{
public: // From MHlpZoomStateObserver
	virtual void HandleZoomStateChangedL(MHlpZoomStateManager::THlpZoomState aZoomState) = 0;
	};



//
// ----> MHlpZoomStateChangeNotifier (header)
//
class MHlpZoomStateChangeNotifier : public MHlpZoomStateManager
/**
@internalComponent
@released
*/
	{
public: // From MHlpZoomStateChangeNotifier
	virtual void RegisterForZoomStateChangeNotificationsL(MHlpZoomStateObserver& aObserver) = 0;
	virtual void RegisterForZoomStateChangeNotificationsCancel(MHlpZoomStateObserver& aObserver) = 0;
	};

#endif
