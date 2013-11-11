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
* This model class for DirectPrint data managemrnt
*
*/

#include <bautils.h>
#include <eikenv.h>
#include <s32file.h>
#include <DirectPrintApp.rsg>

#include "clog.h"
#include "DirectPrintModel.h"
#include "directprintprinterinforeader.h"

CDirectPrintModel::CDirectPrintModel()
	: iCurrentPrinter(-1)
	, iCurrentBearer(-1)
	, iCurrentProperty(-1)
	, iMaxPrinterId(0)
	{
	}

CDirectPrintModel::~CDirectPrintModel()
	{
	iPrinters.Close();
	iBearers.Close();
	iProperties.ResetAndDestroy();
	delete iPrinterInfoReader;
	}

CDirectPrintModel* CDirectPrintModel::NewL()
	{
	CDirectPrintModel* self = CDirectPrintModel::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintModel* CDirectPrintModel::NewLC()
	{
	CDirectPrintModel* self = new(ELeave) CDirectPrintModel();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintModel::ConstructL()
	{
	LOG("CDirectPrintModel::ConstructL BEGIN");
	iFs = CEikonEnv::Static()->FsSession();

	// Read setting data
	InternalizeDataL();

	iPrinterInfoReader = CDirectPrintPrinterInfoReader::NewL(*this);
	iPrinterInfoReader->ReadPrinterInfoL();

	LOG("CDirectPrintModel::ConstructL END");
	}

void CDirectPrintModel::InitializeDataL()
	{
	}

void CDirectPrintModel::InternalizeDataL()
	{
	LOG("CDirectPrintModel::ConstructL BEGIN");
	TFileName fileName;
	CEikonEnv::Static()->ReadResourceAsDes16L(fileName, R_DIRECTPRINT_SETTING_FILE);

	if (BaflUtils::FileExists(iFs, fileName))
		{
		LOG("CDirectPrintModel::ConstructL file exist");
		RFileReadStream stream;
		TInt err = stream.Open(iFs, fileName, EFileRead|EFileStream);

		TInt count, i;
		count = stream.ReadInt32L();
		for (i=0; i<count; i++)
			{
			TDirectPrintPrinterData data;
			data.InternalizeL(stream);
			AddPrinterDataL(data);
			if (iMaxPrinterId < data.iPrinterId)
				{
				iMaxPrinterId = data.iPrinterId;
				}
			}
		iCurrentPrinter = stream.ReadInt32L();

		count = stream.ReadInt32L();
		for (i=0; i<count; i++)
			{
			TDirectPrintBearerData data;
			data.InternalizeL(stream);
			AddBearerDataL(data);
			}
		iCurrentBearer = stream.ReadInt32L();

		count = stream.ReadInt32L();
		for (i=0; i<count; i++)
			{
			CPrinterProperty* property = new(ELeave)CPrinterProperty();
			CleanupStack::PushL(property);
			property->InternalizeL(stream);
			CleanupStack::Pop(property);
			AddPrinterPropertyL(property);
			}
		iCurrentProperty = stream.ReadInt32L();

		stream.Close();
		}
	LOG("CDirectPrintModel::ConstructL END");
	}

void CDirectPrintModel::ExternalizeDataL()
	{
	LOG("CDirectPrintModel::ExternalizeDataL START");
	TFileName fileName;
	CEikonEnv::Static()->ReadResourceAsDes16L(fileName, R_DIRECTPRINT_SETTING_FILE);

	RFileWriteStream stream;
	TInt err = stream.Replace(iFs, fileName, EFileWrite|EFileStream);

	TInt count, i;
	count = iPrinters.Count();
	stream.WriteInt32L(count);
	for (i=0; i<count; i++)
		{
		iPrinters[i].ExternalizeL(stream);
		}
	stream.WriteInt32L(iCurrentPrinter);

	count = iBearers.Count();
	stream.WriteInt32L(count);
	for (i=0; i<count; i++)
		{
		iBearers[i].ExternalizeL(stream);
		}
	stream.WriteInt32L(iCurrentBearer);

	count = iProperties.Count();
	stream.WriteInt32L(count);
	for (i=0; i<count; i++)
		{
		iProperties[i]->ExternalizeL(stream);
		}
	stream.WriteInt32L(iCurrentProperty);

	stream.Close();
	LOG("CDirectPrintModel::ExternalizeDataL END");
	}

void CDirectPrintModel::GetPrinterDataL(TInt aIndex, TDirectPrintPrinterData& aData)
	{
	aData = iPrinters[aIndex];
	}

void CDirectPrintModel::GetBearerDataL(TInt aIndex, TDirectPrintBearerData& aData)
	{
	aData = iBearers[aIndex];
	}

void CDirectPrintModel::GetCurrentBearerDataL(TDirectPrintBearerData& aData)
	{
	TDirectPrintPrinterData printer;
	GetPrinterDataL(CurrentPrinter(), printer);

	TInt count = BearerCount();
	for (TInt i=0; i<count; i++)
		{
		TInt printerId = iBearers[i].iPrinterId;
		if (printerId == printer.iPrinterId)
			{
			aData = iBearers[i];
			break;
			}
		}
	}

void CDirectPrintModel::GetSelectedBearerDataL(TDirectPrintBearerData& aData)
	{
	TDirectPrintPrinterData printer;
	GetPrinterDataL(SelectPrinter(), printer);

	TInt count = BearerCount();
	for (TInt i=0; i<count; i++)
		{
		TInt printerId = iBearers[i].iPrinterId;
		if (printerId == printer.iPrinterId)
			{
			aData = iBearers[i];
			break;
			}
		}
	}

CPrinterProperty* CDirectPrintModel::GetPrinterPropertyL(TInt aIndex)
	{
	return iProperties[aIndex];
	}

CPrinterProperty* CDirectPrintModel::GetCurrentPrinterPropertyL(TUid aAppUid)
	{
	CPrinterProperty* property = NULL;
	TDirectPrintPrinterData printer;
	GetPrinterDataL(CurrentPrinter(), printer);

	TInt index, count;
	count = PropertyCount();
	for (index=0; index<count; index++)
		{
		TInt printerId = iProperties[index]->iPrinterId;
		TUid uid = TUid::Uid(iProperties[index]->iApplicationId);
		if ((printerId == printer.iPrinterId) &&
			(uid == aAppUid))
			{
			if (iCurrentProperty != index)
				{
				iCurrentProperty = index;
				}
			property = iProperties[index];
			break;
			}
		}

	return property;
	}

TInt CDirectPrintModel::GetCurrentPrinterIdL()
	{
	TDirectPrintPrinterData printer;
	GetPrinterDataL(CurrentPrinter(), printer);
	return printer.iPrinterId;
	}

TInt CDirectPrintModel::CurrentPrinter()
	{
	return iCurrentPrinter;
	}

TInt CDirectPrintModel::CurrentBearer()
	{
	return iCurrentBearer;
	}

TInt CDirectPrintModel::CurrentProperty()
	{
	return iCurrentProperty;
	}

TInt CDirectPrintModel::SelectPrinter()
	{
	return iSelectPrinter;
	}

TInt CDirectPrintModel::PrinterCount()
	{
	return iPrinters.Count();
	}

TInt CDirectPrintModel::BearerCount()
	{
	return iBearers.Count();
	}

TInt CDirectPrintModel::PropertyCount()
	{
	return iProperties.Count();
	}

void CDirectPrintModel::SetPrinterDataL(TInt aIndex, const TDirectPrintPrinterData& aData)
	{
	if ((aIndex >= 0) && (iPrinters.Count() > aIndex))
		{
		iPrinters[aIndex] = aData;
		}
	else
		{
		iPrinters.AppendL(aData);
		}
	iPrinterInfoReader->ReadPrinterInfoL();
	}

void CDirectPrintModel::SetPrinterDataL(const TDirectPrintPrinterData& aData)
	{
	TBool isSet = EFalse;
	TInt count = PrinterCount();
	for (TInt i=0; i<count; i++)
		{
		TInt printerId = iPrinters[i].iPrinterId;
		if (printerId == aData.iPrinterId)
			{
			iPrinters[i] = aData;
			isSet = ETrue;
			break;
			}
		}

	if (!isSet)
		{
		iPrinters.AppendL(aData);
		}
	iPrinterInfoReader->ReadPrinterInfoL();
	}

void CDirectPrintModel::SetBearerDataL(TInt aIndex, const TDirectPrintBearerData& aData)
	{
	if ((aIndex >= 0) && (iBearers.Count() > aIndex))
		{
		iBearers[aIndex] = aData;
		}
	else
		{
		iBearers.AppendL(aData);
		}
	}

void CDirectPrintModel::SetBearerDataL(const TDirectPrintBearerData& aData)
	{
	TBool isSet = EFalse;
	TInt count = BearerCount();
	for (TInt i=0; i<count; i++)
		{
		TInt printerId = iBearers[i].iPrinterId;
		if (printerId == aData.iPrinterId)
			{
			iBearers[i] = aData;
			isSet = ETrue;
			break;
			}
		}

	if (!isSet)
		{
		iBearers.AppendL(aData);
		}
	}

void CDirectPrintModel::SetPrinterPropertyL(TInt aIndex, CPrinterProperty* aProperty)
	{
	if ((aIndex >= 0) && (iProperties.Count() > aIndex))
		{
		*(iProperties[aIndex]) = *aProperty;
		delete aProperty;
		}
	else
		{
		iProperties.AppendL(aProperty);
		}
	}

void CDirectPrintModel::SetPrinterPropertyL(CPrinterProperty* aProperty)
	{
	CPrinterProperty* property = GetCurrentPrinterPropertyL(TUid::Uid(aProperty->iApplicationId));
	if (property)
		{
		TInt count1, count2;
		count1 = property->iPropertyItems.Count();
		count2 = aProperty->iPropertyItems.Count();
		for (TInt i=0; i<count2; i++)
			{
			TBool isSet = EFalse;
			for (TInt j=0; j<count1; j++)
				{
				if (aProperty->iPropertyItems[i].iItemId == property->iPropertyItems[j].iItemId)
					{
					(property->iPropertyItems[j]) = (aProperty->iPropertyItems[i]);
					isSet = ETrue;
					break;
					}
				}

			if (!isSet)
				{
				property->iPropertyItems.AppendL(aProperty->iPropertyItems[i]);
				}
			}
		delete aProperty;
		}
	else
		{
		iProperties.AppendL(aProperty);
		}
	}

void CDirectPrintModel::SetCurrentPrinter(TInt aIndex)
	{
	iCurrentPrinter = aIndex;
	}

void CDirectPrintModel::SetCurrentBearer(TInt aIndex)
	{
	iCurrentBearer = aIndex;
	}

void CDirectPrintModel::SetCurrentProperty(TInt aIndex)
	{
	iCurrentProperty = aIndex;
	}

void CDirectPrintModel::SetSelectPrinter(TInt aIndex)
	{
	iSelectPrinter = aIndex;
	}

void CDirectPrintModel::AddPrinterDataL(const TDirectPrintPrinterData& aData)
	{
	iPrinters.AppendL(aData);
	}

void CDirectPrintModel::AddBearerDataL(const TDirectPrintBearerData& aData)
	{
	iBearers.AppendL(aData);
	}

void CDirectPrintModel::AddPrinterPropertyL(CPrinterProperty* aProperty)
	{
	iProperties.AppendL(aProperty);
	}

void CDirectPrintModel::RemovePrinterDataL(TInt aIndex)
	{
	if ((aIndex >= 0) && (iPrinters.Count() > aIndex))
		{
		iPrinters.Remove(aIndex);
		}
	}

void CDirectPrintModel::RemoveBearerDataL(TInt aIndex)
	{
	if ((aIndex >= 0) && (iBearers.Count() > aIndex))
		{
		iBearers.Remove(aIndex);
		}
	}

void CDirectPrintModel::RemovePrinterPropertyL(TInt aIndex)
	{
	if ((aIndex >= 0) && (iProperties.Count() > aIndex))
		{
		iProperties.Remove(aIndex);
		}
	}

void CDirectPrintModel::MoveTopPrinterDataL(TInt aIndex)
	{
	if ((aIndex >= 0) && (iPrinters.Count() > aIndex))
		{
		TDirectPrintPrinterData data = iPrinters[aIndex];
		iPrinters.Remove(aIndex);
		iPrinters.Insert(data, 0);
		}
	iPrinterInfoReader->ReadPrinterInfoL();
	}

TUid CDirectPrintModel::SettingUIPluginUidL()
	{
	TDirectPrintPrinterData printer;
	GetPrinterDataL(0, printer);

	return iPrinterInfoReader->GetSettingUIPluginL(printer.iTypeName);
	}

