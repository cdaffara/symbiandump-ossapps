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
* Description:  LiveComm Videotelephony Plugin Class
*
*/

#ifndef CLCVTENGINE_H
#define CLCVTENGINE_H

#include <e32base.h>
#include <lcengine.h>

class CLcVtSession;
class MLcSessionObserver;
class MLcUiProvider;
class MLcSession;

class CLcVtEngine : public CLcEngine
    {
public:
    
    /**
    * Static constructor
    *
    * @param aParams ECom instantiation parameters
    * @return An initialized instance of this class.
    */
    static CLcVtEngine* NewL( TAny* aParams );
    
    ~CLcVtEngine();
    
public: // From CLcEngine
    
    MLcSession& Session();

    TBool IsFeatureSupported( TLcFeature aLcFeature );

private: 
    void ConstructL();
    CLcVtEngine();
    
private:
    
    CLcVtSession* iSession;
    
    };

#endif // CLCVTENGINE_H

// End of File
