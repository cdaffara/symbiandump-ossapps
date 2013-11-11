// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>

// Globals
static RFs								TheFs;
static RTest							TheTest(_L("TLOADER - Test help file loading schema"));
static CTrapCleanup*					TheTrapCleanup;

// Constants

// Literal constants
_LIT(KHlpFileSpec,			"*.h*");

_LIT(KHlpFileSearchPath,	"\\Resource\\Help\\");

// Classes referenced
class CHlpFileEntry;

// Typedefs
typedef CArrayPtrFlat<CHlpFileEntry> CHlpFileList;


//
// ----> CHlpFileEntry (header)
//

class CHlpFileEntry : public CBase
	{
public:
	enum THelpFileType
		{
		EPrimary,
		ESecondary,
		EGeneral,			
		EInpropperLocale,
		EDiscarded
		};

public:
	static CHlpFileEntry* NewLC(TDriveUnit aDrive, const TDesC& aFile, TLanguage aLanguage);

	inline TDriveUnit Drive() const { return iDrive; }
	inline const TDesC& FileName() const { return iFile; }
	inline const TDesC& Name() const { return iName; }
	inline THelpFileType Type() const { return iType; }
	inline void SetType(THelpFileType aType) { iType = aType; }
	inline TLanguage HelpFileLocale() const { return iHelpFileLocale; }
	void GetFullNameAndPath(TDes& aName) const;

private:
	CHlpFileEntry(TDriveUnit aDrive, const TDesC& aFile) : iHelpFileLocale(ELangOther), iFile(aFile), iDrive(aDrive) {}
	void ConstructL(TLanguage aLanguage);
	void MakeLocaleSpecificExtension(TDes& aNameToAppendTo, TLanguage aLanguage) const;
	TLanguage NextLanguage(TLanguage aLanguage) const;

private:
	TLanguage iHelpFileLocale; // Only valid when the type is not EGeneral
	TName iName;
	TName iFile;
	TDriveUnit iDrive;
	THelpFileType iType;
	};




//
// ----> CHlpFileEntry (source)
//


void CHlpFileEntry::ConstructL(TLanguage aLanguage)
	{
	_LIT(KDefaultHelpFileExtension, ".hlp");

	// Decide what type of file this is....
	TFileName file;

	TChar driveLetter;
	User::LeaveIfError(RFs::DriveToChar(iDrive, driveLetter));
	file.Append(driveLetter);
	file.Append(':');
	file.Append(KHlpFileSearchPath);
	file.Append(iFile);

	TParsePtrC parser(file);
	if	(!parser.ExtPresent())
		User::Leave(KErrCorrupt);

	iName.Copy(parser.Name());

	TPtrC extension(parser.Ext());
	if	(extension.CompareF(KDefaultHelpFileExtension) == 0)
		iType = EGeneral;
	else
		{
		TFileName idealHelpFileName(parser.DriveAndPath());
		idealHelpFileName.Append(parser.Name());

		MakeLocaleSpecificExtension(idealHelpFileName, aLanguage);

		if	(idealHelpFileName.CompareF(file) == 0)
			{
			// This is a primary match
			iType = EPrimary;
			iHelpFileLocale = aLanguage;
			}
		else
			{
			// Is it a secondary match? If it isn't then it should be discarded....
			idealHelpFileName = parser.DriveAndPath();
			idealHelpFileName.Append(parser.Name());
	
			// Get the nearest secondary language
			aLanguage = NextLanguage(aLanguage);
			MakeLocaleSpecificExtension(idealHelpFileName, aLanguage);

			if	(idealHelpFileName.CompareF(file) == 0)
				{
				iHelpFileLocale = aLanguage;
				iType = ESecondary;
				}
			else
				{
				TLex lexer(extension);

				// Skip the leading .H
				lexer.Inc(2);
				
				// Lex the value, but silently ignore errors
				TUint localeAsUnsignedInt = ELangOther;
				lexer.Val(localeAsUnsignedInt);
				iHelpFileLocale = STATIC_CAST(TLanguage, localeAsUnsignedInt);
				iType = EInpropperLocale;
				}
			}
		}
	}


TLanguage CHlpFileEntry::NextLanguage(TLanguage aLanguage) const
	{
	switch(aLanguage)
		{
	case ELangSwissFrench:
		return ELangFrench;
	case ELangSwissGerman:
		return ELangGerman;
	case ELangBelgianFlemish:
		return ELangDutch;
	case ELangBelgianFrench:
		return ELangFrench;
	default:
		return ELangEnglish;
		}
	}


CHlpFileEntry* CHlpFileEntry::NewLC(TDriveUnit aDrive, const TDesC& aFile, TLanguage aLanguage)
	{
	CHlpFileEntry* self = new(ELeave) CHlpFileEntry(aDrive, aFile);
	CleanupStack::PushL(self);
	self->ConstructL(aLanguage);
	return self;
	}


//
//
//

void CHlpFileEntry::GetFullNameAndPath(TDes& aName) const
	{
	TChar driveLetter = '?';
	RFs::DriveToChar(Drive(), driveLetter);
	aName.Zero();
	aName.Append(driveLetter);
	aName.Append(':');
	aName.Append(KHlpFileSearchPath);
	aName.Append(FileName());
	}


void CHlpFileEntry::MakeLocaleSpecificExtension(TDes& aNameToAppendTo, TLanguage aLanguage) const
	{
	_LIT(KLocaleExtensionFormat, ".H%02d");
	aNameToAppendTo.AppendFormat(KLocaleExtensionFormat, aLanguage);
	}
















static void PrintEntryL(const TDesC& aPrompt, const CHlpFileEntry& aEntry, TInt aNumber = -1)
	{
	TFileName pFileName(aEntry.FileName());
	TChar driveLetter = '?';
	RFs::DriveToChar(aEntry.Drive(), driveLetter);

	//HBufC* buf = HBufC::NewLC(aPrompt.Length() + pFileName.Length() + KHlpFileSearchPath().Length() + 40);
	//TDes pBuf(buf->Des());
	TBuf<256> pBuf;		//Tempory fix to allow this to work in Code Warrior builds.

	
	if	(aNumber >= KErrNone)
		pBuf.Format(_L("\n%S %d: %c:%S%S\n"), &aPrompt, aNumber, static_cast<TUint>(driveLetter), &KHlpFileSearchPath(), &pFileName);
	else
		pBuf.Format(_L("\n%S: %c:%S%S\n"), &aPrompt, static_cast<TUint>(driveLetter), &KHlpFileSearchPath(), &pFileName);

	TheTest.Printf(pBuf);

	//CleanupStack::PopAndDestroy();
	}


// Defines
#define __PRINT_FILE(aPrompt, aEntry)				(PrintEntryL(aPrompt, aEntry))
#define __PRINT_FILE_NO(aPrompt, aEntry, aNumber)	(PrintEntryL(aPrompt, aEntry, aNumber))


static CHlpFileList* BuildListForDriveLC(TDriveUnit aDrive, RFs& aFsSession)
//
//	Generate a list of help files for the specified drive
//
	{
	CHlpFileList* list = new(ELeave) CHlpFileList(5);
	CleanupStack::PushL(list);

	// Generate the folder spec to search in
	TFileName searchSpec;
	TChar driveLetter;
	User::LeaveIfError(RFs::DriveToChar(aDrive, driveLetter));
	searchSpec.Append(driveLetter);
	searchSpec.Append(':');
	searchSpec.Append(KHlpFileSearchPath);
	searchSpec.Append(KHlpFileSpec);

	CDir* dirList;

	TLanguage language(User::Language());
	TFindFile finder(aFsSession);
	TInt ret = finder.FindWildByPath(searchSpec, NULL, dirList);
	if	(ret < KErrNone)
		{
		if	(ret == KErrNotFound)
			return list;
		else
			User::Leave(ret);
		}
	CleanupStack::PushL(dirList);

	// Add files to help file list
	TInt count = dirList->Count();
	for(TInt i=0; i<count; i++)
		{
		CHlpFileEntry* entry = CHlpFileEntry::NewLC(aDrive, (*dirList)[i].iName, language);
		__PRINT_FILE_NO(_L("Found entry during dir scan"), *entry, i);

		// At this point we now check to see if there is already a file with the same name
		// but a different extension already present in the array. If there is already an
		// entry with a lower 'rating' (e.g. ESecondary) than 'entry' we remove the old one
		// and add the new, otherwise, we just ignore the new entry as it's not of sufficient
		// 'quality.'
		TBool foundMatch = EFalse;
		
		TInt existingEntryCount = list->Count();
		for(TInt j=existingEntryCount-1; j>=0; j--)
			{
			CHlpFileEntry* existingEntry = list->At(j);
			
			// Is the file already present in the array?
			if	(existingEntry->Name().CompareF(entry->Name()) == 0)
				{
				foundMatch = ETrue;
				__PRINT_FILE(_L("Entry name duplicate"), *entry);

				// Names are the same but extensions differ in some way...
				if	(entry->Type() < existingEntry->Type())
					{
					__PRINT_FILE(_L("New entry is better than the existing entry with the same name.\n Replacing existing entry with:"), *entry);
					// The new entry is better than the existing one, so delete the
					// existing entry and append the new one
					list->Delete(j);
					delete existingEntry;
					list->AppendL(entry);
					break;
					}
				else
					{
					// The entry is of lower 'quality' than the existing entries
					// so we destroy it
					__PRINT_FILE(_L("Discarding low quality entry"), *entry);
					CleanupStack::PopAndDestroy(); // entry
					break;
					}
				}
			}

		// If the file was not present in the existing array then we're free to
		// add it to the array. Otherwise, we assume that it's been deleted or added
		// as appropriate by the looping code above.
		if	(!foundMatch)
			{
			list->AppendL(entry);
			CleanupStack::Pop(); // entry
			}
		}
	CleanupStack::PopAndDestroy(); // dirList

	return list;
	}


static TBool DiskPresent(TInt aDrive)
	{
	TDriveList list;
	if	(TheFs.DriveList(list) < KErrNone)
		return EFalse;

	TDriveInfo info;
	TInt error = TheFs.Drive(info, aDrive);
	if	(error < KErrNone)
		return EFalse;

	return (list[aDrive] && info.iType != EMediaNotPresent);
	}


static void TestL()
	{
	CHlpFileList* masterList = new(ELeave) CHlpFileList(5);
	CleanupStack::PushL(masterList);

	TInt i = EDriveY;
	FOREVER
		{
		// Make sure we go from Y -> C, then do Z last
		if	(i < EDriveC)
			i = EDriveZ;
		
		if	(DiskPresent(i))
			{

			{
			TChar driveLetter = '?';
			RFs::DriveToChar(i, driveLetter);
			TheTest.Printf(_L("\n\nDisk %c: is installed..."), static_cast<TUint>(driveLetter));
			}

			// This generates a list for a specific directory on the specified drive.
			CHlpFileList* list = BuildListForDriveLC(TDriveUnit(i), TheFs);

			// We now compare this list with our 'master' list to check for duplicates,
			// better matches, etc.
			TInt masterCount = masterList->Count();
			for(TInt j=masterCount-1; j>=0; j--)
				{
				CHlpFileEntry* entry = masterList->At(j);

				// This bit checks to see if an entry in the master list 
				// collides with an entry in the list we have just generated...
				//TBool foundCollision = EFalse;
				TInt newListCount = list->Count();
				for(TInt k=newListCount-1; k>=0; k--)
					{
					CHlpFileEntry* newEntry = list->At(k);
					__PRINT_FILE_NO(_L("Performing drive resolution on entry"), *newEntry, k);

					if	(entry->Name().CompareF(newEntry->Name()) == 0)
						{
						// Names are the same, so compare priorities...
						if	(newEntry->Type() == entry->Type())
							{
							// A file with the same name AND extension is already present in the array,
							// but are the UID's the same? If they are NOT, then the file
							// should be treated as a different help file to that of the 
							// existing array entry.
							TFileName file;

							TEntry existingTEntry;
							entry->GetFullNameAndPath(file);
							User::LeaveIfError(TheFs.Entry(file, existingTEntry));

							TEntry newTEntry;
							newEntry->GetFullNameAndPath(file);
							User::LeaveIfError(TheFs.Entry(file, newTEntry));

							if	(existingTEntry.MostDerivedUid() == newTEntry.MostDerivedUid())
								{
								// The uids and names of the files are the same. If the extensions are also the same
								// then we load the file on the drive tending to C:. However, if the extensions differ
								// we load the file with the extension tending to 01, i.e. UK English.
								if	(entry->FileName().CompareF(newEntry->FileName()) == 0)
									{
									// Name, uid and extensions are the same, therefore load the file on the drive
									// nearest C:. We do this by setting the priority of the existing entry to
									// EDsicarded so that it will force the next 'if' statement to fire, and hence
									// the existing entry will be replaced by the new one.
									entry->SetType(CHlpFileEntry::EDiscarded);
									__PRINT_FILE(_L("Uid, name, extension match. Saving entry tending to C: which is"), *newEntry);
									}
								else
									{
									// Name and uid the same, extensions different therefore load the file with
									// extension nearest to 01. If the new entry is nearer 01, then we set the
									// existing entry to discarded and therefore it will be removed at the next
									// 'if' statement.
									if	(entry->HelpFileLocale() > newEntry->HelpFileLocale())
										{
										entry->SetType(CHlpFileEntry::EDiscarded);
										__PRINT_FILE(_L("Uid & name match, extensions differ.\n Saving entry tending to H01 which is"), *newEntry);
										}
									else
										{
										__PRINT_FILE(_L("Uid & name match, extensions differ.\n Discarding new entry because it tends to H99"), *newEntry);
										}
									}
								}
							else
								{
								// This entry should be treated as a separate entity from the current
								// existing entry because it has different uid's. Therefore, we just
								// move on to check against the next existing entry in the list
								__PRINT_FILE(_L("Duplicate name and extension, but different uids detected. New entry == "), *newEntry);
								continue;
								}
							}

						// Is the new entry a better match than the existing one?
						if	(newEntry->Type() < entry->Type())
							{
							// The new entry is of a better 'quality' than the existing
							// entry, so remove the old and add the new...
							__PRINT_FILE(_L("Removing stored (new better match found)"), *entry);

							//foundCollision = ETrue;
							masterList->Delete(j);
							delete entry;
							masterList->AppendL(newEntry);

							// Remove it from the new list as the master list now takes
							// ownership...
							list->Delete(k);
							break;
							}
						else
							{
							// This entry is of lower quality than an existing
							// entry in the master list, so we can safely discard it.
							delete newEntry;
							list->Delete(k);
							}
						continue;
						}
					}
				}

			// At this point, anything that is left int the new list should
			// be valid to add to the master list...
			TInt newListCount = list->Count();
			for(TInt k=newListCount-1; k>=0; k--)
				{
				__PRINT_FILE_NO(_L("Saving entry"), *list->At(k), k);
				masterList->AppendL(list->At(k));
				}

			// We only destroy the list, rather than list+contents because each element
			// in the new list is now guaranteed to be present in the master list.
			CleanupStack::PopAndDestroy(); // list
			}

		if	(i-- == EDriveZ)
			break;
		}


	// Print out the master list
	TheTest.Printf(_L("\n\nThe help files that would be loaded are:-"));
	TheTest.Printf(_L("\n========================================="));
	TInt masterCount = masterList->Count();
	for(i=0; i<masterCount; i++)
		__PRINT_FILE_NO(_L("Entry"), *masterList->At(i), i);

	masterList->ResetAndDestroy();
	CleanupStack::PopAndDestroy(); // masterList
	}

/**
@SYMTestCaseID PIM-TLOADER-0001 
*/

GLDEF_C TInt E32Main()
//
// Test Help file loading
//
    {
	__UHEAP_MARK;

	TheTest.Title();
	TheTest.Start(_L("@SYMTestCaseID PIM-TLOADER-0001"));
	TheTest(TheFs.Connect() == KErrNone);


	TheTrapCleanup = CTrapCleanup::New();
	if	(!TheTrapCleanup)
		return KErrNoMemory;

	TRAPD(r, TestL());
	TheTest(r == KErrNone);

	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
