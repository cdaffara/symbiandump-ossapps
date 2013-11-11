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

#ifndef __AGSFILECONVERTER_H__
#define __AGSFILECONVERTER_H__

#include <s32std.h>
#include <calcommon.h>

class CAgnAttendee;
class CAgnCalendarConverter;
class CAgnCategory;
class CAgnCategoryList;
class CAgnEntry;
class CAgnEntryManager;
class CAgnModelStreamIdSet;
class CAgnRptDef;
class CAgnServFile;
class CFileStore;
class CStreamDictionary;
class TAgnCalendarTime;
class TAgnVersion;
class CAgnTzRuleIndex;

/**
Factory class for creating file converters.
*/
NONSHARABLE_CLASS(CalFileVersionUtils)
	{
public:
	static CAgnCalendarConverter* CreateConverterL(const TAgnVersion& aFileVersion, CAgnServFile& aAgnServerFile);
	static void FileVersionSupportedL(const TAgnVersion& aFileVersion, CalCommon::TCalFileVersionSupport& aStatus);
	static TAgnVersion CurrentFileVersion();
	};

/**
Base class for converting older versions of the calendar file to the latest version.
This class contains all code which is not version-specific, i.e. storing the new entry and adding it to the indexes.
@internalComponent
*/
NONSHARABLE_CLASS(CAgnCalendarConverter): public CBase
	{
public:
	virtual ~CAgnCalendarConverter();
	
	void CompleteConversionL();
	
	virtual void InternalizeCategoriesL() = 0;
	virtual void InternalizeEntriesL(RReadStream& aStream) = 0;
	virtual void InternalizeModelStreamIdSetL(RReadStream& aStream, CAgnModelStreamIdSet& aModelStreamIdSet) = 0;
	virtual void InternalizeNextUidValuesL(RReadStream& aStream) = 0;

protected:
	CAgnCalendarConverter();
	void ConstructL(CAgnServFile& aAgnServerFile);

	void AddToIndexL(CAgnEntry& aEntry) const;
	void SaveRestoredEntryL(CAgnEntry& aEntry) const;
	void SetNextLocalUidValue(TInt aValue) const;
	void SetModelStreamIds(const TAgnVersion& aVersion,
						   const TStreamId& aEntrySetStreamId,
						   const TStreamId& aNextLocalUidValueStreamId,
						   const TStreamId& aEntryManagerStreamId,
						   const TStreamId& aFileInformationStreamId,
						   CAgnModelStreamIdSet& aModelStreamIdSet) const;
						   
	CFileStore& InputStreamStore() const;
	CFileStore& OutputStreamStore() const;
	CAgnServFile& AgnServerFile() const;
	CStreamDictionary& Dictionary() const;

private:
	TStreamId WriteDescriptorToStreamL(const TDesC& aString) const;
	
	virtual CAgnEntry* InternalizeEntryL(RReadStream& aStream, TBool& aHasExtended) = 0;
	virtual void InternalizeExtendedEntryL(RReadStream& aStream, CAgnEntry& aEntry) = 0;

private:
	CAgnServFile*			iAgnServerFile; // ServerFile whose store is being converted. Not owned.
	CFileStore*				iInputStreamStore; // File Store of old version where data is read from. Not owned.
	CFileStore*				iOutputStreamStore; // Converted data is written to this file store.
	CStreamDictionary*		iDictionary; // dictionary for iOutputStreamStore.
	
	// Model's persistence layer objects that need building as the file conversion progresses
	CAgnModelStreamIdSet*	iOutModelStreamIdSet;
	CAgnEntryManager*		iOutEntryManager;
	
	//The tz rule index based on the converting file (using store and dictionary of converting file)
	CAgnTzRuleIndex*		iOutputTzRuleIndex;
	};
	
/**
Specific file conversion class for converting from file format 209 to the current version.
This file format is used in v9.1 of Symbian OS.
@internalComponent
*/
NONSHARABLE_CLASS(CAgnCalendarConverter209): public CAgnCalendarConverter
	{
public:
	static CAgnCalendarConverter209* NewL(CAgnServFile& aAgnServerFile);
	
protected:
	CAgnCalendarConverter209();
	
	TInt32 InternalizeAlarmL(RReadStream& aStream, TInt16 aAttributes, CAgnEntry& aEntry) const;
	TInt32 CalculateAlarmOffsetL(TInt32 aPreTime, const TTime& aAlarmOrigin) const;
	void InternalizeSummaryL(RReadStream& aStream, TBool aHasBeenDeleted, CAgnEntry& aEntry) const;
	void InternalizeTypeSpecificDataL(RReadStream& aStream, TInt16 aAttributes, CAgnEntry& aEntry) const;

	CArrayFixFlat<TAgnCalendarTime>* InternalizeRptDefL(RReadStream& aStream, CAgnEntry& aEntry) const;
	void AddExceptionsL(CAgnEntry& aEntry, CArrayFixFlat<TAgnCalendarTime>* aExceptions) const;
	void InternalizeRepeatRuleL(RReadStream& aStream, CAgnRptDef& aRptDef) const;
	CArrayFixFlat<TAgnCalendarTime>* InternalizeExceptionsL(RReadStream& aStream) const;

	void InternalizeSporadicDatesL(RReadStream& aStream, CAgnRptDef& aRptDef) const;
	virtual void InternalizeTimeZoneL(RReadStream& aStream, CAgnRptDef& aRptDef) const;
	CAgnAttendee* InternalizeAttendeeL(RReadStream& aStream, TBool& aOrganizer) const;
	CAgnCategory* InternalizeCategoryL(RReadStream& aStream) const;
	void InternalizeNextUidValuesL(RReadStream& aStream);
	
	// from CAgnCalendarConverter
	CAgnEntry* InternalizeEntryL(RReadStream& aStream, TBool& aHasExtended);
	void InternalizeExtendedEntryL(RReadStream& aStream, CAgnEntry& aEntry);
	void InternalizeCategoriesL();
	void InternalizeEntriesL(RReadStream& aStream);
	void InternalizeModelStreamIdSetL(RReadStream& aStream, CAgnModelStreamIdSet& aModelStreamIdSet);

	};
	
/**
Specific file conversion class for converting from file format 210 to the current version.
This file format is used from v9.2 of Symbian OS.

This contains alarm action info at the end of the extended entry data. InternalizeExtendedEntryL has been overwritten to 
call CAgnCalendarConverter::InternalizeExtendedEntryL and then InternalizeAlarmActionL.
@internalComponent
*/
NONSHARABLE_CLASS(CAgnCalendarConverter210) : public CAgnCalendarConverter209
	{
public:
	static CAgnCalendarConverter210* NewL(CAgnServFile& aAgnServerFile);
	
	// from CAgnCalendarConverter209 - overwritten to include InternalizeAlarmActionL
	void InternalizeExtendedEntryL(RReadStream& aStream, CAgnEntry& aEntry);
	
protected:
	CAgnCalendarConverter210();
	void InternalizeAlarmActionL(RReadStream& aStream, CStreamStore& aInputStreamStore, CAgnEntry& aEntry);
	};

/**
Specific file conversion class for converting from file format 210 to the current version.
This file format is used from v9.2 of Symbian OS.

This contains alarm action info at the end of the extended entry data. InternalizeExtendedEntryL has been overwritten to 
call CAgnCalendarConverter::InternalizeExtendedEntryL and then InternalizeAlarmActionL.
@internalComponent
*/
NONSHARABLE_CLASS(CAgnCalendarConverter211) : public CAgnCalendarConverter210
	{
public:
	static CAgnCalendarConverter211* NewL(CAgnServFile& aAgnServerFile);
	virtual ~CAgnCalendarConverter211();
	
	virtual void InternalizeTimeZoneL(RReadStream& aStream, CAgnRptDef& aRptDef) const;
	
private:
	void ConstructL(CAgnServFile& aAgnServerFile);
	CAgnCalendarConverter211();
	
private:
	//The tz rule index stored indexes in original 9.4 file.
	CAgnTzRuleIndex* iInputTzRuleIndex;
	};


// Note:
// Any further converters from, e.g. file format 211, should derive from CAgnCalendarConverter210 and override the relevant function,
// probably InternalizeExtendedEntryL.

#endif // __AGSFILECONVERTER_H__
