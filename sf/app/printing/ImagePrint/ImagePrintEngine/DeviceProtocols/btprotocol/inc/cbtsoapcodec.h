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
* Description:  Defines the CBtSoapCodec class to create and read SOAP-encoded messages.
*
*/


#ifndef CBTSOAPCODEC_H
#define CBTSOAPCODEC_H

#include <e32base.h>
#include <f32file.h>
#include <e32math.h>

#include "cxmlhandler.h"
#include "tprintcapability.h"
#include "btprotocolconsts.h"

/** @name Constants */
/** Template file to use to create the XML soap-encoded messages  */
	_LIT( KBtXmlTemplate, "z:\\resource\\imageprintdata\\protocols\\btxmltemplate.txt" );	


/** general tags */
_LIT8(KBtEnvelopeStartTag, "<s:Envelope");
_LIT8(KBtEnvelopeEndTag, 	"</s:Envelope>");
_LIT8(KBtBodyStartTag, 	"<s:Body>");
_LIT8(KBtBodyEndTag, 		"</s:Body>");

_LIT8(KBtAction,			"[Action]");
_LIT8(KBtActionData,		"[ActionData]");
_LIT8(KBtBytesInBody,		"[BytesInBody]");

/** Create job tags */
_LIT8(KBtCreateJobStart,			"[CreateJobStart]");
_LIT8(KBtCreateJobEnd,				"[CreateJobEnd]");

/** Get printer attributes tags */
_LIT8(KBtGetPrinterAttributesAllStart,	"[GetPrinterAttributesAllStart]");
_LIT8(KBtGetPrinterAttributesAllEnd,	"[GetPrinterAttributesAllEnd]");
_LIT8(KBtGetPrinterAttributesStart,		"[GetPrinterAttributesStart]");
_LIT8(KBtGetPrinterAttributesEnd,		"[GetPrinterAttributesEnd]");
_LIT8(KBtGetPrinterCapabilitiesStart,	"[GetPrinterCapabilitiesStart]");
_LIT8(KBtGetPrinterCapabilitiesEnd,		"[GetPrinterCapabilitiesEnd]");
_LIT8(KBtGetPrinterStateStart,			"[GetPrinterStateStart]");
_LIT8(KBtGetPrinterStateEnd,			"[GetPrinterStateEnd]");


/** Get printer attributes tags */
_LIT8(KBtGetJobAttributesStart,		"[GetJobAttributesStart]");
_LIT8(KBtGetJobAttributesEnd,		"[GetJobAttributesEnd]");

/** Get margins tags */
_LIT8(KBtGetMarginsStart,	"[GetMarginsStart]");
_LIT8(KBtGetMarginsEnd,		"[GetMarginsEnd]");
_LIT8(KBtMediaSizeData,		"[MediaSizeData]");
_LIT8(KBtMediaTypeData,		"[MediaTypeData]");

/** Job related */
_LIT8(KBtJobIdStart,	"[JobIdStart]");
_LIT8(KBtJobIdEnd,		"[JobIdEnd]");

/** Actions */
enum TBtSoapActions
{
	EBtUnknownAction = -1,
	EBtCreateJob,
	EBtCreateJobResponse,
	EBtGetPrinterAttributes,
	EBtGetPrinterAttributesResponse,
	EBtGetJobAttributes,
	EBtGetJobAttributesResponse,
	EBtCancelJob,
	EBtCancelJobResponse,
	EBtGetEvent,
	EBtGetEventResponse,
	EBtGetMargins,
	EBtGetMarginsResponse,
	// Not actual actions but identifiers as sub for EBtGetPrinterAttributes
	EBtGetPrinterCapabilities,
	EBtGetPrinterState
};

_LIT8(KBtCreateJob,						"CreateJob");
_LIT8(KBtCreateJobResponse,				"CreateJobResponse");
_LIT8(KBtGetPrinterAttributes,			"GetPrinterAttributes");
_LIT8(KBtGetPrinterAttributesResponse,	"GetPrinterAttributesResponse");
_LIT8(KBtGetJobAttributes,				"GetJobAttributes");
_LIT8(KBtGetJobAttributesResponse,		"GetJobAttributesResponse");
_LIT8(KBtCancelJob,						"CancelJob");
_LIT8(KBtCancelJobResponse,				"CancelJobResponse");
_LIT8(KBtGetEvent,						"GetEvent");
_LIT8(KBtGetEventResponse,				"GetEventResponse");
_LIT8(KBtGetMargins,					"GetMargins");
_LIT8(KBtGetMarginsResponse,			"GetMarginsResponse");

_LIT8(KBtNumberUpSupported,			"NumberUpSupported");
_LIT8(KBtMediaTypesSupported,		"MediaTypesSupported");
_LIT8(KBtMediaSizesSupported,		"MediaSizesSupported");
_LIT8(KBtPrintQualitySupported,		"PrintQualitySupported");
_LIT8(KBtMediaLoaded,				"MediaLoaded");
_LIT8(KBtLoadedMediumDetails,		"LoadedMediumDetails");

/** Attributes */
_LIT8(KBtOperationStatus,			"OperationStatus");
_LIT8(KBtJobId,						"JobId");
_LIT8(KBtJobName,					"JobName");
_LIT8(KBtJobState,					"JobState");
_LIT8(KBtJobOriginatingUserName,	"JobOriginatingUserName");
_LIT8(KBtJobMediaSheetsCompleted,	"JobMediaSheetsCompleted");
_LIT8(KBtJobPending,				"NumberOfInterveningJobs");

_LIT8(KBtPrinterName,				"PrinterName");
_LIT8(KBtPrinterState,				"PrinterState");
_LIT8(KBtPrinterStateReasons,		"PrinterStateReasons");

_LIT8(KBtDocumentFormat,			"DocumentFormat");
_LIT8(KBtCopies,					"Copies");
_LIT8(KBtSides,						"Sides");
_LIT8(KBtNumberUp,					"NumberUp");
_LIT8(KBtOrientationRequest,		"OrientationRequest");
_LIT8(KBtMediaSize,					"MediaSize");
_LIT8(KBtMediaType,					"MediaType");
_LIT8(KBtPrintQuality,				"PrintQuality");
_LIT8(KBtLoadedMediumSize,			"LoadedMediumSize");
_LIT8(KBtLoadedMediumType,			"LoadedMediumType");

_LIT8(KBtStartTag, 			"<%S>");
_LIT8(KBtEndTag, 			"</%S>");
_LIT8(KBtValueTemplate, 	"[%S]");

static const TInt KAttrStrLen = 0x100;
typedef TBuf8<KAttrStrLen> TBtAttrString8;
typedef TBuf8<KDefaultRealWidth> TBtIntString8;

/**
* @class 	CBtSoapCodec
* @brief	Class to create and read SOAP-encoded messages.
*
* Class defines methods to encode and decode BPP actions to SOAP-encoded messages.
* @n
* @b Usage:
* @n
* @b Creating a message:
* <OL>
* 1. Create an instance of the class
* 2. Create an encoded message by CreateSoapL
* </OL>
* @n
* After a message is created, it must be added in OBEX message body.
* @n
* @b Reading a message:
* After a OBEX message is received, a body from it can be given to
* CBtSoapCodec to decode the SOAP-encoded message.
* <OL>
* 1. Decode and the message by ReadSoapL
* 2. Get the attributes(s) of the message by GetAttributeL
* </OL>
*
*/
NONSHARABLE_CLASS( CBtSoapCodec ) : public CBase
	{
		public:

			/**  		NewL
			* @brief	Creates a new instance of SOAP codec class.
			* @return	Pointer to the instance.
			*/
			static CBtSoapCodec* NewL();

			/**  		NewLC
			* @brief	Creates a new instance of SOAP codec class and leaves it in the cleanup stack.
			* @return	Pointer to the instance.
			*/
			static CBtSoapCodec* NewLC();

			/**  		~CBtSoapCodec
			* @brief	Destructor.
			*/
			~CBtSoapCodec();


			/**  		Reset
			* @brief	Resets the member variables.
			*/
			void Reset();

			/**  		CreateSoapL
			* @brief	Creates the SOAP-encoded message.
			*			According to the action parameter given, the method reads
			*			the xml template for a named action from file @ref KBtXmlTemplate
			*			and decodes it with the data concerning.
			* @param	aAction Name of the action for the message to create.
			* @param	aJobId Jod identifier for the actions required.
			*			NOTE: for CreateJob operation the job id is the cumulative number
			*			of the created jobs during the session.
			* @return	Pointer to the SOAP-encoded message for the action
			*/
			TPtrC8 CreateSoapL(const TInt aAction, const TInt aJobId = KErrNotFound);

			/**   		SoapAction
			* @brief	Returns for the action of the SOAP-encoded message
			* @return	TBtSoapActions type action identifier
			*/
			TInt SoapAction();

			/**   		ReadSoapL
			* @brief	Reads the SOAP-encoded message.
			*			The message is read and the class is initialized with the data of the message.
			* @param	aAction	Returns the name of the action of the SOAP-encoded data
			* @param	aSoapBuffer	The buffer to read
			*/
			void ReadSoapL(TInt& aAction, const TDesC8& aSoapBuffer);

			/** @overload void ReadSoapL(TInt& aAction, const TDesC8& aSoapBuffer) */
			void ReadSoapL(TInt& aAction, const CBufBase& aSoapBuffer);

			/** @overload void ReadSoapL(TInt& aAction, const TDesC8& aSoapBuffer) */
			void ReadSoapL(const TDesC8& aSoapBuffer);

			/** @overload void ReadSoapL(TInt& aAction, const TDesC8& aSoapBuffer) */
			void ReadSoapL(const CBufBase& aSoapBuffer);

		public:	
			void CreateJobResponseL(TInt& aJobId, TInt& aErrorCode);
			
			void GetPrinterAttributesResponseL(TInt& aState, TInt& aStateReasons,
								 			   RArray<TPrintCapability>& aCapabilities,
								 			   TInt& aOperationStatus);
											 			   
			void GetPrinterStateResponseL(TInt& aState, TInt& aStateReasons, TInt& aOperationStatus);

			void GetPrinterCapabilitiesResponseL(RArray<TPrintCapability>& aCapabilities,
								 			   TInt& aOperationStatus);

			void GetJobAttributesResponseL(const TInt  aJobId,
										   TInt& aJobState, 
										   TBtAttrString8& aJobName,
										   TBtAttrString8& aOriginator,
										   TInt& aSheetsCompleted, 
										   TInt& aPendingJobs,
										   TInt& aOperationStatus );
											   			
			void CancelJobResponseL(const TInt aJobId, TInt& aOperationStatus);
			
			void GetEventResponseL(const TInt aJobId,
								  TInt& aJobState,
								  TInt& aPrinterState,
								  TInt& aStateReasons,
								  TInt& aOperationStatus);
			TInt OperationStatusL();
		
		public:
			/**   		GetAttributeL
			* @brief	Reads the requested attributes's value.
			* @param	aName 	Name of the attribute
			* @param	aValue 	Returns the value of the attribute
			* @param	aError 	Returns the code for the occurred error or KErrNone
			*/
			void GetAttributeL(const TDesC8& aName, TDes8& aValue, TInt& aError);
			
			void GetAttributeListL(const TDesC8& aName, CDesC8ArrayFlat& aList, TInt& aError);

		public:
			/** TestMeL - Static test method
			* @brief Static method to test this class' public interface.
			*/
			static void TestMeL();
			static TPtrC8 CreateJobResponseTestSoap();
			static TPtrC8 CancelJobResponseTestSoap();
			static TPtrC8 GetPrinterAttributesResponseTestSoap();
			static TPtrC8 GetJobAttributesResponseTestSoap();
			static TPtrC8 GetEventsResponseTestSoap();
			static TPtrC8 GetMarginsResponseTestSoap();

		protected:

			/**  		ConstructL
			* @brief	Symbian second phase constructor.
			*/
			void ConstructL();

		private:

			/**  		CBtSoapCodec
			* @brief	C++ constructor
			*/
			CBtSoapCodec();

			/**  		CreateSoapTemplateL
			* @brief	Constructs a correct type message template.
			*			According to the given parameters method composes a template
			*			for the message with general headers and action specific data block.
			* @param	aStartTag 	Start tag for the action
			* @param	aEndTag 	End tag for the action
			*/
			void CreateSoapTemplateL(const TDesC8& aStartTag, const TDesC8& aEndTag);

			/**   		CreateJobSoapL
			* @brief	Composes the CreateJob action specific data in template.
			*/
			void CreateJobSoapL(const TInt aJobId);

			/**   		GetPrinterAttributesSoapL
			* @brief	Composes the GetPrinterAttributes action specific data in template.
			*/
			void GetPrinterAttributesSoapL();
			void GetCapabilitiesSoapL();
			void GetPrinterStateSoapL();

			void GetJobAttributesSoapL(const TDesC8&  aJobId);
			void CancelJobSoapL(const TDesC8&  aJobId);
			void GetEventSoapL(const TDesC8&  aJobId);
			void GetMarginsSoapL();

			TPrintCapability ParseLayoutL();
			TPrintCapability ParseQualityL();
			TPrintCapability ParsePaperSizeL();
			void ParseDefaultSizeL(TPrintCapability& aSize, TInt& aPos);
			TInt ValuePosition(TPrintCapability aCapab, TInt aValue);
			
		private:

			/** @var HBufC8* iActionBuffer
			 *  Buffer to store the action data */
			HBufC8* iActionBuffer;

			/** @var CXmlHandler* iXmlHandler
			 *  XML handler */
			CXmlHandler* iXmlHandler;

	};

#endif // CBTSOAPCODEC_H

//  End of File
