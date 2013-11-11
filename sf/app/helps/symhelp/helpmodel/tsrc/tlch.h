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
// Element within Help Model module
// 
//

#ifndef __TLBX_H__
#define __TLBX_H__

#if !defined(__COECNTRL_H__)
#include <coecntrl.h>
#endif
#if !defined(__COECCNTX_H__)
#include <coeccntx.h>
#endif
#if !defined(__COEMAIN_H__)
#include <coemain.h>
#endif
#if !defined(__COEHELP_H__)
#include <coehelp.h>
#endif
#if !defined(__EIKAPPUI_H__)
#include <eikappui.h>
#endif
#if !defined(__EIKAPP_H__)
#include <eikapp.h>
#endif
#if !defined(__EIKDOC_H__)
#include <eikdoc.h>
#endif
#if !defined(__EIKENV_H__)
#include <eikenv.h>
#endif
#if !defined(__APGTASK_H__)
#include <apgtask.h>
#endif
#if !defined(__EIKHLBX_H__)
#include <techview/eikhlbx.h>
#endif
#if !defined(__EIKHLBM_H__)
#include <techview/eikhlbm.h>
#endif
#if !defined(__EIKHLBI_H__)
#include <techview/eikhlbi.h>
#endif
#if !defined(__EIKHLBV_H__)
#include <techview/eikhlbv.h>
#endif
#if !defined(__EIKLBI_H__)
#include <techview/eiklbi.h>
#endif
#if !defined(__EIKLBM_H__)
#include <techview/eiklbm.h>
#endif
#if !defined(__EIKLBX_H__)
#include <techview/eiklbx.h>
#endif
#if !defined(__EIKCLBD_H__)
#include <techview/eikclbd.h>
#endif
#if !defined(__EIKBTGPC_H__)
#include <techview/eikbtgpc.h>
#endif
#if !defined(__EIKTXLBM_H__)
#include <techview/eiktxlbm.h>
#endif
#if !defined(__HLPMODEL_H__)
#include "HLPMODEL.H"
#endif
#if !defined(__GULICON_H__)
#include <gulicon.h>
#endif
#include <techview/eikon.hrh>



const TUid KUidTLchApp={ 0x100052b7 };

class CTLchAppView : public CCoeControl
	{
public:
	void ConstructL(TRect aRect);
	void GetHelpContext(TCoeHelpContext& aContext) const;
	};


class CTLchAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CTLchAppUi();
public:  // from CCoeAppUi
	CArrayFix<TCoeHelpContext>* HelpContextL() const;
private: // from CEikAppUi
	void HandleCommandL(TInt aCommand);
private:
    CTLchAppView* iAppView;
    };

class CTLchDocument : public CEikDocument
	{
public:
	// construct/destruct
	CTLchDocument(CEikApplication& aApp);
private: // from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

class CTLchApplication : public CEikApplication
	{
private: // from CEikApplication - pure virutal
	CApaDocument* CreateDocumentL();
	// FROM CApaApplication - pure virutal	
	TUid AppDllUid() const;
	};
#endif
