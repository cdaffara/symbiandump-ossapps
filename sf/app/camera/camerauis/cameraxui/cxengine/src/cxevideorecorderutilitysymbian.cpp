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
* Description:
*
*/
#include "cxevideorecorderutilitysymbian.h"
#include "cxenamespace.h"
#include "cxutils.h"
#include <AudioPreference.h>

namespace
{
    // Controller UId, can be used by the client to identify the controller, e.g. if the custom command can be used
    const TUid KCamCControllerImplementationUid = {0x101F8503};

    // Custom command for setting a new filename without closing & reopening the controller
    enum TCamCControllerCustomCommands
        {
        ECamCControllerCCNewFilename = 0,
        ECamCControllerCCVideoStopAsync
        };

    const uint KOneMillion = 1000000;
}


CxeVideoRecorderUtilitySymbian::~CxeVideoRecorderUtilitySymbian()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mVideoRecorder;
    mVideoRecorder = NULL;
    CX_DEBUG_EXIT_FUNCTION();
}

CxeVideoRecorderUtilitySymbian::CxeVideoRecorderUtilitySymbian(MVideoRecorderUtilityObserver& aObserver)
{
    CX_DEBUG_ENTER_FUNCTION();
    QT_TRAP_THROWING(mVideoRecorder =
            CVideoRecorderUtility::NewL(aObserver,
                                        KAudioPriorityVideoRecording,
                                        TMdaPriorityPreference(KAudioPrefVideoRecording)));
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Open new file for recording.
* @param cameraHandle Handle for camera.
* @param filename Full filename of the video file to open.
* @param fileMimeType MIME type for the video file.
* @param supplier Preferred supplier.
* @param videoType Video codec MIME type.
* @param audioType Audio FourCC type.
*/
void CxeVideoRecorderUtilitySymbian::open(int cameraHandle,
                                          const QString &filename,
                                          const QString &fileMimeType,
                                          const QString &supplier,
                                          const QString &videoType,
                                          const QString &audioType)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeVideoRecorderUtilitySymbian - filename     [%s]", filename.toAscii().constData()));
    CX_DEBUG(("CxeVideoRecorderUtilitySymbian - file mime    [%s]", fileMimeType.toAscii().constData()));
    CX_DEBUG(("CxeVideoRecorderUtilitySymbian - supplier     [%s]", supplier.toAscii().constData()));
    CX_DEBUG(("CxeVideoRecorderUtilitySymbian - codec mime   [%s]", videoType.toAscii().constData()));
    CX_DEBUG(("CxeVideoRecorderUtilitySymbian - audio FourCC [%s]", audioType.toAscii().constData()));

    QByteArray bytes = videoType.toLatin1();
    TPtrC8 videoTypeDesC(reinterpret_cast<const TUint8*>(bytes.constData()), bytes.size());
    TPtrC16 filenameDesC(reinterpret_cast<const TUint16*>(filename.utf16()));

    TUid controllerId;
    TUid formatId;
    QT_TRAP_THROWING({
        // Find the video controller id and video format id.
        findControllerL(fileMimeType, supplier, controllerId, formatId);

        // Try to open new video file with given parameters.
        mVideoRecorder->OpenFileL(filenameDesC, cameraHandle, controllerId,
                                  formatId, videoTypeDesC, audioFourCC(audioType));
    });
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set frame size.
* @param size The video frame size.
*/
void CxeVideoRecorderUtilitySymbian::setVideoFrameSize(const QSize &size)
{
    CX_DEBUG_ENTER_FUNCTION();
    TSize frameSize(size.width(), size.height());
    QT_TRAP_THROWING(mVideoRecorder->SetVideoFrameSizeL(frameSize));
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set video frame rate.
* @param rate The video frame rate.
*/
void CxeVideoRecorderUtilitySymbian::setVideoFrameRate(int rate)
{
    CX_DEBUG_ENTER_FUNCTION();
    QT_TRAP_THROWING(mVideoRecorder->SetVideoFrameRateL(rate));
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set video bit rate.
* @param The video bit rate.
*/
void CxeVideoRecorderUtilitySymbian::setVideoBitRate(int rate)
{
    CX_DEBUG_ENTER_FUNCTION();
    QT_TRAP_THROWING(mVideoRecorder->SetVideoBitRateL(rate));
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set if audio recording is on or not.
* @param enabled Is audio recording enabled.
*/
void CxeVideoRecorderUtilitySymbian::setAudioEnabled(bool enabled)
{
    CX_DEBUG_ENTER_FUNCTION();
    QT_TRAP_THROWING(mVideoRecorder->SetAudioEnabledL(enabled));
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set maximum video clip size in bytes.
* @param sizeInBytes Video clip size limit.
*/
void CxeVideoRecorderUtilitySymbian::setVideoMaxSize(int sizeInBytes)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (sizeInBytes <= 0) {
        sizeInBytes = KMMFNoMaxClipSize;
    }
    QT_TRAP_THROWING(mVideoRecorder->SetMaxClipSizeL(sizeInBytes));
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Close video recorder, freeing its resources.
*/
void CxeVideoRecorderUtilitySymbian::close()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->Close();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Prepare for video recording.
*/
void CxeVideoRecorderUtilitySymbian::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->Prepare();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Start recording.
*/
void CxeVideoRecorderUtilitySymbian::record()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->Record();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Stop recording.
* @param asynchronous Use asynchronous (true) or synchronous (false) stopping.
*/
void CxeVideoRecorderUtilitySymbian::stop(bool asynchronous)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (asynchronous) {
        TMMFMessageDestination dest(KCamCControllerImplementationUid, KMMFObjectHandleController);
        qt_symbian_throwIfError(mVideoRecorder->CustomCommandSync(
                                    dest, ECamCControllerCCVideoStopAsync, KNullDesC8, KNullDesC8));
    } else {
        qt_symbian_throwIfError(mVideoRecorder->Stop());
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Pause recording.
*/
void CxeVideoRecorderUtilitySymbian::pause()
{
    CX_DEBUG_ENTER_FUNCTION();
    QT_TRAP_THROWING(mVideoRecorder->PauseL());
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get estimated available recording time.
* @return Available recording time estimate in seconds.
*/
int CxeVideoRecorderUtilitySymbian::availableRecordingTime()
{
    CX_DEBUG_IN_FUNCTION();
    // Convert microseconds to seconds.
    return mVideoRecorder->RecordTimeAvailable().Int64() * 1.0 / KOneMillion;
}

/*!
* Get elapsed duration of the recorded video.
* @return Duration of the video in seconds.
*/
int CxeVideoRecorderUtilitySymbian::duration()
{
    CX_DEBUG_ENTER_FUNCTION();
    int time(0);
    // Convert microseconds to seconds.
    QT_TRAP_THROWING(time = mVideoRecorder->DurationL().Int64() * 1.0 / KOneMillion);
    CX_DEBUG_EXIT_FUNCTION();
    return time;
}

/*!
* Find video controller and format UIDs based on file mime type and preferred supplier.
* @param fileMimeType MIME type for the video file.
* @param supplier Preferred supplier.
* @param controllerId Returns found controller UID.
* @param formatId Returns found format UID.
*/
void CxeVideoRecorderUtilitySymbian::findControllerL(const QString& fileMimeType,
                                                     const QString& supplier,
                                                     TUid& controllerId,
                                                     TUid& formatId)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Retrieve a list of possible controllers from ECOM.
    // Controller must support recording the requested mime type.
    // Controller must be provided by preferred supplier.
    controllerId = KNullUid;
    formatId = KNullUid;

    QByteArray bytes = fileMimeType.toLatin1();
    TPtrC8 mimeTPtr(reinterpret_cast<const TUint8*>(bytes.constData()), bytes.size());
    TPtrC16 supplierTPtr(reinterpret_cast<const TUint16*>(supplier.utf16()));

    CMMFControllerPluginSelectionParameters* cSelect(NULL);
    CMMFFormatSelectionParameters* fSelect(NULL);
    RMMFControllerImplInfoArray controllers;

    cSelect = CMMFControllerPluginSelectionParameters::NewLC();
    fSelect = CMMFFormatSelectionParameters::NewLC();

    fSelect->SetMatchToMimeTypeL(mimeTPtr);
    cSelect->SetRequiredRecordFormatSupportL(*fSelect);
    cSelect->SetPreferredSupplierL(supplierTPtr,
                                   CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);
    cSelect->ListImplementationsL(controllers);
    CleanupResetAndDestroyPushL(controllers);

    if (controllers.Count() <= 0) {
        CX_DEBUG(("CxeVideoRecorderUtilitySymbian - zero controllers found, leaving!"));
        User::Leave(KErrNotFound);
    }

    // Inquires the controller about supported formats.
    // We use the first controller found having index 0.
    const RMMFFormatImplInfoArray& formats = controllers[0]->RecordFormats();

    // Get the first format that supports our mime type.
    int count(formats.Count());
    for (int i=0; i<count; i++) {
        if (formats[i]->SupportsMimeType(mimeTPtr)) {
            CX_DEBUG(("CxeVideoRecorderUtilitySymbian - found controller"));
            // Store the format UID
            formatId = formats[i]->Uid();
            // Store the controller UID
            controllerId = controllers[0]->Uid();
            break;
        }
    }

    // Check that we found the uids.
    if (controllerId == KNullUid) {
        CX_DEBUG(("CxeVideoRecorderUtilitySymbian - no matching controller found, leaving!"));
        User::Leave(KErrNotFound);
    }

    CleanupStack::PopAndDestroy(3); // controllers, fSelect, cSelect
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method to convert QString to TFourCC.
* @param str String to convert.
*/
TFourCC CxeVideoRecorderUtilitySymbian::audioFourCC(const QString& str)
{
    CX_DEBUG_ENTER_FUNCTION();

    QByteArray audioType = str.toAscii();

    quint8 char1(' ');
    quint8 char2(' ');
    quint8 char3(' ');
    quint8 char4(' ');

    if (audioType.count() > 3) {
        char1 = audioType[0];
        char2 = audioType[1];
        char3 = audioType[2];

        if (audioType.count() == 4) {
            char4 = audioType[3];
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return TFourCC(char1, char2, char3, char4);
}


// end of file
