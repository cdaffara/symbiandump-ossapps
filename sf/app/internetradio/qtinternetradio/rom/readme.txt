pre-conditions:
1. pf5250_200848 SDK
2. Qt garden release and Orbit
3. compile PresetServer and InternetRadio for armv5 urel
4. compile QTIR for armv5 udeb

create image
1. copy QtInternetRadio.iby to \epoc32\rom\include\core\app

2. copy internetradio_binaries.txt and QtInternetRadio_binaries.txt to \epoc32\rombuild\iad

3. modify \s60\tools\platformtools\iad_rofs\data\iad_rofs_config.xml to add the following:
   <component desc="InternetRadio"    infile="\epoc32\rombuild\iad\internetradio_binaries.txt"       outfile="\epoc32\rombuild\IAD_internetradio_rofsfiles.txt"/> 

   <component desc="QtInternetRadio" infile="\epoc32\rombuild\iad\QtInternetRadio_binaries.txt"      outfile="\epoc32\rombuild\IAD_QtInternetRadio_rofsfiles.txt"/>


4. check dependencies of listed binaries:
checkDependency.pl -i \s60\tools\platformtools\iad_rofs\data\iad_rofs_config.xml -o \epoc32\rombuild\IAD_rofsfiles.txt

   After running this, IAD_QtInternetRadio_rofsfiles.txt should be generated under folder \epoc32\rombuild\.
   Also, IAD_QtInternetRadio_rofsfiles.txt should be included in file IAD_rofsfiles.txt.

5. run createor.pl

create .sis file
1. copy internetradio_binaries.txt to \epoc32\rombuild\iad
2. modify \s60\tools\platformtools\iad_rofs\data\iad_rofs_config.xml to add the following:
   <component desc="InternetRadio"    infile="\epoc32\rombuild\iad\internetradio_binaries.txt"       outfile="\epoc32\rombuild\IAD_internetradio_rofsfiles.txt"/> 

3. check dependencies of listed binaries:
   checkDependency.pl -i \s60\tools\platformtools\iad_rofs\data\iad_rofs_config.xml -o \epoc32\rombuild\IAD_rofsfiles.txt

4. 
