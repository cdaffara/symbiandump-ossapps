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

#include <e32svr.h>
#include <profile.hrh>
#include <eunitmacros.h>
#include <eunitdecorators.h>
#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include "ut_cphoneringingtonecontroller.h"
#include "cphoneringingtonecontroller.h"
#include "tphonecmdparamringtone.h"
#include "cphoneringingtone.h"
#include "cphonecenrepproxy.h"

_LIT(KNokiatune, "Z:\\Data\\Sounds\\Digital\\Nokia tune.aac");



// custom comparison method for matching expected and items
TBool operator==( const CPhoneRingingTone& aData1, 
        const CPhoneRingingTone& aData2 )
    {
    return ( aData1.FileName() == aData2.FileName()&& 
             aData1.Volume() == aData2.Volume() &&
             aData1.RingingType() == aData2.RingingType()&&
             aData1.TtsToneToBePlayed() == aData2.TtsToneToBePlayed());
    }

// - Construction -----------------------------------------------------------

ut_cphoneringingtonecontroller* ut_cphoneringingtonecontroller::NewL()
    {
    ut_cphoneringingtonecontroller* self = ut_cphoneringingtonecontroller::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphoneringingtonecontroller* ut_cphoneringingtonecontroller::NewLC()
    {
    ut_cphoneringingtonecontroller* self = new( ELeave ) ut_cphoneringingtonecontroller();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneringingtonecontroller::~ut_cphoneringingtonecontroller()
    {
    CSmcMockContext::Release();
    }

ut_cphoneringingtonecontroller::ut_cphoneringingtonecontroller()
    {
    }

void ut_cphoneringingtonecontroller::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneringingtonecontroller::SetupL(  )
    {
    iMockContext->InitializeL();
    // need to be created here otherwise memory leak can be detected.
    iPhoneCenRepProxy = CPhoneCenRepProxy::Instance();
    iRingingtoneplayer = CPhoneRingingToneController::NewL();
    }
    

void ut_cphoneringingtonecontroller::Teardown(  )
    {
    delete CPhoneCenRepProxy::Instance();
    UserSvr::DllFreeTls(KUidCenRepProxySingleton.iUid);
    
    delete iRingingtoneplayer; 
    iRingingtoneplayer = NULL;
    iMockContext->Reset();
    }
    

void ut_cphoneringingtonecontroller::T_PlayRingingtoneRinging(  )
    {
    // Expected data
    TBuf<50> ringingtone(KNokiatune);
    _LIT8( KMethod, "CPhoneRingingtonePlayer::PlayProfileBasedTone" );
    CPhoneRingingTone* aAudioVideoRingingTone = CPhoneRingingTone::NewL(ringingtone);
    aAudioVideoRingingTone->SetRingingType(EProfileRingingTypeRinging);
    aAudioVideoRingingTone->SetTtsToneToBePlayed(EFalse);
    aAudioVideoRingingTone->SetVolume(5);
    iMockContext->ExpectCallL( KMethod() ).WithL(aAudioVideoRingingTone);
    
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(5);
    ringToneParam.SetRingingType(EProfileRingingTypeRinging);
    // Set the profile ring tone
    ringToneParam.SetRingTone( ringingtone );
    ringToneParam.SetType( EPhoneRingToneProfile );

    iRingingtoneplayer->PlayRingToneL( &ringToneParam );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    delete aAudioVideoRingingTone;
    
    }

void ut_cphoneringingtonecontroller::T_PlayRingingtoneSilence(  )
    {
    _LIT8( KMethod, "CPhoneRingingtonePlayer::PlaySilentTone" );

    iMockContext->ExpectCallL( KMethod() );
    
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(5);
    ringToneParam.SetRingingType(EProfileRingingTypeSilent);
    // Set the profile ring tone
    TBuf<50> ringingtone(KNokiatune);
    ringToneParam.SetRingTone( ringingtone );
    ringToneParam.SetType( EPhoneRingToneProfile );

    iRingingtoneplayer->PlayRingToneL( &ringToneParam );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphoneringingtonecontroller::T_PlayRingingtoneBeepOnce(  )
    {
    _LIT8( KMethod, "CPhoneRingingtonePlayer::PlayBeepOnce" );
    iMockContext->ExpectCallL( KMethod() ).WithL(5);    
    
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(5);
    ringToneParam.SetRingingType(EProfileRingingTypeBeepOnce);
    // Set the profile ring tone
    TBuf<50> ringingtone(KNokiatune);
    ringToneParam.SetRingTone( ringingtone );
    ringToneParam.SetType( EPhoneRingToneProfile );

    iRingingtoneplayer->PlayRingToneL( &ringToneParam );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphoneringingtonecontroller::T_PlayRingingtoneAscending(  )
    {
    TBuf<50> ringingtone(KNokiatune);
    _LIT8( KMethod, "CPhoneRingingtonePlayer::PlayProfileBasedTone" );
    CPhoneRingingTone* aAudioVideoRingingTone = CPhoneRingingTone::NewL(ringingtone);
    aAudioVideoRingingTone->SetRingingType(EProfileRingingTypeAscending);
    aAudioVideoRingingTone->SetTtsToneToBePlayed(EFalse);
    aAudioVideoRingingTone->SetVolume(5);
    iMockContext->ExpectCallL( KMethod() ).WithL(aAudioVideoRingingTone);
    
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(5);
    ringToneParam.SetRingingType(EProfileRingingTypeAscending);
    // Set the profile ring tone
    ringToneParam.SetRingTone( ringingtone );
    ringToneParam.SetType( EPhoneRingToneProfile );

    iRingingtoneplayer->PlayRingToneL( &ringToneParam );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    delete aAudioVideoRingingTone;
    }

void ut_cphoneringingtonecontroller::T_PlayRingingtoneRingingOnce(  )
    {
    TBuf<50> ringingtone(KNokiatune);
    _LIT8( KMethod, "CPhoneRingingtonePlayer::PlayProfileBasedTone" );
    CPhoneRingingTone* aAudioVideoRingingTone = CPhoneRingingTone::NewL(ringingtone);
    aAudioVideoRingingTone->SetRingingType(EProfileRingingTypeRingingOnce);
    aAudioVideoRingingTone->SetTtsToneToBePlayed(EFalse);
    aAudioVideoRingingTone->SetVolume(5);
    iMockContext->ExpectCallL( KMethod() ).WithL(aAudioVideoRingingTone);
    
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(5);
    ringToneParam.SetRingingType(EProfileRingingTypeRingingOnce);
    // Set the profile ring tone
    ringToneParam.SetRingTone( ringingtone );
    ringToneParam.SetType( EPhoneRingToneProfile );

    iRingingtoneplayer->PlayRingToneL( &ringToneParam );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    delete aAudioVideoRingingTone;
    }

void ut_cphoneringingtonecontroller::T_PlayRingingtoneTTS()
{
    // Expected data
    _LIT(KTextToSay, "Mr. Brownstone");
    TBuf<30> texttosay (KTextToSay);
    TBuf<50> ringingtone(KNokiatune);
    CPhoneRingingTone* aAudioVideoRingingTone = CPhoneRingingTone::NewL(ringingtone);
    aAudioVideoRingingTone->SetRingingType(EProfileRingingTypeRinging);
    aAudioVideoRingingTone->SetTtsToneToBePlayed(ETrue);
    aAudioVideoRingingTone->SetVolume(5);
    _LIT8( KMethod, "CPhoneTTSPlayer::NewL" );
    iMockContext->ExpectCallL( KMethod() );
    _LIT8( KMethod2, "CPhoneTTSPlayer::PlayTtsTone" );
    iMockContext->ExpectCallL( KMethod2() ).WithL(KTextToSay(), aAudioVideoRingingTone);   
    _LIT8( KMethod3, "CPhoneRingingtonePlayer::PlayProfileBasedTone" );
    iMockContext->ExpectCallL( KMethod3() ).WithL(aAudioVideoRingingTone);

    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(5);
    ringToneParam.SetRingingType(EProfileRingingTypeRinging);
    ringToneParam.SetTextToSay(texttosay);
    // Set the profile ring tone
    ringToneParam.SetRingTone( ringingtone);
    ringToneParam.SetType( EPhoneRingToneProfile );

    iRingingtoneplayer->PlayRingToneL( &ringToneParam );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    delete aAudioVideoRingingTone;
}

void ut_cphoneringingtonecontroller::T_StopRingingtone()
{
    _LIT8( KMethod, "CPhoneRingingtonePlayer::StopPlaying" );
    iMockContext->ExpectCallL( KMethod() );

    iRingingtoneplayer->StopPlaying();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
}
// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneringingtonecontroller,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "T_PlayRingingtoneRinging",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, T_PlayRingingtoneRinging, Teardown)
    
EUNIT_TEST(
    "T_PlayRingingtoneSilence",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, T_PlayRingingtoneSilence, Teardown)

EUNIT_TEST(
    "T_PlayRingingtoneBeepOnce",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, T_PlayRingingtoneBeepOnce, Teardown)
    
EUNIT_TEST(
    "T_PlayRingingtoneAscending",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, T_PlayRingingtoneAscending, Teardown)    
     
EUNIT_TEST(
    "T_PlayRingingtoneAscending",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, T_PlayRingingtoneRingingOnce, Teardown)   

EUNIT_TEST(
    "T_PlayRingingtoneTTS",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, T_PlayRingingtoneTTS, Teardown)
    
EUNIT_TEST(
    "T_StopRingingtone",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, T_StopRingingtone, Teardown)
EUNIT_END_TEST_TABLE
