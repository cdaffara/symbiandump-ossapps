// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CCALDATAEXCHANGEIMPL_H__
#define __CCALDATAEXCHANGEIMPL_H__

#include <vutil.h>

const TUid  KUidEComAgnVersitInterface = {0x102035F4};

class CAgnVersit;
class CCalEntry;
class CCalSession;
class MCalDataExchangeCallBack;
class RReadStream;
class RWriteStream;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CCalDataExchangeImpl) : public CBase
	{
public:
	CCalDataExchangeImpl(CCalSession& aSession);
	~CCalDataExchangeImpl();
	
	void ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray);
	void ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, TInt aFlags);
	void ExportL(TUid aDataFormat, RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray);

	void ImportL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, 
													TInt aFlags, TInt aNumEntries);
	
	void ImportAsyncL(TUid aDataFormat, RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray, MCalDataExchangeCallBack& aObserver, TInt aFlags);
	void ExportAsyncL(TUid aDataFormat, RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray, MCalDataExchangeCallBack& aObserver);

	void DoImportEntryArrayTransferL();
	
private:
	void LoadDllL();
	void ImportVCalL(TInt aFlags, RReadStream& aReadStream, RPointerArray<CCalEntry>& aEntryArray);

private: 
	CCalSession& iSession;
	RLibrary                                iLibrary;
	TBool									iDllLoadFailed;
	CAgnVersit*                             iVCalConverter;
	};

class MAgnVersit
/** Provides an interface to decouple the static dependency of CAgnEntryModel on 
Versit.

This interface is implemented by the internal class TAgnVCalConverter, in a DLL 
called agnversit.dll. Rather than statically linking against versit.dll, the 
agenda engine dynamically loads agnversit.dll, to remove its dependency on Versit. 
All CAgnEntryModel's vCalendar-specific functions call the corresponding functions, 
below. 
@internalAll
@released
*/
	{
public:
	/** Imports a vCalendar consisting of one or more vCalendar entities.
	
	The vCalendar is read from the read stream specified and its entities are 
	converted into agenda entries and returned in the array aEntryArray.
	
	@param aFlags CAgnEntryModel::ELocalTime causes all date/times to be converted 
	to machine-local; CAgnEntryModel::EDefault leaves them unadjusted.
	@param aReadStream The read stream which contains the vCalendar.
	@param aEntryArray On return, a list of the agenda entries which were imported 
	from the vCalendar. The list contains the vEvents followed by the vTodos.
	@param aEntryModel Pointer to the agenda entry model instance that is calling 
	this function. */
	virtual void ImportVCalL(TInt aFlags, RReadStream& aReadStream, RPointerArray<CCalEntry>& aEntryArray)=0;
	/** Exports one or more agenda entries as vCalendar entities to the write stream 
	specified. 
	
	The entries are exported either as vTodos, if they are to-dos, otherwise as 
	vEvents.
	
	@param aFlags CAgnEntryModel::ELocalTime causes all date/times to be left 
	as machine-local; CAgnEntryModel::EDefault causes them to be converted into 
	UTC.
	@param aWriteStream The stream to which the agenda entries should be externalised 
	as vCalendar entities.
	@param aEntryArray Pointer to the array of agenda entries to export.
	@param aCharSet The character encoding type.
	@param aEntryModel Pointer to the agenda entry model instance that is calling 
	this function. */
	virtual	void ExportVCalL(RWriteStream& aWriteStream, const RPointerArray<CCalEntry>& aEntryArray, const Versit::TVersitCharSet aCharSet)=0;
	/** Parses a vCalendar stream and updates the appropriate entries.
	
	/** Asynchronously imports a vCalendar consisting of one or more vCalendar entities.
	
	The vCalendar is read from the read stream specified and its entities are 
	converted into agenda entries and returned in the array aEntryArray.
	@param aFlags CAgnEntryModel::ELocalTime causes all date/times to be converted 
	to machine-local; CAgnEntryModel::EDefault leaves them unadjusted.
	@param aReadStream The read stream which contains the vCalendar.
	@param aEntryArray On return, a list of the agenda entries which were imported 
	from the vCalendar. The list contains the vEvents followed by the vTodos.
	@param aEntryModel Pointer to the agenda entry model instance that is calling 
	this function. 
	@param aObserver Callback to indicate the progress of the import and when it has completed.
	@publishedAll
	@released
	 */
	virtual void ImportVCalAsyncL(TInt aFlags, RReadStream& aReadStream, RPointerArray<CCalEntry>& aEntryArray, 
				 MCalDataExchangeCallBack& aObserver) = 0;
					
	/** Asynchronously exports one or more agenda entries as vCalendar entities to the write stream 
	specified. 
	
	The entries are exported either as vTodos, if they are to-dos, otherwise as 
	vEvents.
	@param aFlags CAgnEntryModel::ELocalTime causes all date/times to be left 
	as machine-local; CAgnEntryModel::EDefault causes them to be converted into 
	UTC.
	@param aWriteStream The stream to which the agenda entries should be externalised 
	as vCalendar entities.
	@param aEntryArray Pointer to the array of agenda entries to export.
	@param aCharSet The character encoding type.
	@param aEntryModel Pointer to the agenda entry model instance that is calling 
	this function. 
	@param aObserver Callback to indicate the progress of the import and when it has completed.
	@publishedAll
	@released
	 */
	virtual void ExportVCalAsyncL(RWriteStream& aWriteStream, RPointerArray<CCalEntry>& aCalEntryArray, 
				MCalDataExchangeCallBack& aObserver, const Versit::TVersitCharSet aCharSet) = 0;

	};
	
class CAgnVersit: public CBase , public MAgnVersit
/** Provides an interface for ECom plugins
@internalAll
@released
*/
	{
public:
	static CAgnVersit* NewL();
	/** Releases Ecom resources
	*/
	virtual ~CAgnVersit();
private:
	TUid iDtor_ID_Key;
	};
	
#endif // __CCALDATAEXCHANGEIMPL_H__
