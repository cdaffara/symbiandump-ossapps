//
/*
 * ==============================================================================
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
 * ==============================================================================
 */

#ifndef _WlanCtrlDCMOAdapter__
#define _WlanCtrlDCMOAdapter__

#include "dcmointerface.h"
#include "dcmoconst.h"

// An implementation of the CDCMOInterface definition
class CWLanCtrlDCMOAdapter : public CDCMOInterface
	{
public:
	// Standardised safe construction which leaves nothing the cleanup stack.
	static CWLanCtrlDCMOAdapter* NewL(TAny* aInitParams);

	// Destructor	
	~CWLanCtrlDCMOAdapter();

	// Implementation of CDCMOInterface
    virtual TDCMOStatus  GetDCMOPluginIntAttributeValueL( TDCMONode aId, TInt& aValue); 
    virtual TDCMOStatus  GetDCMOPluginStrAttributeValueL( TDCMONode aId, TDes& aStrValue);
    virtual TDCMOStatus  SetDCMOPluginIntAttributeValueL( TDCMONode aId, TInt aValue);
    virtual TDCMOStatus  SetDCMOPluginStrAttributeValueL( TDCMONode aId, const TDes& aStrValue);
    virtual void GetLocalizedNameL( HBufC*& aLocName );
	
private:
	// Construction
	CWLanCtrlDCMOAdapter(TAny* aInitParams);
	void ConstructL();
	TDCMOStatus MapFeatureControlError( TInt aErrorCode );

private:
	// Data to pass back from implementation to client
	HBufC* iProperty;
	HBufC* iDescription;
	// Parameters taken from client
	CDCMOInterface::TDCMOInterfaceInitParams* iInitParams;
	
	
	};  

#endif
