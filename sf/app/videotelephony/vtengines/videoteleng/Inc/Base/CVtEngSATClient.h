/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SAT listener.
*
*/



#ifndef CVTENGSATCLIENT_H
#define CVTENGSATCLIENT_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <rsatrefresh.h>
#include <rsatsession.h>


// CLASS DECLARATION

/**
*  SAT listener.
*
*  @lib videoteleng.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CVtEngSATClient ) : public CBase, MSatRefreshObserver
    {
    public:  // Constructors and destructor.
        
        /**
        * Two-phased constructor.
        * @return instance of CVtEngSATClient
        */
        static CVtEngSATClient* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CVtEngSATClient();
        
    public: // Functions from base classes

        /**
        * From MSatRefreshObserver
        */
        TBool AllowRefresh(
            TSatRefreshType aType, const TSatRefreshFiles& aFiles );
        /**
        * From MSatRefreshObserver.
        */
        void Refresh( TSatRefreshType aType, const TSatRefreshFiles& aFiles );

    private:

        /**
        * C++ default constructor.
        */
        CVtEngSATClient();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
    
    private:    // Data
    
        // Session & Refresh for SAT listening.
        RSatSession iSatSession;
        RSatRefresh iSatRefresh;

    };

#endif      // CVTENGSATCLIENT_H
            
// End of File
