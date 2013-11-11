// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

inline TUid CParserPropertyValue::Uid() const
/** Returns the property value's UID.

This UID identifies the property value's type and is specified on construction. 
The values are defined in vuid.h.

@return The property value's UID. */
	{return iPropertyValueTypeUid;}

inline void CParserPropertyValue::SetPlugIn(MVersitPlugIn* aPlugIn)
/** Assigns a Versit plug-in to the property value.

Allows functions of the MVersitPlugIn class to be used when writing to a stream.

If a plug-in is in use then this function needs to be called with each property value 
before it is externalised. However, this will be done for you if you add a property 
using the function CVersitParser::AddPropertyL(). 

@param aPlugIn A pointer to an MVersitPlugIn instance. */
	{iPlugIn=aPlugIn;}

inline MVersitPlugIn* CParserPropertyValue::PlugIn()
/** Gets a pointer to the Versit plug-in

@return A pointer to the plug-in. */
	{return iPlugIn;}

//
//  CParserPropertyValueCDesCArray
//
inline CParserPropertyValueBinary::CParserPropertyValueBinary() :CParserPropertyValue(TUid::Uid(KVersitPropertyBinaryUid))
/** Constructs a binary property value. 

Sets the property value UID to KVersitPropertyBinaryUid. */
	{}

inline CParserPropertyValueBinary::CParserPropertyValueBinary(CBufSeg& aValue) :CParserPropertyValue(TUid::Uid(KVersitPropertyBinaryUid)), iValue(&aValue)
/** Constructs a binary property value. 

Sets the property value to aValue and the property value UID to KVersitPropertyBinaryUid.

@param aValue The property value. */
	{}

//
//  CParserPropertyValueCDesCArray
//
inline CDesCArray* CParserPropertyValueCDesCArray::Value() const
/** Gets the property value.

@return Pointer to the array of descriptors owned by the property value object. */
	{return iValue;}

//
// CParserPropertyValueTimeZone
//
inline TTimeIntervalSeconds CParserPropertyValueTimeZone::Value() const
/** Gets the time zone property value.

@return A time interval (in seconds) which represents the offset from universal 
time. */
	{return iValue;}

//
// CParserPropertyValueDaylight
//
inline CVersitDaylight* CParserPropertyValueDaylight::Value() const
/** Gets the daylight saving property value.

@return Pointer to the daylight saving specification. */
	{return iValue;}

//
//  CParserPropertyValueDateTime
//
inline TVersitDateTime* CParserPropertyValueDateTime::Value() const
/** Gets a pointer to the date/time property value.

@return Pointer to the date/time property value. */
	{return iValue;}

//
//  CParserPropertyValueDate
//
inline TVersitDateTime* CParserPropertyValueDate::Value() const
/** Gets a pointer to the date property value.

@return Pointer to the date property value. */
	{return iValue;}

//
//  CParserPropertyValueMultiDateTime
//
inline CArrayPtr<TVersitDateTime>* CParserPropertyValueMultiDateTime::Value() const
/** Gets a pointer to the object's list of date/time property values.

@return Pointer to the array of date/time property values. */
	{return iValue;}

//
//  CParserPropertyValueInt
//
inline TInt CParserPropertyValueInt::Value() const
/** Gets the signed integer property value.

@return The signed integer property value. */
	{return iValue;}

//
//	CParserProperty
//
inline void CParserProperty::SetValue(CParserPropertyValue* aPropertyValue)
/** Sets the property value.

The property takes ownership of the property value.

@param aPropertyValue A pointer to the property value to be set. */
	{iPropertyValue=aPropertyValue;}

inline CParserPropertyValue* CParserProperty::Value() const
/** Gets a pointer to the property value.

Ownership of the property value is not transferred by this function.

@return Pointer to generic parser property value. */
	{return iPropertyValue;}

inline TUid CParserProperty::Uid() const
/** Gets the UID of the property value.

@return UID of the property value. */
	{return iPropertyValue->Uid();}

/*
 * Gets the name of the property.
 *
 * @return    " HBufC8*& "
 *            Pointer to the property name.
 */
inline HBufC8*& CParserProperty::NameBuf()
	{return iPropertyName;}

inline void CParserProperty::SetNameUid(TUid aUid)
/** Sets a property name UID.

This function is called by CVersitParser::AddProperty() when internalising, 
and will not normally be used directly.

@param aUid A property name UID. */
	{iPropertyNameUid=aUid;}

inline TUid CParserProperty::NameUid() const
/** Gets the property name UID.

@return UID of the property name. */
	{return iPropertyNameUid;}

inline void CParserProperty::SetParamArray(CArrayPtr<CParserParam>* aArrayOfParams)
/** Sets the array of property parameters. 

The property takes ownership of aArrayOfParams

@param aArrayOfParams Array of property parameters. */
	{iArrayOfParams=aArrayOfParams;}
