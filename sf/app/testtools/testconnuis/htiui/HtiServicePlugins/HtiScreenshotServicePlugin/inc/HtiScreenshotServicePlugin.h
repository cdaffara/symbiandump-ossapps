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
* Description:  Implementation of ECOM plug-in service interface. Provides
*                screenshot service.
*
*/


#ifndef SCREENSHOOTPLUGIN_H
#define SCREENSHOOTPLUGIN_H

// INCLUDES
#include <HtiServicePluginInterface.h>
#include <w32std.h>
#include <f32file.h>
#include "HtiTextRcg.h"


// FORWARD DECLARATIONS
class CImageEncoder;


// CLASS DECLARATIONS
class MICLObserver
    {
public:
    virtual void ICLComplete( TInt anError) = 0;
    };

class CICLHandler : public CActive
    {
public:
    CICLHandler(CImageEncoder* aService, MICLObserver* anObserver);
    ~CICLHandler();

    void Start();

protected: //from CActive
    void RunL();
    void DoCancel();
    //TInt RunError(TInt aError);

protected:
    MICLObserver* iObserver;
    CImageEncoder* iService;
    };





class MSeriesShotTimerObserver
    {
public:
    virtual void TimerExpired( TInt aId ) = 0;
    };



class CSeriesShotTimer : public CTimer
    {
public:
    static CSeriesShotTimer* NewL(MSeriesShotTimerObserver* aObserver,
                                 TInt aId,
                                 TTimeIntervalMicroSeconds32 aTime);
    ~CSeriesShotTimer();
    void Start();

protected:
    void ConstructL();
    CSeriesShotTimer(MSeriesShotTimerObserver* aObserver,
                    TInt aId,
                    TTimeIntervalMicroSeconds32 aTime);
public: // from CTimer
    void RunL();

private:
    MSeriesShotTimerObserver* iObserver;
    TInt iId;
    TTimeIntervalMicroSeconds32 iTime;
    };


class MSeriesShotObserver
    {
public:
    virtual void SeriesShotCompletedL( HBufC8* aMsg ) = 0;
    virtual TBool StartShotL(TRect aRegion, TDisplayMode aDisplayMode, TDesC8 &aMimeType) = 0;
    };


class CHtiScreenshotServicePlugin; // forward declaration

class CSeriesShot : public CBase,
                    public MSeriesShotTimerObserver
    {
    enum TTimerType
        {
        EDuration,
        EInterval
        };
public:
    static CSeriesShot* NewL( MSeriesShotObserver* aServicePluginObserver );
    CSeriesShot( MSeriesShotObserver* aServicePluginObserver );
    virtual ~CSeriesShot();

    void StartL( TTimeIntervalMicroSeconds32 aDuration,
                 TTimeIntervalMicroSeconds32 aInterval,
                 TDisplayMode aDisplayMode,
                 TRect aRegion,
                 TPtrC8 aMime);
    TBool IsOngoing();
    void SaveImage( TDesC8* aImage, TBool isCompressed );
    void TriggerNewShot();
    void Cancel();
    void EncodeCompleted();
    HBufC8* ConstructCompletedMessageL();

protected:
    void ConstructL();
    void ClearShots();
    void GetMIMEExtension(TDesC8 &aMime, TDes &aExt);

public: // from MSeriesShotTimerObserver
    void TimerExpired(TInt aId);

private:
    MSeriesShotObserver* iServicePluginObserver;
    CSeriesShotTimer* iDurationTimer;
    CSeriesShotTimer* iIntervalTimer;
    TDisplayMode iDisplayMode;
    TInt iIndex;
    RFs iFs;
    TBool isEncoding;
    TBuf8<30> iMimeType;
    TBuf<10>  iExtension;
    TRect iRegion;
    };



class CHtiScreenshotServicePlugin : public CHTIServicePluginInterface,
                                    public MICLObserver,
                                    public MSeriesShotObserver
    {
public:

    static CHtiScreenshotServicePlugin* NewL();

    // Interface implementation
    TBool IsBusy();
    void ProcessMessageL(const TDesC8& aMessage, THtiMessagePriority aPriority);
    void NotifyMemoryChange( TInt aAvailableMemory );

    // Observer implementation
    void ICLComplete( TInt anError); // from MICLObserver
    void SeriesShotCompletedL( HBufC8* aMsg ); // from MSeriesShotObserver
    TBool StartShotL(TRect aRegion, TDisplayMode aDisplayMode, TDesC8 &aMimeType); // from MSeriesShotObserver

protected:

    void ProcessTextRcgMessageL(const TDesC8& aMessage);
    void ProcessTextBitmapMessageL(const TDesC8& aMessage);

    void SendTextRecgReplyL(const TBool aResult, const TRect& aLocation,
                            const TInt aFontIndex);


    /**
    * Selects fonts based on predefined platform-dependent strategy
    *
    */
    TBool RecognizeTextL(const TDesC& aText,
                        TRect& aResult,
                        TInt& aFontIndex);

    /**
    * Extracts string from incoming request and convert it to unicode
    * for non-unicode request
    * aResult should have enough length
    * Function returns either offset for a next parameter in aRequest
    * or some symbian error code
    */
    TInt ParseString( const TDesC8& aRequest,
                        TInt anOffset,
                        TBool aUnicode,
                        TDes& aResult);

    void CopyUnicode( TDes & aTo, const TDesC8& aFrom );

    /**
    * Extract from request font description (font name, height, style)
    */
    TInt ParseFontSpec( const TDesC8& aRequest,
                        TInt anOffset,
                        TBool aUnicode,
                        TFontSpec& aResult);

    CWsScreenDevice* GetScreenDeviceL();

    void CreateBitmapL( TRect& aRegion, TDisplayMode aMode = ENone);

    void SelectEncoder( const TUid aEncoderUid );

    //encode iScreen
    void EncodeBitmapL(const TDesC8& aImageTypeMIME = KNullDesC8);

    inline TInt ParseInt16( const TUint8* aStart );
    inline TInt ParseInt32( const TUint8* aStart );

    /**
    * Compress content of iEncodedBitmap descriptor
    */
    TInt Compress();

    CHtiScreenshotServicePlugin();
    void ConstructL();

    virtual ~CHtiScreenshotServicePlugin();

    //void InitFontCache();

    TBool IsMIMETypeSupported(TDesC8 &aMime);
    
    void SetScreenNumber(TInt aScreenNumber);
    
    void HandleRotateScreen(const TDesC8& aData);
protected:
    CFbsBitmap* iScreen; //raw screen bitmap
    HBufC8* iEncodedBitmap; //ready to send

    RWsSession iWs;
    CWsScreenDevice* iScreenDevice;

    CImageEncoder* iBitmapEncoder;
    CICLHandler* iICLHandler;

    TBool iCompress;

    //text recognition algorithms
    CHtiTextRcg iTextRcg;
    //fonts to use for recognition
    RArray<TFontSpec> iFontCache;

    // Series shot implementation
    CSeriesShot*    iSeriesShot;

    // Members for deltacapture
    TBool       iDeltaCapture;
    CFbsBitmap* iPreviousBitmap;
    TRect       iDeltaRect;
    };

#endif // SCREENSHOOTPLUGIN_H
