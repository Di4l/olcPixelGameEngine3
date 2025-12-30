@echo on
cd ..\..\src
..\..\..\olcGimmeHead\msvc\x64\debug\gimme-head.exe sh_template.h ..\msvc\olcPGE3_BuildSH\olcPixelGameEngine3.h
copy ..\msvc\olcPGE3_BuildSH\olcPixelGameEngine3.h ..\..\olcPixelGameEngine3.h