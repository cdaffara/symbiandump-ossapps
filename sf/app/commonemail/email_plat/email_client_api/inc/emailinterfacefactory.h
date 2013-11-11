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
* Description: ECom interface for Email Client API
*
*/


#ifndef CEMAILINTERFACEFACTORY_H
#define CEMAILINTERFACEFACTORY_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <memailclientapi.h>
#include <memailcontent.h>

namespace EmailInterface {
    
/**
 * ECom factory class for Email Client API.
 * @since S60 v5.0
 * 
 */
NONSHARABLE_CLASS( CEmailInterfaceFactory ) : public CBase
{
public:       
        
    /**
     * Contructor
     * @return factory for creating interface instances.
     */
    inline static CEmailInterfaceFactory* NewL();
    
    /**
    * destructor
    */
    inline ~CEmailInterfaceFactory();
    
    /**
    * Returns email interface pointer by id
    * @param aInterfaceId interface id, see emailclientapi.hrh for available ids
    * @return pointer to interface
    * @exception KErrNotFound if unkown interface id is given
    *
    * @code
    
     #include <emailinterfacefactory.h>
    
     using namespace EmailInterface;
     
     CEmailInterfaceFactory* factory = CEmailInterfaceFactory::NewL();
     CleanupStack::PushL( factory );
     MEmailInterface* ifPtr = factory->InterfaceL( KEmailClientApiInterface );
     MEmailClientApi* clientApi = static_cast<MEmailClientApi*>( ifPtr );
     CleanupReleasePushL( *clientApi );
     // <do something with clientApi...>
     CleanupStack::PopAndDestroy( 2 ); // clientApi and factory
     @endcode
    * 
    */
    virtual EmailInterface::MEmailInterface* InterfaceL(  
        const TInt aInterfaceId ) = 0;

private:
    
        // Unique instance identifier key
        TUid iDtor_ID_Key;
};

    
/**
 * Cleanup support for pointer arrays with MEmailInterface elements which are 
 * destroyed with Release() method.
 * @since S60 v5.0
   @code
       using namespace EmailInterface;
 
       RMailboxPtrArray mailboxes; // array of MEmailMailbox pointers
       // note: PushL can take any pointer array of elements with Release()
       // method
       CleanupResetAndRelease<MEmailMailbox>::PushL( mailboxes );
      // <here some code that might leave>
       mailClient->GetMailboxesL( mailboxes );
 
       // MEmailMailbox::Release() called for each array element and then
       // mailboxes.Reset() is called 
       CleanupStack::PopAndDestroy();  
   @endcode
 */
template<class T>
NONSHARABLE_CLASS( CleanupResetAndRelease )
{
public:
    /**
     * Cleanup for elements containing Release() method  
     */
    inline static void PushL( RPointerArray<T>& aItem );

    /**
     * Releases array elements and resets array.
     */
    inline static void ResetAndRelease( TAny* aPtr );  
};

#include "emailinterfacefactory.inl"

} // namespace EmailInterface

#endif // CEMAILINTERFACEFACTORY_H
