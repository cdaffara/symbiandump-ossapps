/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: Indrajit Tapadar
*
* Description:   Readme file is a help file to know more about the folder, its files and usage
*
*/

This file contains the sis package installations for Photos on Qt.

About the files :

Photos.pkg : This contains all the information regarding the photos as a package. all new dll's should 
	     be added to this file without fail.

Photos_stub.pkg : This conatins the dll names which needs to be updated later with a sis package.
		Any New addition of Dll's also should be added here.
		Remember , This package is used to create a photos_stub.sis file which is then exported to
		the install directory in epoc, which is again read from the iby file to be passed in rom image.

RDTest_02.key : Key for making a sisx file
RDTest_02.cert : certificate for creating a sisx file.

Createsis.bat : this file conatins the procedure of creating the Photos_stub.sis file and Photos.sisx file.
		The former is passed to the install directory wheras the later is installed on phone.
	

After any new addition of dll's please do not forget to update the photos_stub.sis file and also checkin the latest 
	file.