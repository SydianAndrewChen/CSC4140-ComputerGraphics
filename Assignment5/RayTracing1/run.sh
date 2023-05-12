# Focus on one particular scene with at least one area light and compare the noise levels in soft shadows when rendering with 1, 4, 16, and 64 light rays (the -l flag) and with 1 sample per pixel (the -s flag) using light sampling, not uniform hemisphere sampling.
./pathtracer -t 8 -s 1 -l 1 -m 6 -r 480 360 -f bunny_rays_1.png ../dae/sky/CBbunny.dae
./pathtracer -t 8 -s 1 -l 4 -m 6 -r 480 360 -f bunny_rays_4.png ../dae/sky/CBbunny.dae
./pathtracer -t 8 -s 1 -l 16 -m 6 -r 480 360 -f bunny_rays_16.png ../dae/sky/CBbunny.dae
./pathtracer -t 8 -s 1 -l 64 -m 6 -r 480 360 -f bunny_rays_64.png ../dae/sky/CBbunny.dae


# # For CBbunny.dae, compare rendered views with max_ray_depth set to 0, 1, 2, 3, and 100 (the -m flag). Use 1024 samples per pixel.

# ./pathtracer -t 8 -s 64 -l 16 -m 0 -r 480 360 -f bunny_ray_depth_0.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 64 -l 16 -m 1 -r 480 360 -f bunny_ray_depth_1.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 64 -l 16 -m 2 -r 480 360 -f bunny_ray_depth_2.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 64 -l 16 -m 3 -r 480 360 -f bunny_ray_depth_3.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 64 -l 16 -m 4 -r 480 360 -f bunny_ray_depth_4.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 64 -l 16 -m 100 -r 480 360 -f bunny_ray_depth_100.png ../dae/sky/CBbunny.dae

# # Pick one scene and compare rendered views with various sample-per-pixel rates, including at least 1, 2, 4, 8, 16, 64, and 1024. Use 4 light rays.

# ./pathtracer -t 8 -s 1 -l 16 -m 6 -r 480 360 -f bunny_sampleppx_1.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 2 -l 16 -m 6 -r 480 360 -f bunny_sampleppx_2.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 4 -l 16 -m 6 -r 480 360 -f bunny_sampleppx_4.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 8 -l 16 -m 6 -r 480 360 -f bunny_sampleppx_8.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 16 -l 16 -m 6 -r 480 360 -f bunny_sampleppx_16.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 64 -l 16 -m 6 -r 480 360 -f bunny_sampleppx_64.png ../dae/sky/CBbunny.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 6 -r 480 360 -f bunny_sampleppx_1024.png ../dae/sky/CBbunny.dae


# # Show some images rendered with global (direct and indirect) illumination. Use 1024 samples per pixel.
# ./pathtracer -t 8 -s 1024 -l 16 -m 6 -r 480 360 -f CBspheres_lambertian.png ../dae/sky/CBspheres_lambertian.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 6 -r 480 360 -f CBcoil.png ../dae/sky/CBcoil.dae

# # Pick one scene and render it with at least 2048 samples per pixel. Show a good sampling rate image with clearly visible differences in sampling rate over various regions and pixels. Include both your sample rate image, which shows your how your adaptive sampling changes depending on which part of the image you are rendering, and your noise-free rendered result. Use 1 sample per light and at least 5 for max ray depth.
# ./pathtracer -t 1 -s 2048 -l 16 -m 5 -r 480 360 -f bunny_adaptive_sample.png ../dae/sky/CBbunny.dae