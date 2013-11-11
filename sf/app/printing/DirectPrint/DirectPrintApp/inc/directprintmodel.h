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

// Protection against nested includes
#ifndef __DIRECTPRINTMODEL_H__
#define __DIRECTPRINTMODEL_H__

// System includes
#include <e32base.h>
#include <f32file.h>
#include <prninf.h>

// User includes
#include "directprintprinterdata.h"
#include "directprintbearerdata.h"
#include "directprintpropertydata.h"
#include "directprintproperty.h"

// Forward declarations
class CDirectPrintPrinterInfoReader;

// Class declaration
/**
 *  Model class
 *  more_complete_description
 */
class CDirectPrintModel : public CBase
	{
public:
	/** Constructors */
	static CDirectPrintModel* NewL();
	static CDirectPrintModel* NewLC();
	/** Destructor */
	~CDirectPrintModel();

	/**
	 * Initialize data.
	 */
	void InitializeDataL();
	/**
	 * Internalize data.
	 */
	void InternalizeDataL();
	/**
	 * Externalize data.
	 */
	void ExternalizeDataL();

	/**
	 * Get printer data.
	 *
	 * @param aIndex The index of data array.
	 * @param aData printer data
	 */
	void GetPrinterDataL(TInt aIndex, TDirectPrintPrinterData& aData);
	/**
	 * Get bearer data.
	 *
	 * @param aIndex The index of data array.
	 * @param aData bearer data
	 */
	void GetBearerDataL(TInt aIndex, TDirectPrintBearerData& aData);
	/**
	 * Get current bearer data.
	 *
	 * @param aData bearer data
	 */
	void GetCurrentBearerDataL(TDirectPrintBearerData& aData);
	/**
	 * Get selected bearer data.
	 *
	 * @param aData bearer data
	 */
	void GetSelectedBearerDataL(TDirectPrintBearerData& aData);
	/**
	 * Get property data.
	 *
	 * @param aIndex The index of data array.
	 * @return property data
	 */
	CPrinterProperty* GetPrinterPropertyL(TInt aIndex);
	/**
	 * Get current property data.
	 *
	 * @param aAppUid Application UID.
	 * @return property data
	 */
	CPrinterProperty* GetCurrentPrinterPropertyL(TUid aAppUid);
	/**
	 * Get the id of current printer data.
	 *
	 * @return The id of printer data
	 */
	TInt GetCurrentPrinterIdL();
	/**
	 * Get the index of current printer data.
	 *
	 * @return The index of data array.
	 */
	TInt CurrentPrinter();
	/**
	 * Get the index of current bearer data.
	 *
	 * @return The index of data array.
	 */
	TInt CurrentBearer();
	/**
	 * Get the index of current property data.
	 *
	 * @return The index of data array.
	 */
	TInt CurrentProperty();
	/**
	 * Get the index of selected printer data.
	 *
	 * @return The index of data array.
	 */
	TInt SelectPrinter();
	/**
	 * Get the count of printer data.
	 *
	 * @return The count of data.
	 */
	TInt PrinterCount();
	/**
	 * Get the count of bearer data.
	 *
	 * @return The count of data.
	 */
	TInt BearerCount();
	/**
	 * Get the count of property data.
	 *
	 * @return The count of data.
	 */
	TInt PropertyCount();

	/**
	 * Set printer data.
	 *
	 * @param aIndex The index of data array.
	 * @param aData printer data
	 */
	void SetPrinterDataL(TInt aIndex, const TDirectPrintPrinterData& aData);
	/**
	 * Set printer data.
	 *
	 * @param aData printer data
	 */
	void SetPrinterDataL(const TDirectPrintPrinterData& aData);
	/**
	 * Set bearer data.
	 *
	 * @param aIndex The index of data array.
	 * @param aData bearer data
	 */
	void SetBearerDataL(TInt aIndex, const TDirectPrintBearerData& aData);
	/**
	 * Set bearer data.
	 *
	 * @param aData bearer data
	 */
	void SetBearerDataL(const TDirectPrintBearerData& aData);
	/**
	 * Set property data.
	 *
	 * @param aIndex The index of data array.
	 * @param aProperty property data
	 */
	void SetPrinterPropertyL(TInt aIndex, CPrinterProperty* aProperty);
	/**
	 * Set property data.
	 *
	 * @param aProperty property data
	 */
	void SetPrinterPropertyL(CPrinterProperty* aProperty);
	/**
	 * Set the index of current printer data.
	 *
	 * @param aIndex The index of data array.
	 */
	void SetCurrentPrinter(TInt aIndex);
	/**
	 * Set the index of current bearer data.
	 *
	 * @param aIndex The index of data array.
	 */
	void SetCurrentBearer(TInt aIndex);
	/**
	 * Set the index of current property data.
	 *
	 * @param aIndex The index of data array.
	 */
	void SetCurrentProperty(TInt aIndex);
	/**
	 * Set the index of selected printer data.
	 *
	 * @param aIndex The index of data array.
	 */
	void SetSelectPrinter(TInt aIndex);

	/**
	 * Add printer data.
	 *
	 * @param aData printer data
	 */
	void AddPrinterDataL(const TDirectPrintPrinterData& aData);
	/**
	 * Add bearer data.
	 *
	 * @param aData bearer data
	 */
	void AddBearerDataL(const TDirectPrintBearerData& aData);
	/**
	 * Add property data.
	 *
	 * @param aProperty property data
	 */
	void AddPrinterPropertyL(CPrinterProperty* aProperty);

	/**
	 * Remove printer data from data array.
	 *
	 * @param aIndex The index of data array.
	 */
	void RemovePrinterDataL(TInt aIndex);
	/**
	 * Remove bearer data from data array.
	 *
	 * @param aIndex The index of data array.
	 */
	void RemoveBearerDataL(TInt aIndex);
	/**
	 * Remove property data from data array.
	 *
	 * @param aIndex The index of data array.
	 */
	void RemovePrinterPropertyL(TInt aIndex);

	/**
	 * Move printer data to top of data array.
	 *
	 * @param aIndex The index of data array.
	 */
	void MoveTopPrinterDataL(TInt aIndex);

	/**
	 * Get new data flag.
	 *
	 * @return new data flag.
	 */
	inline TBool IsNewData() {return iIsNewData;};
	/**
	 * Set new data flag.
	 *
	 * @param aFlag new data flag.
	 */
	inline void SetIsNewData(TBool aFlag) {iIsNewData = aFlag;};
	/**
	 * Generate new printer id.
	 *
	 * @return new data flag.
	 */
	inline TInt NewPrinterId() {return ++iMaxPrinterId;};

	/**
	 * Get uid of plugin.
	 *
	 * @return uid of plugin.
	 */
	TUid SettingUIPluginUidL();

protected:
	CDirectPrintModel();
	void ConstructL();

private:
	/** File server session. */
	RFs iFs;

	/** Printer data Array. */
	RDPPrinterDataArray iPrinters;
	/** Bearer data Array. */
	RDPBearerDataArray iBearers;
	/** Property data Array. */
	RPrinterPropertyArray iProperties;
	/** The index of current printer data. */
	TInt iCurrentPrinter;
	/** The index of current bearer datay. */
	TInt iCurrentBearer;
	/** The index of current property data. */
	TInt iCurrentProperty;
	/** The index of selected printer data. */
	TInt iSelectPrinter;

	/** New data flag. */
	TBool iIsNewData;
	/** Printer id. */
	TInt iMaxPrinterId;

	CDirectPrintPrinterInfoReader* iPrinterInfoReader;
	};

#endif // __DIRECTPRINTMODEL_H__
