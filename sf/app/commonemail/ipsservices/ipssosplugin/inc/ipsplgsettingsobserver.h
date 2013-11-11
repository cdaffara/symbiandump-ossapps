/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Email settings observer class
*
*/

#ifndef IPSPLGSETTINGSOBSERVER_H
#define IPSPLGSETTINGSOBSERVER_H

//  INCLUDES


// FORWARD DECLARATIONS
class CRepository;
class MFSMailEventObserver;
class MIpsPlgSettingsObserverCallback;

const TUid KCRUidExtendedSettings   = {0x102071F1};

// CLASS DECLARATION

/**
* MIpsPlgSettingsObserverCallback
*/
class MIpsPlgSettingsObserverCallback
    {
public:    
    virtual void SettingsChangedL( TFSMailMsgId aAccount )=0;
    };

/**
* CIpsPlgSettingsObserver
*/
NONSHARABLE_CLASS ( CIpsPlgSettingsObserver ) : public CActive
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    *
    */
    static CIpsPlgSettingsObserver* NewL( 
        TFSMailMsgId aAccount,
        CRepository& aCenRep,
        MIpsPlgSettingsObserverCallback& aIPSCallback );
    
    /**
    * Destructor.
    */
    virtual ~CIpsPlgSettingsObserver();

    /**
    *
    */
    void SetKeyAndActivateL( 
        TUint32 aKey, 
        const RPointerArray<MFSMailEventObserver>& aObservers );
    
    /**
    *
    */
    void UnActivate( );    
    
    /**
    *
    */
    TFSMailMsgId MailboxId();

private:

    /**
    * Private C++ constructor.
    *
    */
    CIpsPlgSettingsObserver( 
        TFSMailMsgId aAccount,
        CRepository& aCenRep,
        MIpsPlgSettingsObserverCallback& aIPSCallback );

private:

    void ConstructL();

    /**
    * From CActive
    */
    void RunL();

    /**
    * From CActive
    */
    void DoCancel();
    
    /**
	* Function to handle error during running the operation
	* @since Series60 2.6
	* @return KErrNone, when error has been handled
	*/
	virtual TInt RunError( TInt aError );

    /**
    * 
    */	
	void Activate();
    
private: // Data

    CRepository&            iCenRep;        
    TUint32                 iKey;
    TFSMailMsgId            iAccount;
    
    // pointers not ovned
    RPointerArray<MFSMailEventObserver>  iFSObservers;
    MIpsPlgSettingsObserverCallback&     iIPSCallback;
        
    };

#endif      // IPSPLGSETTINGSOBSERVER_H
            
// End of File
