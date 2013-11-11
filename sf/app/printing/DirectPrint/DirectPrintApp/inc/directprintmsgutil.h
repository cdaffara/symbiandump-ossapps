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
* This class is the message utility class of DirectPrint
*
*/

#ifndef __DIRECTPRINTMSGUTIL_H__
#define __DIRECTPRINTMSGUTIL_H__

// System includes
#include <e32base.h>
#include <f32file.h>

// User includes

// Forward declarations

// Class declaration
/**
 *  one_line_short_description
 *  more_complete_description
 *
 */
class CDirectPrintMsgUtil : public CBase
	{
public:
	static CDirectPrintMsgUtil* NewL();
	static CDirectPrintMsgUtil* NewLC();
	~CDirectPrintMsgUtil();

    /**
    * Displays error note
    * @param aResourceId Resource id for note text.
    */
    static void DisplayErrorNoteL( TInt aResourceId );

protected:
	CDirectPrintMsgUtil();
	void ConstructL();

private:
	};

#endif // __DIRECTPRINTMSGUTIL_H__
// End of file
