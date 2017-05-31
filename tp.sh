printf "deleting old exe file\n"
rm Z_TPGrupal
printf "executing cmake\n"
cmake CMakeLists.txt
printf "executing make\n"
make
printf "Executing generated file\n"
./Z_Server
