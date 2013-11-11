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

#ifndef METADATAREADER_H_
#define METADATAREADER_H_

#include <e32base.h>
#include <mdaaudiosampleplayer.h>
#include "ShowInfo.h"
#include "MetaDataReaderObserver.h"

class CCnvCharacterSetConverter;
#define KMetaDataBufferLength  1024
class CMetaDataReader : public CBase, public MMdaAudioPlayerCallback
{
public:
	CMetaDataReader(MMetaDataReaderObserver &aObserver, RFs& aFs);
	void SubmitShowL(CShowInfo *aShowInfo);
	virtual ~CMetaDataReader();
	void ConstructL();
	void SetIgnoreTrackNo(TBool aIgnoreTrackNo);
	
protected:
	void MapcPlayComplete(TInt aError);
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
	static TInt ParseNextShowCallbackL(TAny* aMetaDataReader);	
private:
	void ParseNextShowL();
	
private:
	CMdaAudioPlayerUtility *iPlayer;
	RShowInfoArray iShowsToParse;
	CShowInfo *iShow;
	MMetaDataReaderObserver &iObserver;
	TBuf8<KMaxFileName> iTempFileName;	
	TBuf8<KMetaDataBufferLength> iTempDataBuffer;
	TBuf<KMetaDataBufferLength> iStringBuffer;
	TBuf<KMetaDataBufferLength> iStringBuffer2;
	CAsyncCallBack* iParseNextShowCallBack;
	CCnvCharacterSetConverter* iCharConverter;
	TUint iLastConverterCharset;
	TBool iIgnoreTrackNo;
	RFs &iFs;
};

#endif /*METADATAREADER_H_*/
