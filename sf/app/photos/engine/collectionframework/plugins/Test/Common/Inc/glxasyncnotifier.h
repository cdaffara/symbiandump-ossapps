/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Album collection plugin definition
*
*/



#ifndef C_GLXASYNCNOTIFIER_H
#define C_GLXASYNCNOTIFIER_H

// INCLUDES

#include <e32cmn.h>
#include <mpxcollectionplugin.h>

// FORWARD DECLARATIONS

class CGlxCollectionPlugin;
class CGlxAsyncNotifier;

// CLASS DECLARATION

/**
* Glx Asynchronous Notifier
* This class is used by a plugin when it needs to call a callback in an Asynchronous manor. 
*/

class CGlxAsyncNotifier : public CBase
	{
public:
	enum TCallType { EOpen, EMedia, EFindAll };					// Different call type. Currently only used in OpenL, MediaL and FindAllL

public:
    /**
    * Two-phased construction. This class should only be instatiated by CGlxCollectionPugin by calling one of the AsyncNotify methods.
    *
    * @param aObs Observer. It is this interface that will be called on completion
    * @param aMPXMedia. The MPXMedia data to pass to the observer. The pointer can be null
    *					in which case a dummy CMPXMedia is created (for example in the case of KErrNotSupported).
    *					Ownership is passed to this class
    * @param aError. Error to pass to the Observer.
    * @param aCallType, Identifies the type of call to complete. OpenL, MediaL, FindAllL...
    * @param aParent, Parent/owner gets notified when completion is made so that it can delete this class
    * @return object of construction.
    */
	static CGlxAsyncNotifier* NewL(MMPXCollectionPluginObserver* aObs, 
									CMPXMedia*& aMPXMedia,
									TInt aError,
									TCallType aCallType,
									MGlxAsyncCallBackObserver* aParent);
    /**
    * Destructor
    */
	~CGlxAsyncNotifier();
	
	/**
	* Cancels any outstanding Async request
	*/
	void CancelRequest();
	
	/**
	* Creates a dummy CMPXMedia
	* @return a pointer to the dummy CMPXMedia;
	*/
	static CMPXMedia* CreateDummyMPXMediaL();
	
private:
    /**
    * constructor
    */
	CGlxAsyncNotifier(MMPXCollectionPluginObserver* aObs, CMPXMedia*& aMPXMedia, TInt aError, TCallType aCallType, MGlxAsyncCallBackObserver* aParent);

    /**
    * Second phase constructor - ConstructL
    */
	void ConstructL();
	
	
	// static Async callback method. called from CAsyncCallBack
	static TInt StaticAsyncCallBack(TAny * aPtr);
	
	// Callback method called from static callback method. This in turn calls the observer
	TInt AsyncCallBack();
	
	// Cleans up
	void CleanUp();
	
private:
	MMPXCollectionPluginObserver*	iObs;						// Observer. The class that must be asynchronously notified
	CMPXMedia*&						iMPXMedia;					// Owner ship passed to this class
	TInt							iError;						// Error to pass to Client
	TCallType						iCallType;					// Open, Media, FindAll...
	MGlxAsyncCallBackObserver*		iParent;					// The Parent/Owener
	CAsyncCallBack*					iAsyncCallBack;				// Class from the SDK to generate Async callbacks			
	};

#endif
