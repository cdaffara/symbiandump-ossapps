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
* Description:  PhoneUI panic reason codes
 *
*/


#ifndef __PHONEUI_PAN__
#define __PHONEUI_PAN__


enum TPhoneUiUtils
	{
    EPhoneUtilsIndexOutOfBounds = 1,
    EPhoneUtilsResourcePhoneappNotFound = 2,
    EPhoneUtilsCouldNotCreateSingleton = 3,
    EPhoneUtilsBufferOverflow = 4,
    EPhoneUtilsInvariant = 5,
    EPhoneUtilsParameterNotInitialized = 6,
    EPhoneUtilsNoCallBack = 7,
    EPhoneUtilsInvalidArgument = 8
    };

enum TPhoneUiView
	{
	EPhoneViewCaseNotHandled = 20,
	EPhoneViewGeneralError = 21,
	EPhoneViewNoImgConverterObserver = 22,
	EPhoneViewParameterNotInitialized = 23,
	EPhoneViewIndexOutOfBounds = 24,
	EPhoneViewInvariant = 25,
	EPhoneViewErrorInAddingResourceFile = 26
	};
	
enum TPhoneUiControl
	{
	EPhoneCtrlSingletonNotInitialized = 40,
	EPhoneCtrlFactoryLibraryNotFound = 41,
	EPhoneCtrlUnknownPanic = 42,
	EPhoneCtrlInvariant = 43,
	EPhoneCtrlParameterNotInitialized = 44,
	EPhoneCtrlCaseNotHandled = 45,
	EPhoneCtrlIndexOutOfBounds = 46,
	EPhoneCtrlPSKeyDefineFail = 47,
	};

enum TPhoneUi
	{
    EPhoneUiNoWindowGroup = 60
	};

enum TPhoneMediatorCenter
	{
	EPhoneMediatorCenterSingletonNotInitialized = 80,
	EPhoneMediatorCenterCouldNotCreateSingleton,
	EPhoneMediatorCenterParameterNotInitialized,
	EPhoneMediatorCenterInvalidCommand,
	EPhoneMediatorCenterInitializationPanic,
	EPhoneMediatorCenterRegistrationFailed
	};

inline void Panic( TInt aReason )
    {
	_LIT( componentName, "Common phone" );
    User::Panic( componentName, aReason );
    }

#endif  // __PHONEUI_PAN__


// End of File
