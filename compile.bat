
SET _mainpath=D:\WORK\web_Assembly\archiver_web\Project_Code\
SET _subdir=lib\
SET _subsubdir=exports\

cd /D %_mainpath%%_subdir%
 
for  %%f in (*.c) do ( 
   emcc %%f -o %_mainpath%%_subsubdir%%%f.bc
)