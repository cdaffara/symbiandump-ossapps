/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Bio control for Provisioning documents.
*
*/


#ifndef CWPBIOCONTROL_H
#define CWPBIOCONTROL_H

// INCLUDES
#include <e32cmn.h>
#include <CWPNameValue.h>
#include <hbsymbianvariant.h>
#include <hbdevicedialogsymbian.h>
#include <CWPBootstrap.h>
#include "ProvisioningUIDs.h"
#include "CpMessage.h"
#include "CWPSaver.h"
#include "CpQtSpView.h"

class CWPEngine;
class CWPNameValue;
class CWPPushMessage;
class CWPBootstarp;
class CpMessage;
class CpQtSp;

const TInt KMaxPinSize = 20;

// CLASS DECLARATION

/**
 * Bio control for OMA Provisioning messages.
 * @since 2.0
 */
 
 class CDummyObserver : public CBase, public MMsvSessionObserver
{
public:
        void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {};
};


class CWPBioControl: public CActive, MHbDeviceDialogObserver
{
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver Reference to the Bio control observer.
        * @param aSession Reference to Message Server session.
        * @param aId Id of the message.
        * @param aEditorOrViewerMode Enum for the mode, is it as editor or viewer.
        * @param aFileName The newly created object.
        * @param aFile file handle to provisioning content.
        */
        IMPORT_C static CWPBioControl* NewL( CMsvSession* aSession,TMsvId aId,CpQtSp* sp);
       ~CWPBioControl();
        RPointerArray<CWPNameValue>* CollectItemsLC() const;        
	    
	    void SaveSettingsL();
	    void DoSaveL();
	    void RestoreMsgL();
	    TInt getAuth();
          // from MHbDeviceDialogObserver
	    void DataReceived( CHbSymbianVariantMap& aData );
	    void DeviceDialogClosed(TInt aCompletionCode);
	    void SaveSettingStatus();
	    void saveDefaultSetting();
	    void postAuthetication();
	    void updateAuthetication();
	    void authenticationFailed();
	    void senderUnknownContinue();
	    void senderUnknownCancel();
    protected:  // from CActive
            void DoCancel();
            void RunL();
            
    private: // Constructors

        /**
        * Constructor is prohibited.
        * @param aObserver Reference to the Bio control observer.
        * @param aSession Reference to Message Server session.
        * @param aId Id of the message in Message Server.
        * @param aEditorOrViewerMode Enum for the mode, is it as editor or viewer.
        * @param aFileName The newly created object.
        * @param aFile file handle to provisioning content.
        */
        CWPBioControl( CMsvSession* aSession,TMsvId aId);
        void ConstructL(CMsvSession* aSession,TMsvId aId,CpQtSp* sp);
	
    private: 
        
        void AddItemsL();
        void CollateAndAddItemsL( RPointerArray<CWPNameValue>& aItems );        
        TBool AuthenticateL( CWPPushMessage& aMessage );
        static TInt Compare( const CWPNameValue& aImpl1,const CWPNameValue& aImpl2 );
        void SetCenrepKeyL(const TDesC8& aValue);
        void SaveMessageL();        
        static void Cleanup( TAny* aAny );
        TInt WaitUntilDeviceDialogClosed();        
    private: 
        CWPEngine*      iEngine;
        CWPPushMessage* iMessage;
        CDummyObserver* iObserver;
        CMsvSession* iSession;
        TMsvId iId;   
        TMsvEntry iEntry;
 		TInt ireturnkey;
        CHbDeviceDialogSymbian* iDevDialog;
        CWPBootstrap* ibootstrap;
        TInt itries;
        TInt iAuth;
        CActiveSchedulerWait* iWait;
        TBool iIsDisplayingDialog;
        TInt iCompletionCode;
        CpMessage* iMsg;
        CpQtSp* iSp;
    };



#endif // CWPBIOCONTROL_H

// End of File
