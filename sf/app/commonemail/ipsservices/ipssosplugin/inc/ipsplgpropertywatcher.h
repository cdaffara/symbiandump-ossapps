/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class CIpsPlgPropertyWatcher.
*
*/


#ifndef IPSPLGPROPERTYWATCHER_H_
#define IPSPLGPROPERTYWATCHER_H_

class CIpsPlgEventHandler;

/**
* CIpsPlgPropertyWatcher
*
*/
NONSHARABLE_CLASS ( CIpsPlgPropertyWatcher ) : 
    public CActive
    {
public:

    /**
    * Symbian OS NewL function
    * @return CIpsPlgPropertyWatcher*, self pointer
    */
    static CIpsPlgPropertyWatcher* NewL( 
        TInt aPriority,
        CIpsPlgEventHandler& aEventHandler );


    /**
    * destructor
    */
    virtual ~CIpsPlgPropertyWatcher();

private:

    /**
    * Constructor
    * @param aPriority active object priority
    */
    CIpsPlgPropertyWatcher( 
        TInt aPriority,
        CIpsPlgEventHandler& aEventHandler );

    /**
    * ConstructL
    * Symbian OS second phase constructor.
    */
    void ConstructL();

public:

    /**
     * 
    */
    void StartWatch();


protected:

    /**
    * DoCancel
    * From CActive
    */
    virtual void DoCancel();

    /**
    * RunL
    * From CActive
    */
    virtual void RunL();
    

protected: // Data
    
    CIpsPlgEventHandler& iEventHandler;
    
    RProperty iProperty;
    };


#endif /*IPSPLGPROPERTYWATCHER_H_*/
