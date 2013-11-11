/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef IRRECOGNIZER_H
#define IRRECOGNIZER_H

#include <apmrec.h>
#include "iruid3.hrh"

const TInt KIRRecognizerImplementationUid = UID3_IRFILERECOGNIZER_DLL_IMPLEMENTATION_UID;
const TUid KIRRecognizerDllUid = {UID3_IRFILERECOGNIZER_DLL};
/**
 * Recognizer plug-in for .Pls files.
 * 
 * @code
 * Uses Methods from the base class CApaDataRecognizerType
 * to recognise the .pls files  
 * @endcode
 *
*/
class CApaRecognizerEx: public CApaDataRecognizerType
    {
public:
    /**
    * Default constructor
    */
    CApaRecognizerEx();

    /**
    * Destructor.
    */
    virtual ~CApaRecognizerEx();

    /**
    * CreateRecognizerL()
    * From CApaDataRecognizerType CreateRecognizerL.
    * creates and returns recognizer
    */
	static CApaDataRecognizerType* CreateRecognizerL();

    /**
    * PreferredBufSize()
    * From CApaDataRecognizerType PreferredBufSize.
    * returns preferred buffer size
    */
	TUint PreferredBufSize();

    /**
    * SupportedDataTypeL(SupportedDataTypeL)
    * From CApaDataRecognizerType SupportedDataTypeL.
    * @param aIndex is index that identifies the data type
    * returns supported mime type
    */
	TDataType SupportedDataTypeL(TInt aIndex) const;

private:
        /**
	 * DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	 * From CApaDataRecognizerType DoRecognizeL.
	 * recognises the file by name and/or buffer
	 * @param aName is file name containing the data to be recognized
	 * @param aBuffer is buffer containing data to be recognized
	 */
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	
	/*
	 * try to recognize a m3u file and decide whether it should be launched by InternetRadio application
	 */
	void RecognizeM3uFileL(const TDesC& aFileName);
	
	/*
	 * check if specified protocol streaming links are in aBuffer
	 */
	TBool CheckStreamingLinksL(TDes8& aBuffer, const TDesC8& aProtocol);
    };

#endif //IRRECOGNIZER_H
