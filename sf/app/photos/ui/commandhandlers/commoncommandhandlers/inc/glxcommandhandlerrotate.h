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

#ifndef GLXCOMMANDHANDLERROTATE_H
#define GLXCOMMANDHANDLERROTATE_H

#include <glxmpxcommandhandler.h>
#include <ExifModify.h>

#ifdef BUILD_COMMONCOMMANDHANDLERS
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_IMPORT
#endif


class GLX_COMMONCOMMANDHANDLERS_EXPORT GlxCommandHandlerRotate : public GlxMpxCommandHandler
	{
public:
    GlxCommandHandlerRotate();
    ~GlxCommandHandlerRotate();
    CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const ;
	void DoExecuteCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume);
private:
	/*
	* Responsible for rotating the Image by Changing the Image Orientation Exif
	*/
	void RotateImageL(TFileName aFileToBeRotated);
	/*
	* Responsible for Initializing the Exif writer. Will leave In case of Error 
	*/
	void InitializeExifWriterL(TFileName aFileToBeRotated);
	void DestroyExifWriter();
	TUint16 ReadImageOrientationL();
	TUint16 CalculateFinalOrientationL(TUint16 aInitialOrientation);
	void SetImageOrientationL(TUint16 aFinalOrientation);
	
	RFs iFs;
	RFile iFileHandle;
    HBufC8* iExifData;
    CExifModify* iExifWriter;
	};

#endif // GLXCOMMANDHANDLERROTATE_H
