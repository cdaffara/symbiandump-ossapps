// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definitions of elemenent names and transitions
// for the SMIL DTD.
// 
//

/**
 @file
*/

#ifndef __SMILDATA_H__
#define __SMILDATA_H__

// Constants for the initial and terminal states
const TInt KSMILDTDStartState = 0;
const TInt KSMILDTDEndState = -1;
const TInt KSMILDTDMinJump = 5; // Don't do binary chop below this size.  Not a critical value.

#define MAKESMILDTDSTATETYPE( from, to, str ) { from, to, _S(str), (sizeof(str)-1) }

//Literal definitions for element names
_LIT(KSMILDTDElta, "a");
_LIT(KSMILDTDEltanchor, "anchor");
_LIT(KSMILDTDEltanimation, "animation");
_LIT(KSMILDTDEltarea, "area");
_LIT(KSMILDTDEltaudio, "audio");
_LIT(KSMILDTDEltbody, "body");
_LIT(KSMILDTDElthead, "head");
_LIT(KSMILDTDEltimg, "img");
_LIT(KSMILDTDEltlayout, "layout");
_LIT(KSMILDTDEltpar, "par");
_LIT(KSMILDTDEltprefetch, "prefetch");
_LIT(KSMILDTDEltref, "ref");
_LIT(KSMILDTDEltregion, "region");
_LIT(KSMILDTDEltroot_layout, "root-layout");
_LIT(KSMILDTDEltseq, "seq");
_LIT(KSMILDTDEltsmil, "smil");
_LIT(KSMILDTDEltswitch, "switch");
_LIT(KSMILDTDElttext, "text");
_LIT(KSMILDTDElttextstream, "textstream");
_LIT(KSMILDTDEltvideo, "video");
_LIT(KSMILDTDEltmeta, "meta");
_LIT(KSMILDTDEltmetadata, "metadata");
_LIT(KSMILDTDEltDoc, "documentelement");
_LIT(KSMILDTDEltTrans, "transition");
// Child Validation States

// Array of allowed child element state transitions
// <!ELEMENT  seq (text|img|audio|video|animation|textstream|ref|switch|a|anchor|area|par|seq)*>
// Same for par
// Entries must be sorted by tag and then by from state
LOCAL_D const TSMILDTDChildStateType SMILDTDTimingChildStates[15] =
	{
		MAKESMILDTDSTATETYPE(0,    -1, ""),
		MAKESMILDTDSTATETYPE(0,     0, "a"),
		MAKESMILDTDSTATETYPE(0,     0, "anchor"),
		MAKESMILDTDSTATETYPE(0,     0, "animation"),
		MAKESMILDTDSTATETYPE(0,     0, "area"),
		MAKESMILDTDSTATETYPE(0,     0, "audio"),
		MAKESMILDTDSTATETYPE(0,     0, "img"),
		MAKESMILDTDSTATETYPE(0,     0, "par"),
		MAKESMILDTDSTATETYPE(0,     0, "prefetch"), 
		MAKESMILDTDSTATETYPE(0,     0, "ref"),
		MAKESMILDTDSTATETYPE(0,     0, "seq"),
		MAKESMILDTDSTATETYPE(0,     0, "switch"),
		MAKESMILDTDSTATETYPE(0,     0, "text"),
		MAKESMILDTDSTATETYPE(0,     0, "textstream"),
		MAKESMILDTDSTATETYPE(0,     0, "video")
	};

LOCAL_D const TInt KSMILDTDTimingChildStateTranCount = 15;


LOCAL_D const TSMILDTDChildStateType SMILDTDAChildStates[8] =
	{
		MAKESMILDTDSTATETYPE(0,    -1, ""),
		MAKESMILDTDSTATETYPE(0,     0, "animation"),
		MAKESMILDTDSTATETYPE(0,     0, "audio"),
		MAKESMILDTDSTATETYPE(0,     0, "img"),
		MAKESMILDTDSTATETYPE(0,     0, "ref"),
		MAKESMILDTDSTATETYPE(0,     0, "text"),
		MAKESMILDTDSTATETYPE(0,     0, "textstream"),
		MAKESMILDTDSTATETYPE(0,     0, "video")
	};

LOCAL_D const TInt KSMILDTDAChildStateTranCount = 8;


// Array of allowed child element state transitions
// <!ELEMENT  body (par|seq|text|img|audio|video|animation|textstream|ref|switch|a|anchor|area)*>
// Entries must be sorted by tag and then by from state
LOCAL_D const TSMILDTDChildStateType SMILDTDBodyChildStates[15] =
	{
		MAKESMILDTDSTATETYPE(0,    -1, ""),
		MAKESMILDTDSTATETYPE(0,     0, "a"),
		MAKESMILDTDSTATETYPE(0,     0, "anchor"),
		MAKESMILDTDSTATETYPE(0,     0, "area"),
		MAKESMILDTDSTATETYPE(0,     0, "animation"),
		MAKESMILDTDSTATETYPE(0,     0, "audio"),
		MAKESMILDTDSTATETYPE(0,     0, "img"),
		MAKESMILDTDSTATETYPE(0,     0, "par"),
		MAKESMILDTDSTATETYPE(0,     0, "prefetch"),
		MAKESMILDTDSTATETYPE(0,     0, "ref"),
		MAKESMILDTDSTATETYPE(0,     0, "seq"),
		MAKESMILDTDSTATETYPE(0,     0, "switch"),
		MAKESMILDTDSTATETYPE(0,     0, "text"),
		MAKESMILDTDSTATETYPE(0,     0, "textstream"),
		MAKESMILDTDSTATETYPE(0,     0, "video")
	};

LOCAL_D const TInt KSMILDTDBodyChildStateTranCount = 15;


// Array of allowed child element state transitions
// <!ELEMENT  head (layout|switch)?> PLUS we allow meta's anywhere!
// Entries must be sorted by tag and then by from state
LOCAL_D const TSMILDTDChildStateType SMILDTDHeadChildStates[10] =
	{
		MAKESMILDTDSTATETYPE(0,    -1, ""),
		MAKESMILDTDSTATETYPE(1,    -1, ""),
		MAKESMILDTDSTATETYPE(0,     1, "layout"),
		MAKESMILDTDSTATETYPE(0,     0, "meta"),
		MAKESMILDTDSTATETYPE(1,     1, "meta"),
		MAKESMILDTDSTATETYPE(0,     0, "metadata"),
		MAKESMILDTDSTATETYPE(1,     1, "metadata"),
		MAKESMILDTDSTATETYPE(0,     0, "transition"),
		MAKESMILDTDSTATETYPE(1,     1, "transition"),
		MAKESMILDTDSTATETYPE(0,     1, "switch")
	};

LOCAL_D const TInt KSMILDTDHeadChildStateTranCount = 10;


// Array of allowed child element state transitions
// <!ELEMENT  layout (root-layout?,(region)*)>
// Entries must be sorted by tag and then by from state
LOCAL_D const TSMILDTDChildStateType SMILDTDLayoutChildStates[5] =
	{
		MAKESMILDTDSTATETYPE(0,    -1, ""),
		MAKESMILDTDSTATETYPE(1,    -1, ""),
		MAKESMILDTDSTATETYPE(0,     1, "region"),
		MAKESMILDTDSTATETYPE(1,     1, "region"),
		MAKESMILDTDSTATETYPE(0,     1, "root-layout")
	};

LOCAL_D const TInt KSMILDTDLayoutChildStateTranCount = 5;


// Array of allowed child element state transitions
// <!ELEMENT MediaObject (area|anchor)*>
// Entries must be sorted by tag and then by from state
LOCAL_D const TSMILDTDChildStateType SMILDTDMediaChildStates[3] =
	{
	MAKESMILDTDSTATETYPE( 0, -1, "") ,
	MAKESMILDTDSTATETYPE( 0,  0, "anchor"),
	MAKESMILDTDSTATETYPE( 0,  0, "area")
	};

LOCAL_D const TInt KSMILDTDMediaChildStateTranCount = 3;

// Array of allowed child element state transitions
// <!ELEMENT  smil (head?,body?)>
// Entries must be sorted by tag and then by from state
LOCAL_D const TSMILDTDChildStateType SMILDTDSmilChildStates[6] =
	{
		MAKESMILDTDSTATETYPE(0,    -1, ""),
		MAKESMILDTDSTATETYPE(1,    -1, ""),
		MAKESMILDTDSTATETYPE(2,    -1, ""),
		MAKESMILDTDSTATETYPE(0,     2, "body"),
		MAKESMILDTDSTATETYPE(1,     2, "body"),
		MAKESMILDTDSTATETYPE(0,     1, "head")
	};

LOCAL_D const TInt KSMILDTDSmilChildStateTranCount = 6;

// Array of allowed child element state transitions
// <!ELEMENT switch ((text|img|audio|video|animation|textstream|ref|a|anchor|area)*|(par|seq)*|layout)>
// Entries must be sorted by tag and then by from state
LOCAL_D const TSMILDTDChildStateType SMILDTDSwitchChildStates[31] =
	{
		MAKESMILDTDSTATETYPE(0,    -1, ""),
		MAKESMILDTDSTATETYPE(1,    -1, ""),
		MAKESMILDTDSTATETYPE(2,    -1, ""),
		MAKESMILDTDSTATETYPE(3,    -1, ""),
		MAKESMILDTDSTATETYPE(0,     1, "a"),
		MAKESMILDTDSTATETYPE(1,     1, "a"),
		MAKESMILDTDSTATETYPE(0,     1, "anchor"),
		MAKESMILDTDSTATETYPE(1,     1, "anchor"),
		MAKESMILDTDSTATETYPE(0,     1, "animation"),
		MAKESMILDTDSTATETYPE(1,     1, "animation"),
		MAKESMILDTDSTATETYPE(0,     1, "area"),
		MAKESMILDTDSTATETYPE(1,     1, "area"),
		MAKESMILDTDSTATETYPE(0,     1, "audio"),
		MAKESMILDTDSTATETYPE(1,     1, "audio"),
		MAKESMILDTDSTATETYPE(0,     1, "img"),
		MAKESMILDTDSTATETYPE(1,     1, "img"),
		MAKESMILDTDSTATETYPE(0,     3, "layout"),
		MAKESMILDTDSTATETYPE(0,     2, "par"),
		MAKESMILDTDSTATETYPE(2,     2, "par"),
		MAKESMILDTDSTATETYPE(0,	 1, "prefetch"),
		MAKESMILDTDSTATETYPE(1,     1, "prefetch"),
		MAKESMILDTDSTATETYPE(0,     1, "ref"),
		MAKESMILDTDSTATETYPE(1,     1, "ref"),
		MAKESMILDTDSTATETYPE(0,     2, "seq"),
		MAKESMILDTDSTATETYPE(2,     2, "seq"),
		MAKESMILDTDSTATETYPE(0,     1, "text"),
		MAKESMILDTDSTATETYPE(1,     1, "text"),
		MAKESMILDTDSTATETYPE(0,     1, "textstream"),
		MAKESMILDTDSTATETYPE(1,     1, "textstream"),
		MAKESMILDTDSTATETYPE(0,     1, "video"),
		MAKESMILDTDSTATETYPE(1,     1, "video")
	};

LOCAL_D const TInt KSMILDTDSwitchChildStateTranCount = 31;

#endif // __SMILDATA_H__
