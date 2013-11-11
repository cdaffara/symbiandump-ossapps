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

#ifndef SHOWINFO_H_
#define SHOWINFO_H_
#include <e32base.h>
#include <e32cmn.h>
#include <e32std.h>
#include <f32file.h>
#include <s32strm.h>

const int KShowInfoVersion = 8;

enum TPlayState {
	ENeverPlayed,
	EPlayed,
	EPlaying
};

enum TDownloadState {
	ENotDownloaded,
	EQueued,
	EDownloading,
	EFailedDownload,
	EDownloaded
};

enum TShowType {
	EAudioPodcast = 0,
	EVideoPodcast,
	EOtherPodcast
};

class CShowInfo: public CBase {
public:
	IMPORT_C static CShowInfo* NewL(TUint aVersion=KShowInfoVersion);
	IMPORT_C static CShowInfo* NewLC(TUint aVersion=KShowInfoVersion);
	~CShowInfo();
	IMPORT_C CShowInfo(CShowInfo *aInfo); // copy constructor

public:
	IMPORT_C const TDesC& Title() const;
	IMPORT_C void SetTitleL(const TDesC& aTitle);

	IMPORT_C const TDesC& Url() const;
	IMPORT_C void SetUrlL(const TDesC &aUrl);
	
	IMPORT_C const TDesC& Description() const;
	IMPORT_C void SetDescriptionL(const TDesC &aDescription);
		
	IMPORT_C TTimeIntervalMicroSeconds Position() const;
	IMPORT_C void SetPosition(TTimeIntervalMicroSeconds aPosition);

	IMPORT_C TUint PlayTime() const;
	IMPORT_C void SetPlayTime(TUint aPlayTime);

	IMPORT_C TPlayState PlayState() const;
	IMPORT_C void SetPlayState(TPlayState aPlayState);

	IMPORT_C TDownloadState DownloadState() const;
	IMPORT_C void SetDownloadState(TDownloadState aDownloadState);

	IMPORT_C TUint FeedUid() const;
	IMPORT_C void SetFeedUid(TUint aFeedUid);

	IMPORT_C void SetUid(TUint aUid);
	IMPORT_C TUint Uid() const;
	
	IMPORT_C TUint ShowSize() const;
	IMPORT_C void SetShowSize(TUint aShowSize);
	
	IMPORT_C const TTime PubDate() const;
	IMPORT_C void SetPubDate(TTime aPubDate);
	
	IMPORT_C const TDesC &FileName() const;
	IMPORT_C void SetFileNameL(const TDesC &aFileName);
	
	IMPORT_C void SetShowType(TShowType aShowType);
	IMPORT_C TShowType ShowType() const;

	IMPORT_C void SetTrackNo(TUint aTrackNo);
	IMPORT_C TUint TrackNo() const;
	
	IMPORT_C void SetLastError(TInt aLastError);
	IMPORT_C TInt LastError() const;
private:
	CShowInfo();
	void ConstructL();

private:
	HBufC* iTitle;
	HBufC* iUrl;
	HBufC* iDescription;
	HBufC* iFileName;
	TTimeIntervalMicroSeconds iPosition;
	TUint   iPlayTime;
	TPlayState iPlayState;
	TDownloadState iDownloadState;
	TUint iFeedUid;
	TUint iUid;
	TUint iShowSize;
	TUint iTrackNo;
	TTime iPubDate;
	TBool iDelete;
	TShowType iShowType;
	TInt  iLastError;
};

typedef RPointerArray<CShowInfo> RShowInfoArray;
#endif
