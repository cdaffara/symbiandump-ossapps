/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler for XML file modify. Defines the class to store and modify XML data.
*
*/


#ifndef CXMLHANDLER_H
#define CXMLHANDLER_H

#include <e32base.h>
#include <f32file.h>
#include <badesca.h>

/** general tags */
_LIT8(KBtLessThan, 	"<");
_LIT8(KBtGreaterThan,	">");
_LIT8(KBtLessThanSlash,"</");
_LIT8(KBtSlash, 		"/");
_LIT8(KEof, 			"EOF");

/** Maximum size of template file */
const TInt KBtMaxFileSize = 2048;

/**
* @class 	CXmlHandler
* @brief	Class to store and modify XML data.
* @n
* @b Usage:
* <OL>
* @li Create an instance of class
* @li Initialize class with a data buffer by InitFromBufferL or with a file name by InitFromFileL
* @li Get/replace/remove parts of initialized data
* </OL>
*/
NONSHARABLE_CLASS( CXmlHandler ): public CBase
	{
		public:

			/**  		NewL
			* @brief	Creates a new instance of XML handler.
			* @return	Pointer to the instance.
			*/
			static CXmlHandler* NewL();

			/**  		NewLC
			* @brief	Creates a new instance of XML handler and leaves it in the cleanup stack.
			* @return	Pointer to the instance.
			*/
			static CXmlHandler* NewLC();

			static TPtrC8 ParseValue(const TDesC8 &aData, const TDesC8 &aStart, const TDesC8 &aEnd, TInt& aOffset);
			static void ParseValueListL(const TDesC8 &aData, const TDesC8 &aStart, const TDesC8 &aEnd, CDesC8ArrayFlat& aList);

			/**  		~CXmlHandler
			* @brief	Destructor.
			*/
			~CXmlHandler();


			/**  		Reset
			* @brief	Resets the member variables.
			*/
			void Reset();

			/**  		InitFromFileL
			* @brief	Reads the named template file for handling it.
			* @param	aFile 	Name of the file to read for handle
			* @param	aId		Identifier for the data stored in class
			*/
			void InitFromFileL(const TDesC& aFile, const TInt aId = KErrNotFound);

			/**  		InitFromBufferL
			* @brief	Initialises the data to handle with given buffer.
			* @param	aBuffer The buffer to handle
			* @param	aId		Identifier for the data stored in class
			*/
			void InitFromBufferL(const TDesC8& aBuffer, const TInt aId = KErrNotFound);


			/**   		ComposeDataL
			* @brief	Replaces the template with string value in stored data
			* @param	aStr	String to convert for
			* @param	aTag	Template to replace
			*/
			void ComposeDataL(const TDesC8& aData, const TDesC8& aTag);

			/** @overload void ComposeDataL(const TDesC8& aData, const TDesC8& aTag) */
			void ComposeDataL(const TInt aValue, const TDesC8& aTag);

			/**   		InsertDataL
			* @brief	Inserts data in the buffer
			* @param	aStr	String to convert for
			* @param	aTag	Template to replace
			*/
			void InsertDataL(const TDesC8& aData, const TInt aOffset);

			/**   		GetDataL
			* @brief	Returns the data between given start and end descriptors.
			*			NOTE: The start and end descriptors are @e not returned.
			* @param	aStart	Start tag for data to find.
			* @param	aEnd	End tag for data to find.
			* @param	aOffset	Returns offset for the data start. KErrNotFound indicates the data not found.
			*/
			TPtrC8 GetDataL(const TDesC8& aStart, const TDesC8& aEnd, TInt& aOffset);

			/**   		FinalizeBufferL
			* @brief	Trims the member buffer tail. Replacing the template strings from
			*			the buffer makes the data length in the buffer vary and generates
			*			rubbish in the end of buffer.
			* @param	aTag	End tag to delete all after it.
			* @return	Pointer to the stored data.
			*/
			TPtrC8 FinalizeBufferL(const TDesC8& aTag);

			/**   		DeleteBufferData
			* @brief	Deletes the data from the buffer.
			* @param	aOffset Data offset to start delete
			* @param	aLength Length of the data to delete
			*/
			void DeleteBufferData(TInt aOffset, TInt aLength);

			/**   		BufferId
			* @brief	Returns the identifier of the stored data.
			* @return	Identifier of the stored data
			*/
			TInt BufferId();

			/**   		Buffer
			* @brief	Returns the pointer to the stored data.
			* @return	Pointer to the stored data
			*/
			TPtrC8 Buffer();

			void GetDataListL(const TDesC8 &aStart, const TDesC8 &aEnd, CDesC8ArrayFlat& aList);

		protected:

			/**  		ConstructL
			* @brief	Symbian second phase constructor.
			*/
			void ConstructL();

		private:

			/**  		CXmlHandler
			* @brief	C++ constructor
			*/
			CXmlHandler();

			/**   		ReadFileL
			* @brief	Reads the file in iDataBuf
			*/
			void ReadFileL(const TDesC& aFile);

		private:

			/** @var HBufC8* iDataBuf
			 *  Buffer to store the data initialized */
			HBufC8* iDataBuf;

			/** @var TInt iBufferId
			 *  Identifier for the stored data. Initialized as KErrNotFound. */
			TInt iBufferId;

			/** @var RFs iFs
			 *  File server session */
			RFs iFs;

	};

#endif // CXMLHANDLER_H

//  End of File
