/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef __UT_CPHONESTATE_H__
#define __UT_CPHONESTATE_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS


// CLASS DEFINITION
class CSmcMockContext;
class CPhoneStateMachineGSM;
class CPhoneViewController;
class CPhoneMainResourceResolverMock;
class CPEEngineInfo;
class MPhoneViewCommandHandle;
class TPhoneCommandParam;
class CPhoneCustomizationVoip;
class cphonestate_tester;
 

TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 );
 
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonestate )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonestate* NewL();
        static ut_cphonestate* NewLC();
        ~ut_cphonestate();

    private: // Constructors

        ut_cphonestate();
        void ConstructL();

    private: // New methods

         void SetupL();
         
        
         void Teardown();
        
         void T_ConstructionL();
         void T_DisplayCallTerminationNoteL();
         //key event tests
         void T_HandleKeyEventRightArrowL();
         void T_HandleKeyEventUpArrowNENotVisibleL();
         void T_HandleKeyEventDownArrowNENotVisibleL();
         
         //HandleDtmfKeyTone tests
         void T_HandleDtmfKeyToneStarkeyL();
         void T_HandleDtmfKeyTone1KeyL();

         // other
         void T_HandleAudioOutputChangedL();
         
         void T_HandlePhoneFocusLostEventL();
         
         void T_StartShowSecurityNoteL();
         void T_HandleDtmfKeyToneL_ModeChange();
         
         void T_HandleDisconnectingL();
         
         void T_HandleBTActivatedL();
         
         void T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysTRUE_L();
         void T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysFALSE_L();
         void T_OnlySideVolumeKeySupported_VolumeKeysTRUEandSideVolumeKeysFALSE_L();
         
         void T_DisplayCallSetup_SetNEVisibilityFalseL();
         void T_DisplayCallSetup_CheckIfNEUsedBeforeSettingVisibilityFalseL();
         
         /* old voip related test cases
         void SetupWithVoipL();
         void T_HandleCommandL_CallsPhoneCustomization_L();
         void T_HandlePhoneEngineMessage_EPEMessageCallSecureStatusChanged_L();
         void T_HandlePhoneEngineMessage_CallsPhoneCustomization_L();
         void T_ShowNumberBusyNoteL_CallsCustomizeBusyNoteText_L();
         void T_PhoneNumberFromEntryLC();
         void T_UpdateRemoteInfoDataL();
         void T_UpdateSingleActiveCallL_VoIPcallL();*/
         
    private: // Data


        EUNIT_DECLARE_TEST_TABLE;
        
        MPhoneViewCommandHandle* iViewCmdHandleMock;

        CPhoneStateMachineGSM* iStateMachineMock;
        
        CSmcMockContext* iMockContext;
                
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        
        CPEEngineInfo* iEngineInfoMock;
        
        CPhoneCustomizationVoip* iCustomizationMock;
        
        cphonestate_tester* iPhoneState;
  

    };

#endif      //  __UT_CPHONESTATE_H__
