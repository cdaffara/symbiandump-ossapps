rem Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

REM NOTE! Do not use tabs here!
md c:\system
md c:\system\data
md c:\system\mtm
md c:\private

copy z:\System\MsgTest\system\data\Contacts.cdb  c:\system\data\Contacts.cdb		
attrib c:\system\data\*.* -r
                                    
copy z:\System\MsgTest\system\popc.dat      c:\system\mtm\popc.dat	  		
copy z:\System\MsgTest\system\smtp.dat      c:\system\mtm\smtp.dat	  		
attrib c:\system\mtm\*.* -r

md c:\MsgTest
md c:\MsgTest\imcm
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02a_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02a_Entry_Structure.txt		
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02b1_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02b1_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02b2_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02b2_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02b_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02b_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02c1_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02c1_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02c2_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02c2_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02c_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02c_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02d1_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02d1_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02d2_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02d2_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02d_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02d_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02e1_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02e1_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02e2_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02e2_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM02e_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM02e_Entry_Structure.txt
copy z:\System\MsgTest\MsgTest\imcm\T_IMCM03_Entry_Structure.txt c:\MsgTest\imcm\T_IMCM03_Entry_Structure.txt
attrib c:\MsgTest\imcm\*.* -r

md c:\mailtest
md c:\mailtest\imcm
md c:\mailtest\rfc822
md c:\mailtest\rfct_imcm06
md c:\mailtest\rfct_imcm07

copy z:\System\MsgTest\mailtest\mainpic.msg c:\mailtest\mainpic.msg	  		
attrib c:\mailtest\*.* -r

copy z:\System\MsgTest\mailtest\imcm\rfc82201.txt c:\mailtest\IMCM\rfc82201.txt
copy z:\System\MsgTest\mailtest\imcm\rfc82202.txt c:\mailtest\IMCM\rfc82202.txt
copy z:\System\MsgTest\mailtest\imcm\rfc82203.txt c:\mailtest\IMCM\rfc82203.txt
copy z:\System\MsgTest\mailtest\imcm\rfc82204.txt c:\mailtest\IMCM\rfc82204.txt
copy z:\System\MsgTest\mailtest\imcm\rfc82205.txt c:\mailtest\IMCM\rfc82205.txt
copy z:\System\MsgTest\mailtest\imcm\rfc82206.txt c:\mailtest\IMCM\rfc82206.txt
attrib c:\mailtest\IMCM\*.* -r

copy z:\System\MsgTest\mailtest\rfc822\html_with_attachment.txt c:\mailtest\rfc822\html_with_attachment.txt	
copy z:\System\MsgTest\mailtest\rfc822\mhtml1.eml c:\mailtest\rfc822\mhtml1.eml		
copy z:\System\MsgTest\mailtest\rfc822\mhtml2.eml c:\mailtest\rfc822\mhtml2.eml		
copy z:\System\MsgTest\mailtest\rfc822\mhtml3.eml c:\mailtest\rfc822\mhtml3.eml		
copy z:\System\MsgTest\mailtest\rfc822\mhtml4.eml c:\mailtest\rfc822\mhtml4.eml		
copy z:\System\MsgTest\mailtest\rfc822\mhtml5.eml c:\mailtest\rfc822\mhtml5.eml		
copy z:\System\MsgTest\mailtest\rfc822\plain_text.txt c:\mailtest\rfc822\plain_text.txt	
copy z:\System\MsgTest\mailtest\rfc822\rfc82201.jig c:\mailtest\rfc822\rfc82201.jig		
attrib c:\mailtest\rfc822\*.* -r

copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82201.txt c:\mailtest\rfct_imcm06\rfc82201.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82202.txt c:\mailtest\rfct_imcm06\rfc82202.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82203.txt c:\mailtest\rfct_imcm06\rfc82203.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82204.txt c:\mailtest\rfct_imcm06\rfc82204.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82205.txt c:\mailtest\rfct_imcm06\rfc82205.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82206.txt c:\mailtest\rfct_imcm06\rfc82206.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82207.txt c:\mailtest\rfct_imcm06\rfc82207.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82208.txt c:\mailtest\rfct_imcm06\rfc82208.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82209.txt c:\mailtest\rfct_imcm06\rfc82209.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82210.txt c:\mailtest\rfct_imcm06\rfc82210.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82211.txt c:\mailtest\rfct_imcm06\rfc82211.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82212.txt c:\mailtest\rfct_imcm06\rfc82212.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82213.txt c:\mailtest\rfct_imcm06\rfc82213.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82214.txt c:\mailtest\rfct_imcm06\rfc82214.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82215.txt c:\mailtest\rfct_imcm06\rfc82215.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82216.txt c:\mailtest\rfct_imcm06\rfc82216.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82217.txt c:\mailtest\rfct_imcm06\rfc82217.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82218.txt c:\mailtest\rfct_imcm06\rfc82218.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82219.txt c:\mailtest\rfct_imcm06\rfc82219.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82220.txt c:\mailtest\rfct_imcm06\rfc82220.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82221.txt c:\mailtest\rfct_imcm06\rfc82221.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82222.txt c:\mailtest\rfct_imcm06\rfc82222.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82223.txt c:\mailtest\rfct_imcm06\rfc82223.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82224.txt c:\mailtest\rfct_imcm06\rfc82224.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82225.txt c:\mailtest\rfct_imcm06\rfc82225.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82226.txt c:\mailtest\rfct_imcm06\rfc82226.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm06\rfc82227.txt c:\mailtest\rfct_imcm06\rfc82227.txt		
attrib c:\mailtest\rfct_imcm06\*.* -r
                                                
copy z:\System\MsgTest\mailtest\rfct_imcm07\rfc82202.txt c:\mailtest\rfct_imcm07\rfc82202.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm07\rfc82203.txt c:\mailtest\rfct_imcm07\rfc82203.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm07\rfc82204.txt c:\mailtest\rfct_imcm07\rfc82204.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm07\rfc82205.txt c:\mailtest\rfct_imcm07\rfc82205.txt		
copy z:\System\MsgTest\mailtest\rfct_imcm07\rfc82206.txt c:\mailtest\rfct_imcm07\rfc82206.txt		
attrib c:\mailtest\rfct_imcm07\*.* -r


