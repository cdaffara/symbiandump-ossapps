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
* Description: Interface factory implementation class definition
*
*/

#ifndef EMAILINTERFACEFACTORY_H
#define EMAILINTERFACEFACTORY_H

#include <emailinterfacefactory.h>

using namespace EmailInterface;

/**
* Implements CEmailInterfaceFactory ECom interface
*
*/
NONSHARABLE_CLASS( CEmailInterfaceFactoryImpl ) : public CEmailInterfaceFactory
{
public: 

    /**
    * Creates interface factory
    * @return interface factory
    */
    static CEmailInterfaceFactoryImpl* NewL();
    
    // destructor
    ~CEmailInterfaceFactoryImpl();
    
    /** @see CEmailInterfaceFactory::InterfaceL */
    MEmailInterface* InterfaceL( const TInt aInterfaceId );
    
private:
    enum TEmailUidAppendRemoveMode
        {
        EEmailUidModeAppend,
        EEmailUidModeRemove,
        };
    
    /**
    * Registers/removes application UID to/from P&S so that the application
    * can be shutdown during Email IAD update. Update may fail if any
    * application is using some Email services during the update.
    * Application UID is removed from the list in destructor.
    * @param aMode Is the current process UID added or removed from the list
    */
    void AppendOrRemoveUidL( const TEmailUidAppendRemoveMode aMode );

private:    
    // c++ constructor and 2nd phase constructor
    CEmailInterfaceFactoryImpl();
    void ConstructL();
};

#endif // EMAILINTERFACEFACTORY_H

// End of file
