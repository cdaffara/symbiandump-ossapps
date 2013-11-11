Environment Patcher v1.0.1
==========================

Updated: 12th November 2009


Introduction:
-------------
This tool can be used to patch your S60 SDK environment so that the tricks and
macros introduced in the latest SDKs can be used in a public SDK and older
OEM releases.

This tool can perform the following tasks:
- Adds support for forward slashes in paths in bld.inf/.mmp files in S60 3.0 
- Removes an unncessary warning about DEPENDS and SMPSAFE resource keywords in
  an .mmp file
- Creates a missing epoc32\include\platform_paths.hrh file for supporting
  platform macros introduced since S60 3.2 OEM and Symbian Foundation releases
- Modifies epoc32\include\platform_paths.hrh for missing macros   
- Creates a missing epoc32\include\oem directory to suppress a possible warning


Usage:
------
EnvPatcher.pl <EPOCROOT>

Where EPOCROOT is the path to the root of the SDK, for example:
  EnvPatcher.pl c:\Symbian\9.1\S60_3rd_MR
  EnvPatcher.pl z:\


Requirements:
-------------
- S60 SDK (public or OEM), version 3.0 or newer
- Perl 5.6.1 or newer




Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
All rights reserved.

This component and the accompanying materials are made available
under the terms of "Eclipse Public License v1.0"
which accompanies this distribution, and is available
at the URL "http://www.eclipse.org/legal/epl-v10.html".
