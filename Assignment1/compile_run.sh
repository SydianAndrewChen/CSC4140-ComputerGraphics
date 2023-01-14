mkdir -p build
rm -r img/output/*
cd build
cmake ..
make
./Assignment1
cd ..