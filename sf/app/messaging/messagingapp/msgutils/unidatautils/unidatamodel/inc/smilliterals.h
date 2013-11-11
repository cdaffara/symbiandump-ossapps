/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef _SMILLITERALS_H
#define _SMILLITERALS_H

#include <e32def.h>

//tags
_LIT(KSmilTag,"smil");
_LIT(KHeadTag,"head");
_LIT(KBodyTag,"body");
_LIT(KLayoutTag,"layout");
_LIT(KRegionTag,"region");
_LIT(KRootLayoutTag,"root-layout");
_LIT(KSeqTag,"seq");
_LIT(KParTag,"par");
_LIT(KExclTag,"excl");
_LIT(KImageTag,"img");
_LIT(KAudioTag,"audio");
_LIT(KVideoTag,"video");
_LIT(KAnimationTag,"animation");
_LIT(KRefTag,"ref");
_LIT(KTextTag,"text");
_LIT(KTextStreamTag,"textstream");
_LIT(KSwitchTag,"switch");
_LIT(KATag,"a");
_LIT(KAreaTag,"area");
_LIT(KAnchorTag,"anchor");
_LIT(KTransitionTag,"transition");
_LIT(KPrefetchTag,"prefetch");
_LIT(KParamTag,"param");

//attributes
_LIT(KXmlnsAttr,"xmlns");
_LIT(KXmlnsAttr2,"xmlns:");
_LIT(KIdAttr,"id");
_LIT(KRegionNameAttr,"regionName");
_LIT(KRegionAttr,"region");
_LIT(KAltAttr,"alt");
_LIT(KDurAttr,"dur");
_LIT(KRepeatCountAttr,"repeatCount");
_LIT(KRepeatDurAttr,"repeatDur");
_LIT(KRepeatAttr,"repeat");
_LIT(KBeginAttr,"begin");
_LIT(KEndAttr,"end");
_LIT(KMinAttr,"min");
_LIT(KMaxAttr,"max");
_LIT(KEndsyncAttr,"endsync");
_LIT(KFillAttr,"fill");
_LIT(KSrcAttr,"src");
_LIT(KTopAttr,"top");
_LIT(KBottomAttr,"bottom");
_LIT(KLeftAttr,"left");
_LIT(KRightAttr,"right");
_LIT(KWidthAttr,"width");
_LIT(KHeightAttr,"height");
_LIT(KZIndexAttr,"z-index");
_LIT(KFitAttr,"fit");
_LIT(KBackgroundColorAttr,"backgroundColor");
_LIT(KBackgroundColor2Attr,"background-color");
_LIT(KShowBackgroundAttr,"showBackground");
_LIT(KHrefAttr,"href");
_LIT(KAccesskeyAttr,"accesskey");
_LIT(KActuateAttr,"actuate");
_LIT(KTargetAttr,"target");
_LIT(KTypeAttr,"type");
_LIT(KTabindexAttr,"tabindex");
_LIT(KSubtypeAttr,"subtype");
_LIT(KTransInAttr,"transIn");
_LIT(KTransOutAttr,"transOut");
_LIT(KFadeColorAttr,"fadeColor");
_LIT(KStartProgressAttr,"startProgress");
_LIT(KEndProgressAttr,"endProgress");
_LIT(KDirectionAttr,"direction");
_LIT(KRestartAttr,"restart");
_LIT(KSkipContentAttr,"skip-content");
_LIT(KShapeAttr,"shape");
_LIT(KCoordsAttr,"coords");
_LIT(KClipBeginAttr,"clipBegin");
_LIT(KClipBegin2Attr,"clip-begin");
_LIT(KClipEndAttr,"clipEnd");
_LIT(KClipEnd2Attr,"clip-end");
_LIT(KNameAttr,"name");
_LIT(KValueAttr,"value");
//_LIT(KShowAttr, "show");
//_LIT(KSourcePlaystateAttr, "sourceplaystate");

//system test attributes
_LIT(KSystemAttr,"system");
_LIT(KSystemRequiredAttr,"systemRequired");
_LIT(KSystemScreenSizeAttr,"systemScreenSize");
_LIT(KSystemScreenDepthAttr,"systemScreenDepth");
_LIT(KSystemCPUAttr, "systemCPU");
_LIT(KSystemOperatingSystemAttr, "systemOperatingSystem");
_LIT(KSystemComponentAttr, "systemComponent");
//_LIT(KSystemContentTypeAttr, "systemContentType");


//supported systemRequired URLs
static const TText* const KSystemReqValue [] = {
	//modules
	_S("http://www.w3.org/2001/SMIL20/AccessKeyTiming"),
	_S("http://www.w3.org/2001/SMIL20/BasicContentControl"),
	_S("http://www.w3.org/2001/SMIL20/BasicInlineTiming"),
	_S("http://www.w3.org/2001/SMIL20/BasicLayout"),
	_S("http://www.w3.org/2001/SMIL20/BasicLinking"),
	_S("http://www.w3.org/2001/SMIL20/BasicMedia"),
	_S("http://www.w3.org/2001/SMIL20/BasicTimeContainers"),
	_S("http://www.w3.org/2001/SMIL20/BasicTransitions"),
	_S("http://www.w3.org/2001/SMIL20/EventTiming"),
	_S("http://www.w3.org/2001/SMIL20/ExclTimeContainers"),
	_S("http://www.w3.org/2001/SMIL20/LinkingAttributes"),
	_S("http://www.w3.org/2001/SMIL20/MediaAccessibility"),
	_S("http://www.w3.org/2001/SMIL20/MediaClipping"),
	_S("http://www.w3.org/2001/SMIL20/MediaDescription"),
	_S("http://www.w3.org/2001/SMIL20/MediaParam"),
	_S("http://www.w3.org/2001/SMIL20/Metainformation"),
	_S("http://www.w3.org/2001/SMIL20/MinMaxTiming"),
	_S("http://www.w3.org/2001/SMIL20/MultiArcTiming"),
	_S("http://www.w3.org/2001/SMIL20/PrefetchControl"),
	_S("http://www.w3.org/2001/SMIL20/RepeatTiming"),
	_S("http://www.w3.org/2001/SMIL20/RestartTiming"),
	_S("http://www.w3.org/2001/SMIL20/SkipContentControl"),
	_S("http://www.w3.org/2001/SMIL20/Structure"),
	//other
	_S("http://www.w3.org/2001/SMIL20/NestedTimeContainers"),
	_S("http://www.w3.org/2001/SMIL20/DeprecatedFeatures"),

	_S("http://www.w3.org/2001/SMIL20/HostLanguage"),	
	_S("http://www.w3.org/2001/SMIL20/IntegrationSet"),	

	//3gpp
	_S("http://www.3gpp.org/SMIL20/PSS4/"),
	_S("http://www.3gpp.org/SMIL20/PSS5/"),
    _S("http://www.3gpp.org/SMIL20/PSS6/"),
	_S("http://www.3gpp.org/SMIL20/PSS4"),
	_S("http://www.3gpp.org/SMIL20/PSS5"),
    _S("http://www.3gpp.org/SMIL20/PSS6"),
	0 
	};


//attribute values
_LIT(KIndefiniteVal,"indefinite");
_LIT(KMediaVal,"media");
_LIT(KHiddenVal,"hidden");
_LIT(KFillVal,"fill");
_LIT(KSliceVal,"slice");
_LIT(KMeetVal,"meet");
_LIT(KScrollVal,"scroll");
_LIT(KNoneVal,"none");
_LIT(KTransparentVal,"transparent");
_LIT(KWhenActiveVal,"whenActive");
_LIT(KAlwaysVal,"always");
_LIT(KTransitionVal,"transition");
_LIT(KFreezeVal,"freeze");
_LIT(KHoldVal,"hold");
_LIT(KRemoveVal,"remove");
_LIT(KForwardVal,"forward");
_LIT(KReverseVal,"reverse");
_LIT(KFirstVal,"first");
_LIT(KLastVal,"last");
_LIT(KAllVal,"all");
_LIT(KWhenNotActiveVal,"whenNotActive");
_LIT(KNeverVal,"never");
_LIT(KTrueVal,"true");
_LIT(KFalseVal,"false");
_LIT(KCircleVal,"circle");
_LIT(KRectVal,"rect");
_LIT(KPolyVal,"poly");
_LIT(KOnLoadVal,"onLoad");
_LIT(KOnRequestVal,"onRequest");

_LIT(KContentTypeVal,"ContentType:");

_LIT(KSmil2NS,"http://www.w3.org/2001/SMIL20/Language");


// colors
static const TText* const KColorNames [] = {
	_S("black"),	
	_S("silver"),	
	_S("gray"),	
	_S("white"),	
	_S("maroon"),	
	_S("red"),		
	_S("purple"),	
	_S("fuchsia"),	
	_S("green"),	
	_S("lime"),	
	_S("olive"),	
	_S("yellow"),	
	_S("navy"),	
	_S("blue"),	
	_S("teal"),	
	_S("aqua"),	
	0
	};

//NOTE: TRgb constructor uses order 0x00bbggrr
static const TUint KColorValues [] = {
	0x000000,
	0xc0c0c0,
	0x808080,
	0xffffff,
	0x000080,
	0x0000ff,
	0x800080,
	0xff00ff,
	0x008000,
	0x00ff00,
	0x008080,
	0x00ffff,
	0x800000,
	0xff0000,
	0x808000,
	0xffff00,
	0
	};

#if defined (__WINS__) 
_LIT(KCPUVal, "x86");
#else
_LIT(KCPUVal, "arm");
#endif

_LIT(KOSVal, "symbian");


#endif
