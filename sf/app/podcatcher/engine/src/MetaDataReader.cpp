/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "MetaDataReader.h"
#include <utf.h>
#include <charconv.h>
#include <eikenv.h>
#include <mmf\common\mmfMeta.h>
#include "debug.h"

const TInt KMaxParseBuffer = 1024;

CMetaDataReader::CMetaDataReader(MMetaDataReaderObserver& aObserver, RFs& aFs) : iObserver(aObserver), iFs(aFs)
{
	iShow = NULL;
}

CMetaDataReader::~CMetaDataReader()
{
	if( iPlayer )
		{
		iPlayer->Close();
		delete iPlayer;
		}
	
	delete iParseNextShowCallBack;
	iShowsToParse.Close();
	delete iCharConverter;
}

void CMetaDataReader::ConstructL()
{
	DP("MetaDataReader::ConstructL");
	iPlayer = CMdaAudioPlayerUtility::NewL(*this);

	TCallBack callback(ParseNextShowCallbackL, this);
	iParseNextShowCallBack = new (ELeave)CAsyncCallBack(callback, CActive::EPriorityStandard);
	iCharConverter = CCnvCharacterSetConverter::NewL();
	iCharConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierIso88591, iFs); 
	iLastConverterCharset = KCharacterSetIdentifierIso88591;
}

void CMetaDataReader::SubmitShowL(CShowInfo *aShowInfo)
{
	DP("SubmitShow");
	iShowsToParse.Append(aShowInfo);
	
	if (iShow == NULL) {
		ParseNextShowL();
	}
}

TInt CMetaDataReader::ParseNextShowCallbackL(TAny* aMetaDataReader)
{
	static_cast<CMetaDataReader*>(aMetaDataReader)->ParseNextShowL();
	return KErrNone;
}

void CMetaDataReader::ParseNextShowL()
	{
	DP("ParseNextShow");
	iPlayer->Close();
	if (iShowsToParse.Count() == 0) {
		DP("No more shows, stopping");
		iObserver.ReadMetaDataCompleteL();
		return;
	}
	
	iShow = iShowsToParse[0];
	iShowsToParse.Remove(0);	

	iPlayer->OpenFileL(iShow->FileName());
}

void CMetaDataReader::MapcPlayComplete(TInt /*aError*/)
{
	
}

void CMetaDataReader::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
{
	DP1("MapcInitComplete, file=%S", &(iShow->FileName()));


	if(aError == KErrNone)
	{
		HBufC* buf = HBufC::New(KMaxParseBuffer);
		TInt numEntries = 0;
		if (buf != NULL && iPlayer->GetNumberOfMetaDataEntries(numEntries) == KErrNone) {
			DP1("%d meta data entries", numEntries);
			iShow->SetPlayTime((aDuration.Int64()/1000000));
			
			for (int i=0;i<numEntries;i++) {
				CMMFMetaDataEntry * entry = NULL;
				TRAPD(error, entry = iPlayer->GetMetaDataEntryL(i));
				
				if (error != KErrNone) {
					continue;
				}
											
				if (entry->Name() == KMMFMetaEntrySongTitle) {
					buf->Des().Copy(entry->Value());
					TRAP_IGNORE(iShow->SetTitleL(*buf));
					DP1("title: %S", &(iShow->Title()));
				} else if (entry->Name() == _L("artist")) {
					if (iShow->Description().Length() > 0) {
						buf->Des().Copy(iShow->Description());
					}
					buf->Des().Append(_L("\n"));
					buf->Des().Append(entry->Value());
					
					TRAP_IGNORE(iShow->SetDescriptionL(*buf));
				} else if (entry->Name() == KMMFMetaEntryAlbum) {
					if (iShow->Description().Length() > 0) {
					buf->Des().Copy(iShow->Description());
					}
					buf->Des().Append(_L("\n"));
					buf->Des().Append(entry->Value());
					
					TRAP_IGNORE(iShow->SetDescriptionL(*buf));
				}	
				else if (entry->Name() == KMMFMetaEntryAlbumTrack) {
					TLex lexer(entry->Value());
					TUint value = 0;
					if(lexer.Val(value) == KErrNone) {
						iShow->SetTrackNo(value);
					}
				}
				
			}
		}
		delete buf;
		buf = NULL;
	}

	TRAP_IGNORE(iObserver.ReadMetaDataL(*iShow));
	iPlayer->Stop();
	iShow = NULL;
	TRAP_IGNORE(ParseNextShowL());
}

void CMetaDataReader::SetIgnoreTrackNo(TBool aIgnoreTrackNo)
	{
	iIgnoreTrackNo = aIgnoreTrackNo;
	}
