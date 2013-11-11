// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of CPhoneBookIniFile class which manages the PhBkSync.INI
// file.
// 
//

/**
 @file
 @internalComponent
*/

#include "phbksync.h"
#include "Phonebook.h"
#include "PhonebookIniFile.h"
#include "PhonebookManager.h"
#include "phbksynclog.h"
#include "common.h"


//
// Constants for the INI file.  Do not change these.
//
_LIT8(KPhBkSyncStartOptionText, "PhBkSyncStartOption");
_LIT8(KPhBkSyncTemplateIdText,  "PhBkSyncTemplateId");
_LIT8(KPhBkSyncGroupIdText,     "PhBkSyncGroupId");
_LIT8(KPhBkSyncPhonebookIdText, "PhBkSyncPhonebookId");


//
// Minimum and maximum dimensions of an INI file...
//
const TInt KIniFileMaxPhonebooks = KMaxPhonebooks;
const TInt KIniFileMinimumSize   = 1;
const TInt KIniFileNoOfSettings  = 4;
const TInt KIniFileMaxLineLength = 64; // Approx. 20+1+9+1+15=46 max.
const TInt KIniFileMaximumSize   = (KIniFileMaxPhonebooks *
                                    KIniFileNoOfSettings *
                                    KIniFileMaxLineLength);


/**
 *  Static factory method used to create a CPhoneBookIniFile object.
 *
 *  @param aPhonebookList  A list of phonebook UIDs supported by the Phonebook
 *                         synchroniser.
 *
 *  @return  Pointer to the created CPhoneBookIniFile object, or NULL.
 */
CPhoneBookIniFile* CPhoneBookIniFile::NewL(RPointerArray<CPhoneBook>& aPhonebookList)
	{
	LOGINIFILE2(_L8("NewL(%d phonebooks)"), aPhonebookList.Count());

	CPhoneBookIniFile*  ptr = new (ELeave) CPhoneBookIniFile(aPhonebookList);
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	CleanupStack::Pop(ptr);
	return ptr;
	} // CPhoneBookIniFile::NewL


/**
 *  Standard constructor.
 *
 *  @param aPhonebookList  A list of phonebook UIDs supported by the Phonebook
 *                         synchroniser.
 */
CPhoneBookIniFile::CPhoneBookIniFile(RPointerArray<CPhoneBook>& aPhonebookList)
  : iPhonebookList(aPhonebookList),
    iIsIniFileCacheValid(EFalse)
	{
	// NOP
	} // CPhoneBookIniFile::CPhoneBookIniFile


/**
 *  Standard destructor.
 */
CPhoneBookIniFile::~CPhoneBookIniFile()
	{
	// NOP
	} // CPhoneBookIniFile::~CPhoneBookIniFile


/**
 *  Second stage contructor.
 */
void CPhoneBookIniFile::ConstructL()
	{
	LOGINIFILE1(_L8("ConstructL()"));
	
	__ASSERT_DEBUG(iPhonebookList.Count() <= KMaxPhonebooks,
				   PhBkSyncPanic(EPhBkSyncPanicTooManyPhonebooks));

	//
	// Read the INI file and initialise the cache. If a problem occurs
	// during the read of the INI file then we let it go as the server
	// must still start. If the file is corrupt then we write a new one
	// out straight away.
	//
	TRAPD(result, ReadIniFileL());
	LOGINIFILE2(_L8("Read of the INI file returned %d"), result);

	if (result == KErrNone)
		{
		UpdatePhonebookParamsCache();
		}
	else if (result == KErrCorrupt)
		{
		WriteIniFileSettingsIfNeeded();
		}
	} // CPhoneBookIniFile::ConstructL


/**
 *  Requests the writting of the Ini file if required.
 *
 *  @return KErrNone if successful or a Standard Symbian error code.
 */
TInt CPhoneBookIniFile::WriteIniFileSettingsIfNeeded()
	{
	LOGINIFILE1(_L8("WriteIniFileSettingsIfNeeded()"));
	
	//
	// Check the current cached values against the phonebooks to see if
	// we need to file the INI file. If so then write the file.
	//
	TInt  result(KErrNone);
	
	if (CheckPhonebookParamsForChanges())
		{
		TRAP(result, WriteIniFileL());
		LOGINIFILE2(_L8("Write of the INI file returned %d"), result);
	
		//
		// If the INI file was written ok, then update the cache...
		//
		if (result == KErrNone)
			{
			UpdatePhonebookParamsCache();
			}
		}
	
	return result;
	} // CPhoneBookIniFile::WriteIniFileSettingsIfNeeded


/**
 *  Checks if any of the phonebook parameters have changed. This would indicate
 *  that the INI file should be re-written.
 *
 *  @return ETrue if the phonebook parameters have changed, EFalse otherwise.
 */
TBool CPhoneBookIniFile::CheckPhonebookParamsForChanges() const
	{
	LOGINIFILE1(_L8("CheckPhonebookParamsForChanges()"));
				
	//
	// If the INI file does not exist or is corrupt, then write it...
	//
	if (iIsIniFileCacheValid == EFalse)
		{
		LOGINIFILE1(_L8("No INI file data loaded so file needs writting"));
		return ETrue;
		}
	
	//
	// Check each phonebook against the cache for unwritten changes.
	//
	TInt  needsWriting(EFalse);
	TInt  phonebookCount(iPhonebookList.Count());
	
	for (TInt index = 0;  index < phonebookCount;  index++)
		{
		CPhoneBook*  phonebook = iPhonebookList[index];

		if (phonebook != NULL)
			{
			RMobilePhoneBookStore::TMobilePhoneBookInfoV5  phBkInfo;
			
			phBkInfo = phonebook->GetPhoneBookInfo();

			if (iIniFileSyncModeCache[index] != phonebook->GetSyncMode()  ||
				iIniFileTemplateIdCache[index] != phonebook->GetTemplateId()  ||
				iIniFileGroupIdCache[index] != phonebook->GetGroupId()  ||
				iIniFileIdentityCache[index] != phBkInfo.iIdentity)
				{
				needsWriting = ETrue;
				break;
				}
			}
		}
	
	//
	// Log if the INI file needs writing...
	//
	if (needsWriting == EFalse)
		{
		LOGINIFILE1(_L8("INI file does not need to be written"));
		}
	else
		{
		LOGINIFILE1(_L8("INI file needs to be written"));
		}
	
	return needsWriting;
	} // CPhoneBookIniFile::CheckPhonebookParamsForChanges


/**
 *  Get the full path name for the INI file.
 *
 *  @param aFs           File server handle.
 *  @param aIniFileName  Path will be returned in aPath.
 */
void CPhoneBookIniFile::GetIniFileName(RFs& aFs, TDes& aIniFileName) const
	{
	LOGINIFILE1(_L8("GetIniFileName()"));
	
	//
	// Name is made up of the system drive, private path and INI file name...
	//
	_LIT(KPhBkSyncIniFileName, "PhBkSync.ini");

	TDriveUnit  systemDrive(aFs.GetSystemDrive()); 
	TPath  privatePath;
	
	aFs.PrivatePath(privatePath);

	aIniFileName.Zero();
	aIniFileName.Append(systemDrive.Name());
	aIniFileName.Append(privatePath);
	aIniFileName.Append(KPhBkSyncIniFileName);

	//
	// Debug logging of the INI file name...
	//	
#ifdef _DEBUG
	TBuf8<KMaxFileName>  aIniFileNameIn8Bit;
	aIniFileNameIn8Bit.Copy(aIniFileName);
	LOGINIFILE2(_L8("INI file \"%S\""), &aIniFileNameIn8Bit);
#endif
	} // CPhoneBookIniFile::GetIniFileName


/**
 *  Write all phonebook settings to the Phonebook INI file. If the file
 *  does not exist it will create one.
 */
void CPhoneBookIniFile::WriteIniFileL() const
	{
	LOGINIFILE1(_L8("WriteIniFileL()"));

	//
	// Connect to the File Server...
	//
	RFs  fs;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	//
	// Get the name of the INI file...
	//
	TFileName  iniFileName;
	GetIniFileName(fs, iniFileName);

	//
	// Ensure that the private directory exists...
	//
	TInt  result = fs.MkDirAll(iniFileName);
	if (result != KErrAlreadyExists  &&  result != KErrNone)
		{
		User::Leave(result);
		}
	
	//
	// Open the file for writing, truncating it if needed...
	//
	RFile  file;

	User::LeaveIfError(file.Replace(fs, iniFileName, EFileShareAny|EFileWrite));
	CleanupClosePushL(file);

	//
	// Write the settings for each phonebook...
	//
	_LIT8(KIniLineValueFormat, "%S= %d %d\r\n");
	_LIT8(KIniLineStringFormat, "%S= %d %S\r\n");
	TInt  phonebookCount(iPhonebookList.Count());
	TBuf8<KIniFileMaxLineLength>  line;
	
	for (TInt index = 0;  index < phonebookCount;  index++)
		{
		CPhoneBook*  phonebook = iPhonebookList[index];

		if (phonebook != NULL)
			{
			TUid  phonebookUid = phonebook->GetPhonebookUid();

			// Phonebook Sync Mode...
			line.Format(KIniLineValueFormat, &KPhBkSyncStartOptionText,
						phonebookUid.iUid, phonebook->GetSyncMode());
			User::LeaveIfError(file.Write(line));
#ifdef _DEBUG
			line.SetLength(line.Length() - 2);
			LOGINIFILE2(_L8("INI line \"%S\""), &line);
#endif

			// Phonebook Template ID...
			line.Format(KIniLineValueFormat, &KPhBkSyncTemplateIdText,
						phonebookUid.iUid, phonebook->GetTemplateId());
			User::LeaveIfError(file.Write(line));
#ifdef _DEBUG
			line.SetLength(line.Length() - 2);
			LOGINIFILE2(_L8("INI line \"%S\""), &line);
#endif

			// Phonebook Group ID...
			line.Format(KIniLineValueFormat, &KPhBkSyncGroupIdText,
						phonebookUid.iUid, phonebook->GetGroupId());
			User::LeaveIfError(file.Write(line));
#ifdef _DEBUG
			line.SetLength(line.Length() - 2);
			LOGINIFILE2(_L8("INI line \"%S\""), &line);
#endif

			// Phonebook ID string...
			RMobilePhoneBookStore::TMobilePhoneBookInfoV5  phBkInfo;
			
			phBkInfo = phonebook->GetPhoneBookInfo();

			line.Format(KIniLineStringFormat, &KPhBkSyncPhonebookIdText,
						phonebookUid.iUid, &phBkInfo.iIdentity);
			User::LeaveIfError(file.Write(line));
#ifdef _DEBUG
			line.SetLength(line.Length() - 2);
			LOGINIFILE2(_L8("INI line \"%S\""), &line);
#endif
			}
		}

	CleanupStack::PopAndDestroy(2, &fs); // fs, file
	} // CPhoneBookIniFile::WriteIniFileL


/**
 *  Read all phonebook settings from the Phonebook INI file if it exists.
 */
void CPhoneBookIniFile::ReadIniFileL() const
	{
	LOGINIFILE1(_L8("ReadIniFileL()"));

	//
	// Connect to the File Server...
	//
	RFs  fs;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	//
	// Get the name of the INI file...
	//
	TFileName  iniFileName;
	GetIniFileName(fs, iniFileName);

	//
	// Open the INI file...
	//
	RFile  file;

	User::LeaveIfError(file.Open(fs, iniFileName, EFileShareAny | EFileRead));
	CleanupClosePushL(file);

	//
	// Load the file into memory...
	//
	TInt  fileSize;

	User::LeaveIfError(file.Size(fileSize));

	if (fileSize < KIniFileMinimumSize  ||  fileSize > KIniFileMaximumSize)
		{
		User::Leave(KErrCorrupt);
		}

	HBufC8*  iniFileBuf = HBufC8::NewLC(fileSize);
	TPtr8  iniFilePtr(iniFileBuf->Des());
	User::LeaveIfError(file.Read(0, iniFilePtr));

	//
	// Parse the INI file text...
	//
	TLex8  iniFile(*iniFileBuf);

	while (!iniFile.Eos()) 
		{
		//
		// The settings for each follow the pattern:
		//
		//   <Setting>= <Phonebook> <Value>
		//
		TPtrC8  settingsToken(iniFile.NextToken());
		iniFile.SkipSpace();

		//
		// Check the setting field is correct...
		//
		if (iniFile.Eos())
			{
			break;
			}

		if (settingsToken.Find(KPhBkSyncStartOptionText) != KErrNone  &&
			settingsToken.Find(KPhBkSyncTemplateIdText) != KErrNone  &&
			settingsToken.Find(KPhBkSyncGroupIdText) != KErrNone  &&
			settingsToken.Find(KPhBkSyncPhonebookIdText) != KErrNone)
			{
			LOGINIFILE2(_L8("Invalid setting token (%S)"), &settingsToken);
			continue;
			}

		//
		// Get the phonebook. This is the UID value but was previously
		// a constant, so we read both.
		//
		iniFile.SkipSpace();
		if (iniFile.Eos())
			{
			break;
			}

		TInt  phonebookNum(0);
		TUid  phonebookUid;

		TInt  result = iniFile.Val(phonebookNum);
		if (result != KErrNone)
			{
			LOGINIFILE2(_L8("Invalid phonebook number (error %d)"), result);
			continue;
			}
		
		switch (phonebookNum)
			{
			case 1:
				{
				phonebookUid = KUidIccGlobalAdnPhonebook;
				}
				break;
				
			case 2:
				{
				phonebookUid = KUidIccGlobalSdnPhonebook;
				}
				break;
				
			case 3:
				{
				phonebookUid = KUidIccGlobalLndPhonebook;
				}
				break;
				
			case 4:
				{
				phonebookUid = KUidUsimAppAdnPhonebook;
				}
				break;
				
			case 5:
				{
				phonebookUid = KUidIccGlobalFdnPhonebook;
				}
				break;
				
			default:
				{
				phonebookUid = TUid::Uid(phonebookNum);
				}
				break;
			}	

		TInt  phonebookCount(iPhonebookList.Count());
		CPhoneBook*  phonebook(NULL);

		for (TInt index = 0;  index < phonebookCount;  index++)
			{
			if (iPhonebookList[index]->GetPhonebookUid() == phonebookUid)
				{
				phonebook = iPhonebookList[index];
				break;
				}
			}

		if (phonebook == NULL)
			{
			LOGINIFILE2(_L8("Phonebook does not exist (%d)"), phonebookNum);
			continue;
			}

		//
		// Read the value and set it...
		//
		iniFile.SkipSpace();
		if (iniFile.Eos())
			{
			break;
			}

		if (settingsToken.Find(KPhBkSyncStartOptionText) == KErrNone)
			{
			TInt  syncModeNum(-1);
			
			result = iniFile.Val(syncModeNum);
			if (result != KErrNone)
				{
				LOGINIFILE2(_L8("Invalid sync mode number (error %d)"), result);
				continue;
				}

			RPhoneBookSession::TPhonebookSyncMode  syncMode;

			syncMode = static_cast<RPhoneBookSession::TPhonebookSyncMode>(syncModeNum);

			if (syncMode != RPhoneBookSession::EAutoCurrentIcc  &&
				syncMode != RPhoneBookSession::EAutoSameIcc  &&
				syncMode != RPhoneBookSession::EManual)
				{
				LOGINIFILE2(_L8("Invalid sync mode value (%d)"),
				            (TInt) syncMode);
				continue;
				}

			phonebook->SetSyncMode(syncMode);
			
			LOGINIFILE4(_L8("INI read as \"%S %d %d\""),
						&KPhBkSyncStartOptionText,
						phonebook->GetPhonebookUid().iUid, (TInt) syncMode);
			}
		else if (settingsToken.Find(KPhBkSyncTemplateIdText) == KErrNone)
			{
			TContactItemId  templateId(-1);
			
			result = iniFile.Val(templateId);
			if (result != KErrNone)
				{
				LOGINIFILE3(_L8("Invalid template Id number (error %d, number %d)"),
				            result, templateId);
				continue;
				}

			phonebook->SetTemplateId(templateId);

			LOGINIFILE4(_L8("INI read as \"%S %d %d\""),
						&KPhBkSyncTemplateIdText,
						phonebook->GetPhonebookUid().iUid, templateId);
			}
		else if (settingsToken.Find(KPhBkSyncGroupIdText) == KErrNone)
			{
			TContactItemId  groupId(0);
			
			result = iniFile.Val(groupId);
			if (result != KErrNone)
				{
				LOGINIFILE3(_L8("Invalid group ID number (error %d, number %d)"),
				            result, groupId);
				continue;
				}

			phonebook->SetGroupId(groupId);

			LOGINIFILE4(_L8("INI read as \"%S %d %d\""),
						&KPhBkSyncGroupIdText,
						phonebook->GetPhonebookUid().iUid, groupId);
			}
		else if (settingsToken.Find(KPhBkSyncPhonebookIdText) == KErrNone)
			{
			TPtrC8  phonebookId(iniFile.NextToken());
			RMobilePhoneBookStore::TMobilePhoneBookInfoV5  phBkInfo;

			phBkInfo = phonebook->GetPhoneBookInfo();

			if (phonebookId.Length() > phBkInfo.iIdentity.MaxSize())
				{
				LOGINIFILE2(_L8("Invalid phonebook ID (\"%S\")"), &phonebookId);
				}

			phBkInfo.iIdentity.Copy(phonebookId);
			phonebook->SetPhoneBookInfo(phBkInfo);

			LOGINIFILE4(_L8("INI read as \"%S %d %S\""),
						&KPhBkSyncPhonebookIdText,
						phonebook->GetPhonebookUid().iUid, &phBkInfo.iIdentity);
			}
		}

	CleanupStack::PopAndDestroy(3, &fs); // fs, file, contents
	} // CPhoneBookIniFile::ReadIniFileL


/**
 *  Update the internal cache of phonebook parameters with the real values.
 *  This function is used after the INI file is written.
 */
void CPhoneBookIniFile::UpdatePhonebookParamsCache()
	{
	LOGINIFILE1(_L8("UpdatePhonebookParamsCache()"));
				
	TInt  phonebookCount(iPhonebookList.Count());
	
	for (TInt index = 0;  index < phonebookCount;  index++)
		{
		CPhoneBook*  phonebook = iPhonebookList[index];

		if (phonebook != NULL)
			{
			RMobilePhoneBookStore::TMobilePhoneBookInfoV5  phBkInfo;
			
			phBkInfo = phonebook->GetPhoneBookInfo();

			iIniFileSyncModeCache[index] = phonebook->GetSyncMode();
			iIniFileTemplateIdCache[index] = phonebook->GetTemplateId();
			iIniFileGroupIdCache[index] = phonebook->GetGroupId();
			iIniFileIdentityCache[index].Copy(phBkInfo.iIdentity);
			}
		}

	iIsIniFileCacheValid = ETrue;
	} // CPhoneBookIniFile::UpdatePhonebookParamsCache


