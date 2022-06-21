for /R "./sources" %f in (*.h) do xcopy "%f" "./sourcesinclude" /I /Y
xcopy ".\sources\*.h" ".\sourcesinclude"  /sy
pause