
mkdir -p build
cd build
cmake ..
make

./Assignment2 square 10 ../results/result_square.jpg
./Assignment2 triangle 20 ../results/result_triangle.jpg
./Assignment2 circle -30 ../results/result_circle.jpg
./Assignment2 cube 30 ../results/result_cube.jpg

./Assignment2 ../obj/african_head.obj -10 ../results/result_african_head.jpg
./Assignment2 ../obj/diablo3_pose.obj 10 ../results/result_diablo3_pose.jpg