To update the help texts, edit the podcatcher.rtf file. OpenOffice.org seems to work much better than Microsoft Office for this.

To build the .help file, use the command:

  cshlpcmp podcatcher.cshlp
  
That should produce a new .hlp file as well as a new .hlp.hrh file. These files are then automatically pulled in by the build and packaging processes.