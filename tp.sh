printf "executing cmake\n"
cmake CMakeLists.txt
printf "executing make\n"
make
printf "Executing generated file\n"
./Z_TPGrupal
