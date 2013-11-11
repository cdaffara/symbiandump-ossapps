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

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlerrotate.h>
#include <ExifRead.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxcommandhandlerrotateTraces.h"
#endif


GlxCommandHandlerRotate::GlxCommandHandlerRotate()
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_GLXCOMMANDHANDLERROTATE_ENTRY );
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_GLXCOMMANDHANDLERROTATE_EXIT );
    }

GlxCommandHandlerRotate::~GlxCommandHandlerRotate()
    {
    OstTraceFunctionEntry0( DUP1_GLXCOMMANDHANDLERROTATE_GLXCOMMANDHANDLERROTATE_ENTRY );
    OstTraceFunctionExit0( DUP1_GLXCOMMANDHANDLERROTATE_GLXCOMMANDHANDLERROTATE_EXIT );
    }

CMPXCommand* GlxCommandHandlerRotate::CreateCommandL(TInt aCommandId,
        MGlxMediaList& /*aMediaList*/, TBool& aConsume) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_CREATECOMMANDL_ENTRY );
    Q_UNUSED(aCommandId);
    Q_UNUSED(aConsume);
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_CREATECOMMANDL_EXIT );
    return NULL;
    }

void GlxCommandHandlerRotate::DoExecuteCommandL(TInt aCommandId,
        MGlxMediaList& aMediaList, TBool& aConsume)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_DOEXECUTECOMMANDL_ENTRY );
    Q_UNUSED(aCommandId);
    Q_UNUSED(aConsume);
    const TGlxMedia& item = aMediaList.Item(aMediaList.FocusIndex());
    TFileName uri;
    uri.Copy(item.Uri());
    TRAPD(err,RotateImageL(uri));
    if (err != KErrNone)
        {
        OstTrace0( TRACE_NORMAL, GLXCOMMANDHANDLERROTATE_DOEXECUTECOMMANDL, "GlxCommandHandlerRotate::DoExecuteCommandL Exif Update failed" );
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_DOEXECUTECOMMANDL_EXIT );
    }

void GlxCommandHandlerRotate::RotateImageL(TFileName aFileToBeRotated)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_ROTATEIMAGEL_ENTRY );
    //Start an IFS session
    //File system 
    User::LeaveIfError(iFs.Connect());
    //first initialize the Exif Writer which is responsible for reading and writing the Exif Data
    //Will leave here itself in cases where the Exif data is absent
    InitializeExifWriterL(aFileToBeRotated);
    //read the Orientation tag stored in  the Exif Data
    TUint16 initialOrientation = ReadImageOrientationL();
    //as the image is rotated to 90 degrees clockwise calculate the new orientation by adding that angle
    TUint16 finalOrientation = CalculateFinalOrientationL(initialOrientation);
    // Set the Final Orientation on the file
    SetImageOrientationL(finalOrientation);
    // Clear the sessions acquired
    DestroyExifWriter();
    //close the File Session
    iFs.Close();
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_ROTATEIMAGEL_EXIT );
    }

void GlxCommandHandlerRotate::InitializeExifWriterL(
        TFileName aFileToBeRotated)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_INITIALIZEEXIFWRITERL_ENTRY );
    User::LeaveIfError(iFileHandle.Open(iFs, aFileToBeRotated, EFileWrite));
    TInt filesize;
    User::LeaveIfError(iFileHandle.Size(filesize));
    iExifData = HBufC8::NewL(filesize);
    TPtr8 ptr(iExifData->Des());
    User::LeaveIfError(iFileHandle.Read(ptr));
    iExifWriter = CExifModify::NewL(*iExifData, CExifModify::EModify,
            CExifModify::ENoJpegParsing);
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_INITIALIZEEXIFWRITERL_EXIT );
    }

TUint16 GlxCommandHandlerRotate::ReadImageOrientationL()
{
	OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_READIMAGEORIENTATIONL_ENTRY );
	TUint16 initialOrientation;
	const CExifRead* exifReader = iExifWriter->Reader(); //not owned
    User::LeaveIfError(exifReader->GetOrientation(initialOrientation));
	OstTrace1( TRACE_NORMAL, GLXCOMMANDHANDLERROTATE_READIMAGEORIENTATIONL, "GlxCommandHandlerRotate::ReadImageOrientationL;initial orientation=%d", initialOrientation );
	OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_READIMAGEORIENTATIONL_EXIT );
	return (initialOrientation);
}

TUint16 GlxCommandHandlerRotate::CalculateFinalOrientationL(
        TUint16 aInitialOrientation)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_CALCULATEFINALORIENTATIONL_ENTRY );
    /*
     * possible orientation state with angles for rotation
     * Possible Angles 0 - 90 - 180 - 270
     * Possible states 1 - 8 - 3 - 6 without a Flip
     * Possible states 2 - 7 - 4 - 5 when Flip is on
     */
    TUint16 finalOrientation = aInitialOrientation;
    if (aInitialOrientation > 8)
        {
        //invalid orientation passed Leave
        User::Leave(KErrCorrupt);
        }
    TInt rotOffset = 1;
    TInt isOrientationOdd = aInitialOrientation % 2;
    TInt initStateIndex = 0;
    TInt finalStateIndex = 0;
    //Setting the orientation states for the initial unflipped orientation combinations
    TInt orientationStateArray[] =
        {
        1, 8, 3, 6
        };
    //Seting the index for current orientation
    if (aInitialOrientation < 3)
        {
        initStateIndex = 0;
        }
    else if (aInitialOrientation >= 3 && aInitialOrientation < 5)
        {
        initStateIndex = 2;
        }
    else if (aInitialOrientation >= 5 && aInitialOrientation < 7)
        {
        initStateIndex = 3;
        }
    else if (aInitialOrientation >= 7 && aInitialOrientation <= 8)
        {
        initStateIndex = 1;
        }
    //Calculating the final orientation using the cyclic orientationStateArray. 
    //folding final index so that it behaves like a cyclic machine 
    finalStateIndex = (initStateIndex + rotOffset) % 4;
    finalOrientation = orientationStateArray[finalStateIndex];
    //Checking if a Flip was present 
    if (aInitialOrientation > 4 && isOrientationOdd)
        {
        finalOrientation -= 1;
        }
    if (aInitialOrientation < 5 && !isOrientationOdd)
        {
        finalOrientation += 1;
        }
    OstTrace1( TRACE_NORMAL, GLXCOMMANDHANDLERROTATE_CALCULATEFINALORIENTATIONL, "GlxCommandHandlerRotate::CalculateFinalOrientationL;finalOrientation=%d", finalOrientation );
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_CALCULATEFINALORIENTATIONL_EXIT );
    return finalOrientation;
    }

void GlxCommandHandlerRotate::SetImageOrientationL(TUint16 aFinalOrientation)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_SETIMAGEORIENTATIONL_ENTRY );
    iExifWriter->SetOrientationL(aFinalOrientation);
    HBufC8* modifiedexifData = NULL;
    modifiedexifData = iExifWriter->WriteDataL(iExifData->Des());
    User::LeaveIfError(iFileHandle.Write(0, modifiedexifData->Des()));
    delete modifiedexifData;
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_SETIMAGEORIENTATIONL_EXIT );
    }

void GlxCommandHandlerRotate::DestroyExifWriter()
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERROTATE_DESTROYEXIFWRITER_ENTRY );
    iFileHandle.Close();
    if (iExifData != NULL)
        {
        delete iExifData;
        iExifData = NULL;
        }
    if (iExifWriter != NULL)
        {
        delete iExifWriter;
        iExifWriter = NULL;
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERROTATE_DESTROYEXIFWRITER_EXIT );
    }
