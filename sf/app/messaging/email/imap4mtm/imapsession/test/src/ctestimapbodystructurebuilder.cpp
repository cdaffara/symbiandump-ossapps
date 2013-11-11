// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "ctestimapbodystructurebuilder.h"

#include <utf.h>

#include "cimapatom.h"
#include "cimapatomparser.h"
#include "cimapatomwalker.h"
#include "cimapbodystructurebuilder.h"
#include "cimapbodystructure.h"
#include "cimapfetchresponse.h"
#include "cimaplogger.h"

CTestImapBodyStructureBuilder::CTestImapBodyStructureBuilder()
	: iFetchResponse(NULL)
	, iBodyStructureBuilder(NULL)
	{}

CTestImapBodyStructureBuilder::~CTestImapBodyStructureBuilder()
	{
	delete iBodyStructureBuilder;
	delete iFetchResponse;
	}

void CTestImapBodyStructureBuilder::SetupL()
	{
	iFetchResponse = CImapFetchResponse::NewL();
	iBodyStructureBuilder = CImapBodyStructureBuilder::NewL(*iFetchResponse, KDefaultLog);
	}
	
void CTestImapBodyStructureBuilder::TearDownL()
	{
	delete iBodyStructureBuilder;
	iBodyStructureBuilder = NULL;
	
	delete iFetchResponse;
	iFetchResponse = NULL;
	}

void CTestImapBodyStructureBuilder::AssertDescriptorsEqual(const TDesC8& aFirst, const TDesC8& aSecond)
	{
	ASSERT_EQUALS(aFirst, aSecond);
	}

void CTestImapBodyStructureBuilder::TestSimpleTextMessageL()
	{
	INFO_PRINTF1(_L("TestSimpleTextMessageL"));
	
	_LIT8(KLine, "(TEXT PLAIN (name1 value1 name2 value2) ID \"this is a description\" encoding size lines");
	//_LIT8(KLine, "(TEXT PLAIN NIL ID \"this is a description\" encoding size lines)");
	//_LIT8(KLine, "(TEXT PLAIN NIL \"NIL\" NIL 123 5)");

	TBool wantMore = iBodyStructureBuilder->ProcessBlockL(KLine);
	ASSERT_FALSE(wantMore);

	CImapBodyStructure* bodyStructure = iFetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	AssertDescriptorsEqual(bodyStructure->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructure->SubType(), _L8("PLAIN"));
	AssertDescriptorsEqual(bodyStructure->BodyDescription(), _L8("this is a description"));

	// To Improve Coverage
	bodyStructure->ExtMD5();
	const TBufC8<1> yy;
	TRAP_IGNORE(bodyStructure->AppendExtLanguageListL(yy));
	bodyStructure->SetExtLocation(yy);
	
	}

void CTestImapBodyStructureBuilder::TestComplexTextMessageL()
	{
	INFO_PRINTF1(_L("TestComplexTextMessageL"));
	
	_LIT8(KLine1, "(TEXT PLAIN (name1 value1 name2 value2 name3 value3) ID {25}");
	_LIT8(KLit2, "first line\r\nsecond line\r\n");
	_LIT8(KLine3, "encoding size lines MD5 (disp (dn1 dv1 dn2 dv2)) lang place (ext1 ext2 (ext3 ext4) ext5)");
//	_LIT8(KLine3, "encoding size lines");
//	_LIT8(KLine3, "encoding size \"missing quote"); // and missing end bracket!
//	_LIT8(KLine3, "encoding size lines) Stuff at the end that we're not interested in");
	
	TBool wantMore = iBodyStructureBuilder->ProcessBlockL(KLine1);
	ASSERT_TRUE(wantMore);
	wantMore = iBodyStructureBuilder->ProcessBlockL(KLit2);
	ASSERT_TRUE(wantMore);
	wantMore = iBodyStructureBuilder->ProcessBlockL(KLine3);
	ASSERT_FALSE(wantMore);
	
	CImapBodyStructure* bodyStructure = iFetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	AssertDescriptorsEqual(bodyStructure->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructure->SubType(), _L8("PLAIN"));
	ASSERT_EQUALS(bodyStructure->ParameterList().Count(), 3);
 	ASSERT_EQUALS(bodyStructure->ParameterList()[0].iAttribute, _L8("name1"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[0].iValue, _L8("value1"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[1].iAttribute, _L8("name2"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[1].iValue, _L8("value2"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[2].iAttribute, _L8("name3"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[2].iValue, _L8("value3"));
	AssertDescriptorsEqual(bodyStructure->BodyId(), _L8("ID"));
	AssertDescriptorsEqual(bodyStructure->BodyDescription(), _L8("first line\r\nsecond line\r\n"));
	
	// extensions
	AssertDescriptorsEqual(bodyStructure->ExtDispositionName(), _L8("disp"));
	ASSERT_EQUALS(bodyStructure->ExtDispositionParameterList().Count(), 2);
	ASSERT_EQUALS(bodyStructure->ExtDispositionParameterList()[0].iAttribute, _L8("dn1"));
	ASSERT_EQUALS(bodyStructure->ExtDispositionParameterList()[0].iValue, _L8("dv1"));
	ASSERT_EQUALS(bodyStructure->ExtDispositionParameterList()[1].iAttribute, _L8("dn2"));
	ASSERT_EQUALS(bodyStructure->ExtDispositionParameterList()[1].iValue, _L8("dv2"));
	ASSERT_EQUALS(bodyStructure->ExtLanguageList().Count(), 1);
	ASSERT_EQUALS(bodyStructure->ExtLanguageList()[0], _L8("lang"));
	AssertDescriptorsEqual(bodyStructure->ExtLocation(), _L8("place"));	
	}

void CTestImapBodyStructureBuilder::TestRfc822MessageL()
	{
	INFO_PRINTF1(_L("TestRfc822MessageL"));
		
	_LIT8(KLine, "(\"MESSAGE\" \"RFC822\" (n1 v1 n2 v2) rfcID \"this is an rfc822 message\" rfcEncoding rfcSize (env-date env-subject ((from1name from1adl from1mbox from1host)(NIL NIL from2mbox from2host)) NIL NIL ((to-name to-adl to-mbox to-host)) NIL NIL in-reply-to msg-id) (TEXT PLAIN NIL subID \"sub description\" subEncoding subSize subLines) rfcLines");
	
	TBool wantMore = iBodyStructureBuilder->ProcessBlockL(KLine);
	ASSERT_FALSE(wantMore);

	// Check the root bodystructure
	CImapBodyStructure* bodyStructure = iFetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	AssertDescriptorsEqual(bodyStructure->Type(), _L8("MESSAGE"));
	AssertDescriptorsEqual(bodyStructure->SubType(), _L8("RFC822"));
	AssertDescriptorsEqual(bodyStructure->BodyDescription(), _L8("this is an rfc822 message"));
	AssertDescriptorsEqual(bodyStructure->BodyLines(), _L8("rfcLines"));
	AssertDescriptorsEqual(bodyStructure->BodyId(), _L8("rfcID"));
	AssertDescriptorsEqual(bodyStructure->BodyEncoding(), _L8("rfcEncoding"));
	AssertDescriptorsEqual(bodyStructure->BodySizeOctets(), _L8("rfcSize"));
	ASSERT_EQUALS(bodyStructure->ParameterList().Count(), 2);
 	ASSERT_EQUALS(bodyStructure->ParameterList()[0].iAttribute, _L8("n1"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[0].iValue, _L8("v1"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[1].iAttribute, _L8("n2"));
	ASSERT_EQUALS(bodyStructure->ParameterList()[1].iValue, _L8("v2"));
	
	// Check its envelope
	CImapEnvelope& envelope = bodyStructure->GetRfc822EnvelopeStructureL();
	AssertDescriptorsEqual(envelope.EnvDate(), _L8("env-date"));
	AssertDescriptorsEqual(envelope.EnvSubject(), _L8("env-subject"));
	ASSERT_EQUALS(envelope.EnvFrom().Count(), 2);
	CheckAddressL(envelope.EnvFrom()[0], _L16("\"from1name\" <from1mbox@from1host>"));
	CheckAddressL(envelope.EnvFrom()[1], _L16("from2mbox@from2host"));
	ASSERT_EQUALS(envelope.EnvSender().Count(), 0);
	ASSERT_EQUALS(envelope.EnvReplyTo().Count(), 0);
	ASSERT_EQUALS(envelope.EnvTo().Count(), 1);
	CheckAddressL(envelope.EnvTo()[0], _L16("\"to-name\" <to-mbox@to-host>"));
	ASSERT_EQUALS(envelope.EnvCc().Count(), 0);
	ASSERT_EQUALS(envelope.EnvBcc().Count(), 0);	
	AssertDescriptorsEqual(envelope.EnvInReplyTo(), _L8("in-reply-to"));
	AssertDescriptorsEqual(envelope.EnvMessageId(), _L8("msg-id"));
	
	// Check the embedded bodystructure
	ASSERT_EQUALS(bodyStructure->EmbeddedBodyStructureList().Count(), 1);
	CImapBodyStructure* bodyStructureEmbedded = bodyStructure->EmbeddedBodyStructureList()[0];
	
	AssertDescriptorsEqual(bodyStructureEmbedded->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructureEmbedded->SubType(), _L8("PLAIN"));
	ASSERT_EQUALS(bodyStructureEmbedded->ParameterList().Count(), 0);
 	AssertDescriptorsEqual(bodyStructureEmbedded->BodyId(), _L8("subID"));
	AssertDescriptorsEqual(bodyStructureEmbedded->BodyDescription(), _L8("sub description"));
	AssertDescriptorsEqual(bodyStructureEmbedded->BodyEncoding(), _L8("subEncoding"));
	AssertDescriptorsEqual(bodyStructureEmbedded->BodySizeOctets(), _L8("subSize"));
	AssertDescriptorsEqual(bodyStructureEmbedded->BodyLines(), _L8("subLines"));
	}
	
void CTestImapBodyStructureBuilder::CheckAddressL(const CImapEnvelope::TAddress& aAddress, const TDesC16& aExpectedAddressString)
	{
	HBufC16* addressString = aAddress.CreateAddressStringL();
	CleanupStack::PushL(addressString);
	
	ASSERT_EQUALS((TDesC16&)*addressString, aExpectedAddressString);
	
	CleanupStack::PopAndDestroy(addressString);
	}

void CTestImapBodyStructureBuilder::TestMultipartTextMessageNestLeftL()
	{
	INFO_PRINTF1(_L("TestMultipartTextMessageNestLeftL"));
	
//	_LIT8(KLine, "((\"TEXT\" \"PLAIN\" (n1 v1 n2 v2) ID description encoding size lines)(\"TEXT\" \"PLAIN\" (xn1 xv1 xn2 xv2) xID xDescription xEncoding xSize xLines) \"MIXED\")");
	_LIT8(KLine, "(((\"TEXT\" \"PLAIN\" (An1 Av1 An2 Av2) aID Adescription Aencoding Asize Alines)(\"TEXT\" \"PLAIN\" (Bn1 Bv1 Bn2 Bv2) bID Bdescription Bencoding Bsize Blines) \"gMIXED\" (gn1 gv1) (gdisp (gdn1 gdv1)) NIL gLondon (gExt1 gExt2 (gExt3 gExt4)))(\"TEXT\" \"PLAIN\" (xn1 xv1 xn2 xv2) xID xDescription xEncoding xSize xLines) \"hMIXED\")");
	
	// (((a)(b) gM gE)(x) hM)
	
	TBool wantMore = iBodyStructureBuilder->ProcessBlockL(KLine);
	ASSERT_FALSE(wantMore);
	
	// Root
	CImapBodyStructure* bodyStructurePart_h = iFetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructurePart_h);
	AssertDescriptorsEqual(bodyStructurePart_h->Type(), _L8("MULTIPART")); // << possibly leave this blank, and use iBodyStructureType enum instead.
	AssertDescriptorsEqual(bodyStructurePart_h->SubType(), _L8("hMIXED"));
	
	ASSERT_EQUALS(bodyStructurePart_h->EmbeddedBodyStructureList().Count(), 2);
	
	
	// MULTI on the left
	CImapBodyStructure* bodyStructurePart_h_g = bodyStructurePart_h->EmbeddedBodyStructureList()[0];
	
	AssertDescriptorsEqual(bodyStructurePart_h_g->Type(), _L8("MULTIPART")); // << possibly leave this blank, and use iBodyStructureType enum instead.
	AssertDescriptorsEqual(bodyStructurePart_h_g->SubType(), _L8("gMIXED"));
	
	// extensions
	ASSERT_EQUALS(bodyStructurePart_h_g->ParameterList().Count(), 1);
	ASSERT_EQUALS(bodyStructurePart_h_g->ParameterList()[0].iAttribute, _L8("gn1"));
	ASSERT_EQUALS(bodyStructurePart_h_g->ParameterList()[0].iValue, _L8("gv1"));
	AssertDescriptorsEqual(bodyStructurePart_h_g->ExtDispositionName(), _L8("gdisp"));
	ASSERT_EQUALS(bodyStructurePart_h_g->ExtDispositionParameterList().Count(), 1);
	ASSERT_EQUALS(bodyStructurePart_h_g->ExtDispositionParameterList()[0].iAttribute, _L8("gdn1"));
	ASSERT_EQUALS(bodyStructurePart_h_g->ExtDispositionParameterList()[0].iValue, _L8("gdv1"));
	ASSERT_EQUALS(bodyStructurePart_h_g->ExtLanguageList().Count(), 0);
	AssertDescriptorsEqual(bodyStructurePart_h_g->ExtLocation(), _L8("gLondon"));

	
	ASSERT_EQUALS(bodyStructurePart_h_g->EmbeddedBodyStructureList().Count(), 2);
	CImapBodyStructure* bodyStructurePart_h_g_a = bodyStructurePart_h_g->EmbeddedBodyStructureList()[0];
	CImapBodyStructure* bodyStructurePart_h_g_b = bodyStructurePart_h_g->EmbeddedBodyStructureList()[1];
		
	AssertDescriptorsEqual(bodyStructurePart_h_g_a->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_a->SubType(), _L8("PLAIN"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_a->BodyDescription(), _L8("Adescription"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_a->BodyLines(), _L8("Alines"));
	
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->SubType(), _L8("PLAIN"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->BodyDescription(), _L8("Bdescription"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->BodyLines(), _L8("Blines"));
	
	
	// PLAIN TEXT on the right
	CImapBodyStructure* bodyStructurePart_h_x  = bodyStructurePart_h->EmbeddedBodyStructureList()[1];
	
	AssertDescriptorsEqual(bodyStructurePart_h_x->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructurePart_h_x->SubType(), _L8("PLAIN"));
	AssertDescriptorsEqual(bodyStructurePart_h_x->BodyDescription(), _L8("xDescription"));
	AssertDescriptorsEqual(bodyStructurePart_h_x->BodyLines(), _L8("xLines"));
	}
	
void CTestImapBodyStructureBuilder::TestMultipartTextMessageNestRightL()
	{
	INFO_PRINTF1(_L("TestMultipartTextMessageNestRightL"));
	
//	_LIT8(KLine, "((\"TEXT\" \"PLAIN\" (n1 v1 n2 v2) ID description encoding size lines)(\"TEXT\" \"PLAIN\" (xn1 xv1 xn2 xv2) xID xDescription xEncoding xSize xLines) \"MIXED\")");
	_LIT8(KLine, "((\"TEXT\" \"PLAIN\" (An1 Av1 An2 Av2) aID Adescription Aencoding Asize Alines)((\"TEXT\" \"PLAIN\" (Bn1 Bv1 Bn2 Bv2) bID Bdescription Bencoding Bsize Blines)(\"TEXT\" \"PLAIN\" (xn1 xv1 xn2 xv2) xID xDescription xEncoding xSize xLines) \"gMIXED\") \"hMIXED\" (hn1 hv1) (hdisp (hdn1 hdv1)) (hENG hESP) NIL (hExt1 hExt2 (hExt3 hExt4)))");
	
	// ((a)((b)(x) gM) hM hE)
	
	TBool wantMore = iBodyStructureBuilder->ProcessBlockL(KLine);
	ASSERT_FALSE(wantMore);
	
	// Root
	CImapBodyStructure* bodyStructurePart_h = iFetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructurePart_h);
	AssertDescriptorsEqual(bodyStructurePart_h->Type(), _L8("MULTIPART")); // << possibly leave this blank, and use iBodyStructureType enum instead.
	AssertDescriptorsEqual(bodyStructurePart_h->SubType(), _L8("hMIXED"));
	
	ASSERT_EQUALS(bodyStructurePart_h->EmbeddedBodyStructureList().Count(), 2);

	// extensions
	ASSERT_EQUALS(bodyStructurePart_h->ParameterList().Count(), 1);
	ASSERT_EQUALS(bodyStructurePart_h->ParameterList()[0].iAttribute, _L8("hn1"));
	ASSERT_EQUALS(bodyStructurePart_h->ParameterList()[0].iValue, _L8("hv1"));
	AssertDescriptorsEqual(bodyStructurePart_h->ExtDispositionName(), _L8("hdisp"));
	ASSERT_EQUALS(bodyStructurePart_h->ExtDispositionParameterList().Count(), 1);
	ASSERT_EQUALS(bodyStructurePart_h->ExtDispositionParameterList()[0].iAttribute, _L8("hdn1"));
	ASSERT_EQUALS(bodyStructurePart_h->ExtDispositionParameterList()[0].iValue, _L8("hdv1"));
	ASSERT_EQUALS(bodyStructurePart_h->ExtLanguageList().Count(), 2);
	ASSERT_EQUALS(bodyStructurePart_h->ExtLanguageList()[0], _L8("hENG"));
	ASSERT_EQUALS(bodyStructurePart_h->ExtLanguageList()[1], _L8("hESP"));
	AssertDescriptorsEqual(bodyStructurePart_h->ExtLocation(), _L8(""));
	
	// PLAIN TEXT on the left
	CImapBodyStructure* bodyStructurePart_h_a  = bodyStructurePart_h->EmbeddedBodyStructureList()[0];
	
	AssertDescriptorsEqual(bodyStructurePart_h_a->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructurePart_h_a->SubType(), _L8("PLAIN"));
	AssertDescriptorsEqual(bodyStructurePart_h_a->BodyDescription(), _L8("Adescription"));
	AssertDescriptorsEqual(bodyStructurePart_h_a->BodyLines(), _L8("Alines"));
	
	// MULTI on the right
	CImapBodyStructure* bodyStructurePart_h_g = bodyStructurePart_h->EmbeddedBodyStructureList()[1];
	
	AssertDescriptorsEqual(bodyStructurePart_h_g->Type(), _L8("MULTIPART")); // << possibly leave this blank, and use iBodyStructureType enum instead.
	AssertDescriptorsEqual(bodyStructurePart_h_g->SubType(), _L8("gMIXED"));
	
	ASSERT_EQUALS(bodyStructurePart_h_g->EmbeddedBodyStructureList().Count(), 2);
	CImapBodyStructure* bodyStructurePart_h_g_b = bodyStructurePart_h_g->EmbeddedBodyStructureList()[0];
	CImapBodyStructure* bodyStructurePart_h_g_x = bodyStructurePart_h_g->EmbeddedBodyStructureList()[1];
	
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->SubType(), _L8("PLAIN"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->BodyDescription(), _L8("Bdescription"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_b->BodyLines(), _L8("Blines"));
	
	AssertDescriptorsEqual(bodyStructurePart_h_g_x->Type(), _L8("TEXT"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_x->SubType(), _L8("PLAIN"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_x->BodyDescription(), _L8("xDescription"));
	AssertDescriptorsEqual(bodyStructurePart_h_g_x->BodyLines(), _L8("xLines"));
	}

CTestSuite* CTestImapBodyStructureBuilder::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_TEST_STEP(TestSimpleTextMessageL);
	ADD_TEST_STEP(TestComplexTextMessageL);
	ADD_TEST_STEP(TestRfc822MessageL);
	ADD_TEST_STEP(TestMultipartTextMessageNestLeftL);
	ADD_TEST_STEP(TestMultipartTextMessageNestRightL);
	END_SUITE;
	}
