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
* Description: Class takes care of CBA's.
*
*/


#ifndef CPHONEUICOMMANDMANAGER_H
#define CPHONEUICOMMANDMANAGER_H

//INCLUDES
#include <e32base.h>

//FORWARD
class MPhoneViewCommandHandle;
class MPhoneStateMachine;
class CPhoneState;

/**
 * Class takes care of ui commands, soft buttons and tool bar.
 *
 * @lib phoneuicontrol
 * @since S60 v10.1
 */
NONSHARABLE_CLASS( CPhoneUiCommandManager ) :  public CBase
    {
    public:
        
        /**
        * Creates new instance of CPhoneUiCommandManager
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: handle to the PhoneUIView
        * @return CPhoneUiCommandManager* object  
        */
        static CPhoneUiCommandManager* NewL(
                    CPhoneState* aState,
                    MPhoneViewCommandHandle& aViewCommandHandle,
                    MPhoneStateMachine& aStateMachine );
        /**
        * Destructor.
        */
        virtual ~CPhoneUiCommandManager();
        
    public:

        /**
        * Updates UI commands, push buttons and toolbar buttons.
        */
        void UpdateUiCommands();
        
    public:
        
        /**
        * Sets the soft reject status
        * @param aSoftRejectStatus, new status
        */
        void SetSoftRejectStatus( TBool aSoftRejectStatus );
        
        /**
        * Returns the soft reject status
        * @return Soft reject status
        */
        TBool SoftRejectStatus();
        
    private:

        /**
        * Sets soft reject flag
        */
        void SetSoftRejectDimmedFlag();
      
        /**
        * Sets button flags
        */
        void SetButtonFlags();
        
        /**
        * Sets touch buttons.
        */
        void SetTouchButtons();
        
        /**
        * Sets toolbar buttons.
        */
        void SetToolBarButtons();
      
        /**
        * Returns toolbar resource id.
        */
        TInt ToolBarResource();
        
        /**
        * Returns ringing call resource id.
        */
        TInt RingingCallResource();
        
        /**
        * Returns single ringing call resource id.
        */
        TInt SingleRingingCallResource();
        
        /**
        * Check is a emergency call ongoing
        */
        TBool IsEmergency();
        
    private:
    
        /**
        * C++ default constructor.
        */
        CPhoneUiCommandManager(
                CPhoneState* aState,
                MPhoneViewCommandHandle& aViewCommandHandle,
                MPhoneStateMachine& aStateMachine );
        
    private:
        
        // Not owned:
        CPhoneState* iState;
        MPhoneViewCommandHandle& iViewCommandHandle;
        MPhoneStateMachine& iStateMachine;
        
        // Soft reject status flag
        TBool iSoftRejectStatus;
    };


#endif // CPHONEUICOMMANDMANAGER_H
