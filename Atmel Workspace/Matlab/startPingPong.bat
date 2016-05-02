set currentPath=%~dp0
matlab -nodisplay -minimize -nosplash -nodesktop -r cd('%currentPath%');pingPong;