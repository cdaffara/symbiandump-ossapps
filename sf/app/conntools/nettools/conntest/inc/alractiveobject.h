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
* Description: conntest application
*
*/

#ifndef ALRACTIVEOBJECT_H
#define ALRACTIVEOBJECT_H

#include <e32base.h>
#include <comms-infras/cs_mobility_apiext.h>

namespace ESock { class CPreferredCarrierAvailable; }
namespace ESock { class CNewCarrierActive; }

class MUINotify;

class CALRActiveObject : public CActive
    {
public:  // Constructors and destructor
        /**
        * NewL is first phase of two-phased constructor.
        */
        static CALRActiveObject* NewL(
                    MUINotify& aConsole, 
                    MMobilityProtocolResp* aApplication);

        /**
        * Destructor.
        */
        ~CALRActiveObject();

    public: // from CActive
        
        void RunL();

        void DoCancel();

        TInt RunError( TInt aError );

    public: // New functions

        void OpenL( RCommsSubSession& aExtensionProvider );
    	void MigrateToPreferredCarrier();
    	void IgnorePreferredCarrier();
    	void NewCarrierAccepted();
    	void NewCarrierRejected();

    private:

        /** 
        * C++ default constructor.
        */
        CALRActiveObject( MUINotify& aConsole, MMobilityProtocolResp* aApplication );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(MUINotify& aConsole);

    private:    // Data
     //   MALRApplication&        iApplication;   // Backpointer
       // CConsoleBase&           iConsole;       // Pointer to console
       	MUINotify&              iConsole;
    	RCommsMobilityApiExt    iExtApi;
    	RBuf8                   iResponseBuf;
    	MMobilityProtocolResp*  iApplication;
    };

#endif // ALRACTIVEOBJECT_H

