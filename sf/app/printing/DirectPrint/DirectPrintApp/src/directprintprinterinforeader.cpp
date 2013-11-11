/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/

// INCLUDE FILES
#include <eikenv.h>
#include <f32file.h>
#include <xmlengelement.h>
#include <xmlengattr.h>
#include <xmlengnodelist.h>

#include "DirectPrintModel.h"
#include "directprintprinterinforeader.h"

CDirectPrintPrinterInfoReader::CDirectPrintPrinterInfoReader(CDirectPrintModel& aModel)
	: iModel(aModel)
	{
	}

CDirectPrintPrinterInfoReader::~CDirectPrintPrinterInfoReader()
	{
	if (iXmlEngDoc.NotNull()) 
		{
		iXmlEngDoc.Close();
		iXmlParser.Close();
		iXmlDomImpl.Close();
		}
	}

CDirectPrintPrinterInfoReader* CDirectPrintPrinterInfoReader::NewL(CDirectPrintModel& aModel)
	{
	CDirectPrintPrinterInfoReader* self = CDirectPrintPrinterInfoReader::NewLC(aModel);
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintPrinterInfoReader* CDirectPrintPrinterInfoReader::NewLC(CDirectPrintModel& aModel)
	{
	CDirectPrintPrinterInfoReader* self = new(ELeave) CDirectPrintPrinterInfoReader(aModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintPrinterInfoReader::ConstructL()
	{
	iXmlDomImpl.OpenL();
	TInt err = iXmlParser.Open(iXmlDomImpl);
	User::LeaveIfError(err);
	}

void CDirectPrintPrinterInfoReader::ReadPrinterInfoL()
	{
	TDirectPrintPrinterData printer;
	iModel.GetPrinterDataL(0, printer);

	_LIT( KFilePath, "c:\\printers\\*.xml");
	_LIT( KFilePathFormat, "c:\\printers\\%S");

	CDir* fileList;
	const TUint KMask = KEntryAttNormal|KEntryAttReadOnly|KEntryAttHidden|KEntryAttSystem;
	User::LeaveIfError(CEikonEnv::Static()->FsSession().GetDir(KFilePath, KMask, ESortByName, fileList));
	CleanupStack::PushL(fileList);

	TInt count = fileList->Count();
	for (TInt i=0; i<count; i++)
		{
		TFileName filename;
		filename.Format(KFilePathFormat, &(*fileList)[i].iName);
		ReadXmlDataL(filename);

		// Check XML file for current printer driver
		if (CheckManufactureL(printer.iDriverName))
			{
			break;
			}
		else
			{
		    iXmlEngDoc.Close();
			}
		}

	CleanupStack::PopAndDestroy(); // fileList
	}

void CDirectPrintPrinterInfoReader::GetManufactureL(TDes& aManufacture)
	{
	if (iXmlEngDoc.NotNull()) 
		{
		_LIT8(KElementManufacture, "Manufacture");

		TXmlEngElement element = iXmlEngDoc.DocumentElement().FirstChild().AsElement();

		while (element.NotNull())
			{
			TBuf<50> elementName;
			elementName.Copy(element.Name());
			if (element.Name().CompareF(KElementManufacture()) == 0)
				{
				aManufacture.Copy(element.Value());
				break;
				}

			element = element.NextSibling().AsElement();
			}
		}
	}

TUid CDirectPrintPrinterInfoReader::GetDriverL(TDesC& aId)
	{
	TUint uidValue = 0;

	if (iXmlEngDoc.NotNull()) 
		{
		_LIT8(KElementPrinters, "Printers");
		_LIT8(KElementPrinter, "Printer");
		_LIT8(KElementDriver, "Driver");
		_LIT8(KAttributeId, "id");

		TXmlEngElement element = iXmlEngDoc.DocumentElement().FirstChild().AsElement();

		while (element.NotNull())
			{
			TBuf<50> elementName;
			elementName.Copy(element.Name());
			if (element.Name().CompareF(KElementPrinters()) == 0)
				{
				element = element.FirstChild().AsElement();
				}
			else if (element.Name().CompareF(KElementPrinter()) == 0)
				{
				TBool next = EFalse;
				TBuf<50> attrValue;
				TXmlEngAttr attr = element.AttributeNodeL(KAttributeId);
				attrValue.Copy(attr.Value());
				if (aId.CompareF(attrValue) == 0)
					{
					element = element.FirstChild().AsElement();
					next = ETrue;
					}

				if (!next)
					{
					element = element.NextSibling().AsElement();
					}
				}
			else if (element.Name().CompareF(KElementDriver()) == 0)
				{
				TLex8 lex(element.Value());
				lex.Val(uidValue, EHex);
				break;
				}
			else
				{
				element = element.NextSibling().AsElement();
				}
			}
		}

	return TUid::Uid(uidValue);
	}

TUid CDirectPrintPrinterInfoReader::GetSettingUIPluginL(TDesC& aId)
	{
	TUint uidValue = 0;

	if (iXmlEngDoc.NotNull()) 
		{
		_LIT8(KElementPrinters, "Printers");
		_LIT8(KElementPrinter, "Printer");
		_LIT8(KElementSettingUIPlugin, "SettingUIPlugin");
		_LIT8(KAttributeId, "id");

		TXmlEngElement element = iXmlEngDoc.DocumentElement().FirstChild().AsElement();

		while (element.NotNull())
			{
			if (element.Name().CompareF(KElementPrinters()) == 0)
				{
				element = element.FirstChild().AsElement();
				}
			else if (element.Name().CompareF(KElementPrinter()) == 0)
				{
				TBool next = EFalse;
				TBuf<50> attrValue;
				TXmlEngAttr attr = element.AttributeNodeL(KAttributeId);
				attrValue.Copy(attr.Value());
				if (aId.CompareF(attrValue) == 0)
					{
					element = element.FirstChild().AsElement();
					next = ETrue;
					}

				if (!next)
					{
					element = element.NextSibling().AsElement();
					}
				}
			else if (element.Name().CompareF(KElementSettingUIPlugin()) == 0)
				{
				TLex8 lex(element.Value());
				lex.Val(uidValue, EHex);
				break;
				}

			element = element.NextSibling().AsElement();
			}
		}

	return TUid::Uid(uidValue);
	}

void CDirectPrintPrinterInfoReader::ReadXmlDataL(const TDesC& aFilename)
	{
	if (iXmlEngDoc.NotNull()) 
		{
		iXmlEngDoc.Close();
		}
	iXmlEngDoc = iXmlParser.ParseFileL(aFilename);
	}

TBool CDirectPrintPrinterInfoReader::CheckManufactureL(const TDesC& aParam)
	{
	TBool res = EFalse;
	TBuf<50> buf;
	GetManufactureL(buf);
	if (aParam.Compare(buf) == 0)
		{
		res = ETrue;
		}
	return res;
	}

