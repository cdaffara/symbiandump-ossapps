/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxeimagedataitemsymbian.h"
#include "cxeerrormappingsymbian.h"
#include "cxesysutil.h"
#include "cxutils.h"
#include "cxestate.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxeimagedataitemsymbianTraces.h"
#endif

/*!
* Constructor.
*/
CxeImageDataItemSymbian::CxeImageDataItemSymbian(QByteArray data,
                                                 QString filename,
                                                 int id,
                                                 bool addLocation,
                                                 CxeImageDataItem::State state)
  : CxeStateMachine("CxeImageDataItemSymbian"),
    mId(id),
    mData(data),
    mAddLocationInfo(addLocation),
    mPath(filename)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeImageDataItem::State>();
    qRegisterMetaType<CxeError::Id>();
    // Init mState
    initializeStates();
    setInitialState(state);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor.
*/
CxeImageDataItemSymbian::~CxeImageDataItemSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();
    closeHandles();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Save the data now.
* @return Status code.
*/
CxeError::Id CxeImageDataItemSymbian::save()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id status(CxeError::None);
    try {
        setState(CxeImageDataItem::Saving);
        trySave();
        setState(CxeImageDataItem::Saved);
    } catch (const std::exception &e) {
        closeHandles();
        status = CxeErrorHandlingSymbian::map(qt_symbian_exception2Error(e));
        setState(CxeImageDataItem::SaveFailed);
    }

    emit imageSaved(status, mPath, mId);

    CX_DEBUG_EXIT_FUNCTION();
    return status;
}

/*!
* Helper method for trying to save the data.
* If any error is encountered during the saving process, exception is thrown.
*/
void CxeImageDataItemSymbian::trySave()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEIMAGEDATAITEMSYMBIAN_SAVE_IN, "msg: e_CX_IMAGEDATAITEM_SAVE 1");

    CX_DEBUG(("CxeImageDataItemSymbian - Starting to save [%s]", qPrintable(mPath)));

    // Check we have the path set.
    if (mPath.isEmpty()) {
        CX_DEBUG(("CxeImageDataItemSymbian - Filename not set!"));
        qt_symbian_throwIfError(KErrArgument);
    }

    TPtrC16 filename;
    filename.Set(reinterpret_cast<const TUint16*>(mPath.utf16()));
    // Init
    CX_DEBUG(("CxeImageDataItemSymbian - connect to RFs.."));
    qt_symbian_throwIfError(mFs.Connect());

    // Get drive number
    TInt drive = 0;
    CX_DEBUG(("CxeImageDataItemSymbian - Get drive number.."));
    qt_symbian_throwIfError(RFs::CharToDrive(filename[0], drive));

    // Check disk has space
    bool fullDisk = checkDiskSpace(&mFs, mData.size(), drive);
    if (fullDisk) {
        CX_DEBUG(("CxeImageDataItemSymbian - Disk is full!"));
        qt_symbian_throwIfError(KErrDiskFull);
    }

    // Attempt to create the file
    // Note: In sake of MDS not starting harvesting here,
    // do not use RFile::Replace. If harvesting is started now,
    // our later call to harvest may be ignored and
    // file may be missing from "Captured" album.
    CX_DEBUG(("CxeImageDataItemSymbian - Create the file.."));
    qt_symbian_throwIfError(mFile.Create(mFs, filename, EFileWrite));

    // Write data to the file.
    CX_DEBUG(("CxeImageDataItemSymbian - Starting to write the file.."));
    TPtrC8 data(reinterpret_cast<const TUint8*> (mData.constData()), mData.size());
    qt_symbian_throwIfError(mFile.Write(data)); // synchronous

    // Flush all the data to file now.
    // This may take a while depending on buffer sizes and file server load.
    OstTrace0(camerax_performance, CXEIMAGEDATAITEMSYMBIAN_FLUSH_1, "msg: e_CX_SAVE_FLUSH_FILE 1");
    qt_symbian_throwIfError(mFile.Flush());
    OstTrace0(camerax_performance, CXEIMAGEDATAITEMSYMBIAN_FLUSH_2, "msg: e_CX_SAVE_FLUSH_FILE 0");

    // Close the file and server handles.
    closeHandles();
    CX_DEBUG(("CxeImageDataItemSymbian - Saving to file completed.."));

    OstTrace0(camerax_performance, CXEIMAGEDATAITEMSYMBIAN_SAVED, "msg: e_CX_SHOT_TO_SAVE 0");
    OstTrace0(camerax_performance, CXEIMAGEDATAITEMSYMBIAN_SAVE_OUT, "msg: e_CX_IMAGEDATAITEM_SAVE 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get the id number of this data item.
*/
int CxeImageDataItemSymbian::id() const
{
    return mId;
}

/*!
* Get the path of this data item.
*/
QString CxeImageDataItemSymbian::path() const
{
    return mPath;
}

/*!
* Check that there's enough space in the drive.
* @param aFs File server session
* @param aBytesToWrite Amount of bytes to be written.
* @param aDrive Drive number.
* @return True, if given amount of bytes can be written to the drive, false otherwise.
*/
bool CxeImageDataItemSymbian::checkDiskSpace(RFs* aFs,
        TInt aBytesToWrite,
        TInt aDrive)
{
    CX_DEBUG_ENTER_FUNCTION();
    bool value = CxeSysUtil::DiskSpaceBelowCriticalLevel(
            aFs,
            aBytesToWrite,
            aDrive );
    return value;
}

/*!
* State of this item.
* @return The state.
*/
CxeImageDataItem::State CxeImageDataItemSymbian::state() const
{
    return static_cast<CxeImageDataItem::State> (stateId());
}

/*!
* Handle state change.
*/
void CxeImageDataItemSymbian::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State> (newStateId), error);
}

/*!
* Is location tagging enabled for this item.
* @return True if location tagging is enabled, false otherwise.
*/
bool CxeImageDataItemSymbian::isLocationEnabled() const
{
    return mAddLocationInfo;
}

/*!
* Init the state machine.
*/
void CxeImageDataItemSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState(SavePending, "SavePending", Saving | SaveFailed));
    addState(new CxeState(Saving, "Saving", Saved | SaveFailed));
    addState(new CxeState(Saved, "Saved", 0));
    addState(new CxeState(SaveFailed, "SaveFailed", 0));
}

/*!
* Close the file server and file handles.
*/
void CxeImageDataItemSymbian::closeHandles()
{
    mFile.Close();
    mFs.Close();
}

// end of file
