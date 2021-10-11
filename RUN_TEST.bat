REM @echo off

REM C:\Users\jtaibo\Documents\devel\ant3D\build\src\Release
set PATH=%PATH%;build\src\Release

REM set INPUT_FILE=samples/monopole.txt
REM set INPUT_FILE=samples/example_nec2pp.txt
REM set INPUT_FILE=samples/example_nec2pp.txt
set INPUT_FILE=samples/test_dipolo_1.txt

REM ant3d -nec2_file %INPUT_FILE%
ant3d -dipole
