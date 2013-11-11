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
* Description:  Notifier for changes in context. Required by CCalenContextImpl
*                on construction.
*
*/


#ifndef CALENCONTEXTCHANGEOBSERVER_H
#define CALENCONTEXTCHANGEOBSERVER_H

// INCLUDES

// CLASS DEFINITIONS
/**
 * This class is for observing the context, and the ContextChange function
 * gets called when the context is changed. An object of this type is required
 * for construction of CCalenContextImpl.
 */
class MCalenContextChangeObserver
    {
    public:  // New functions
        /**
         * Called by MCalenContext when the context gets changed.
         */
        virtual void ContextChanged() = 0;
    };

#endif // CALENCONTEXTCHANGEOBSERVER_H

// End of file
