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
* Description:  Observer for Input Language Setting changes.
*
*/


#ifndef MPHONELANGSETTINGOBSERVER_H 
#define MPHONELANGSETTINGOBSERVER_H

// CLASS DECLARATION

/**
*  Observer for Input Language Setting changes.
*/
class MPhoneLangSettingObserver
    {
    public:

        /**
        * Handle Input Language Setting changes.
        * @param aLanguage state of Input Language setting.
        */
        virtual void HandleInputLanguageSettingChange( TInt aLanguage ) = 0;
    };

#endif      // MPHONELANGSETTINGOBSERVER_H

// End of File
