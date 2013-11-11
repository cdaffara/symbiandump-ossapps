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
* Description:  Interface for Calendar customisation plugins
 *
*/


#ifndef CALENCUSTOMISATION_H
#define CALENCUSTOMISATION_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// CONSTANTS
const TUid KCalenCustomisationInterfaceUid = { 0x2002EA3B };

// FORWARD DECLARATIONS
class CalenView;
class MCalenServices;
class MCalenCommandHandler;
class HbWidget;
class QString;
class HbMenu;

/**
 * CCalenCustomisation is the base class for
 * creating customised calendar plugins.
 */
class CCalenCustomisation : public CBase
    {
    public:
        static CCalenCustomisation* CreateImplementationL( TUid aUid, MCalenServices* aServices );
        virtual ~CCalenCustomisation();

    public: 
        
        virtual HbWidget* InfobarL() = 0; 
		virtual MCalenCommandHandler* CommandHandlerL(TInt aCommand) = 0;
		virtual QString* InfobarTextL() = 0;
		virtual void CustomiseMenu(HbMenu* aHbMenu) = 0;
        

    private:
        /**
         * Instance identifier key. When instance of an implementation is
         * created by ECOM framework, the framework will assign a UID for
         * it. The UID is used in the destructor to notify framework that
         * this instance is being destroyed and resources can be released.
         * Plugin implementations should leave this untouched, as the inline
         * destructor and constructor method take care of ECom releasing.
         */
        TUid iDtor_ID_Key;
    };

inline CCalenCustomisation* CCalenCustomisation::CreateImplementationL( TUid aUid, MCalenServices* aServices )
    {
    TAny* ptr = REComSession::CreateImplementationL( aUid, _FOFF( CCalenCustomisation, iDtor_ID_Key ) , aServices);
    CCalenCustomisation* impl = static_cast<CCalenCustomisation*> (ptr);
    return impl;
    }

inline CCalenCustomisation::~CCalenCustomisation()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }



#endif  // CALENCUSTOMISATION_H
