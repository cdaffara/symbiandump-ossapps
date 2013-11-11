/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Notifier for changes in ecom registry
*
*/


#ifndef CALENECOMCHANGEOBSERVER_H
#define CALENECOMCHANGEOBSERVER_H

// INCLUDES

// CLASS DEFINITIONS
/**
 * This class is for observing the ECom registry, and the EComChange function
 * gets called when the registry is changed.
 */
class MCalenEComChangeObserver
    {
    public:  // New functions
        /**
         * Called by when the ecom registry gets changed.
         */
        virtual void EComChanged() = 0;
    };

#endif // CALENECOMCHANGEOBSERVER_H

// End of file
