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
* Description:  header file for DSCtrl plug-in adapter
*
*/

#ifndef _NSMLDSCTRLDCMOADAPTER__
#define _NSMLDSCTRLDCMOADAPTER__

// Header needed for our CDCMO interface
#include "dcmointerface.h"

/** 
 * An implementation of the CDCMOInterface definition
 * Provides methods to read/write the properties of DCMO Plugin
 * @lib nsmldsctrldcmoadapter.dll
 */
class CNsmlDsCtrlDCMOAdapter : public CDCMOInterface
	{

public:
	/**
	* Two phase construction
	* Standardised safe construction which leaves nothing the cleanup stack.
    */
	static CNsmlDsCtrlDCMOAdapter* NewL(TAny* aInitParams);
	
	/**
    * Destructor
    */
	~CNsmlDsCtrlDCMOAdapter();

	// Implementation of CDCMOInterface
	
	/**
    * Get DCMO Plug-in's attribute value which are integers
    * 
    * @param aId, it refer to the attributes of the capability
    * @param aValue, it refer to the value of the attribute to be fetched
    * @return status, It returns DCMO request success or failure
    */
	TDCMOStatus  GetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt& aValue);
	
	/**
    * Get DCMO Plug-in's attribute value which are strings
    * 
    * @param aId, it refer to the attributes of the capability
    * @param aValue, it refer to the value of the attribute to be fetched
    * @return status, It returns DCMO request success or failure
    */
	TDCMOStatus  GetDCMOPluginStrAttributeValueL(TDCMONode aId, TDes& aStrValue);
	
	/**
    * Set DCMO Plug-in's attribute value which are integers
    * 
    * @param aId, it refer to the attributes of the capability
    * @param aValue, it refer to the value of the attribute to be set
    * @return status, It returns DCMO request success or failure
    */
	TDCMOStatus  SetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt aValue);
	
	/**
    * Set DCMO Plug-in's attribute value which are strings
    * 
    * @param aId, it refer to the attributes of the capability
    * @param aValue, it refer to the value of the attribute to be set
    * @return status, It returns DCMO request success or failure
    */
	TDCMOStatus  SetDCMOPluginStrAttributeValueL(TDCMONode aId, const TDes& aStrValue);
	
	/**
    * Get localised name of the Plug-in
    * 
    * @param aLocName, member to store the name of Plugin
    */
	void GetLocalizedNameL (HBufC*& aLocName);    
	
private:
	/**
    * Construction
    */
    CNsmlDsCtrlDCMOAdapter(TAny* aInitParams);
  
    /**
	* Second Phase constructor
	*/
	void ConstructL();

private:
	// Data to pass back from implementation to client
	
	/**
	* Member to store the Property Attribute of the Plug-in
	* It will be sent back to the client
	*/
	HBufC* iProperty;
	
	/**
	* Member to store the Description Attribute of the Plug-in
	* It will be sent back to the client
	*/
	HBufC* iDescription;
	
	/**
	* Parameters taken from client
	*/
	TDCMOInterfaceInitParams* iInitParams;
	};  

#endif //_NSMLDSCTRLDCMOADAPTER__
