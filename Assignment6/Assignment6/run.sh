# # Show a sequence of six images of scene CBspheres.dae rendered with max_ray_depth set to 0, 1, 2, 3, 4, 5, and 100. The other settings should be at least 64 samples per pixel and 4 samples per light.

# ./pathtracer -t 8 -s 1024 -l 16 -m 0 -r 480 360 -f spheres_ray_depth_0.png ../dae/sky/CBspheres.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 1 -r 480 360 -f spheres_ray_depth_1.png ../dae/sky/CBspheres.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 2 -r 480 360 -f spheres_ray_depth_2.png ../dae/sky/CBspheres.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 3 -r 480 360 -f spheres_ray_depth_3.png ../dae/sky/CBspheres.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 4 -r 480 360 -f spheres_ray_depth_4.png ../dae/sky/CBspheres.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 5 -r 480 360 -f spheres_ray_depth_5.png ../dae/sky/CBspheres.dae
# ./pathtracer -t 8 -s 1024 -l 16 -m 100 -r 480 360 -f spheres_ray_depth_100.png ../dae/sky/CBspheres.dae

# # Show a sequence of 4 images of scene CBdragon_microfacet_au.dae rendered with alpha set to 0.005, 0.05, 0.25 and 0.5. The other settings should be at least 128 samples per pixel and 1 samples per light. The number of bounces should be at least 5. Describe the differences between different images. Note that, to change the alpha, just open the .dae file and search for microfacet.

# ./pathtracer -t 8 -s 1024 -l 5 -m 5 -r 480 360 -f CBdragon_microfacet_au_ray_alpha_05.png ../dae/sky/CBdragon_microfacet_au.dae
# ./pathtracer -t 8 -s 1024 -l 5 -m 5 -r 480 360 -f CBdragon_microfacet_au_ray_alpha_005.png ../dae/sky/CBdragon_microfacet_au_005.dae
# ./pathtracer -t 8 -s 1024 -l 5 -m 5 -r 480 360 -f CBdragon_microfacet_au_ray_alpha_025.png ../dae/sky/CBdragon_microfacet_au_025.dae
# ./pathtracer -t 8 -s 1024 -l 5 -m 5 -r 480 360 -f CBdragon_microfacet_au_ray_alpha_0005.png ../dae/sky/CBdragon_microfacet_au_0005.dae

# ./pathtracer -t 8 -s 1024 -l 5 -m 5 -r 480 360 -f CBdragon_microfacet_au_silver.png ../dae/sky/CBdragon_microfacet_au_silver.dae


# ./pathtracer -t 32 -s 4 -l 64 -m 7 -r 800 600 -f bunny_unlit_importance.png -e ../exr/field.exr ../dae/sky/bunny_unlit.dae
# ./pathtracer -t 32 -s 4 -l 64 -m 7 -r 800 600 -f bunny_microfacet_cu_unlit_importance.png -e ../exr/field.exr ../dae/sky/bunny_microfacet_cu_unlit.dae

#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 4.6 -b 0.7 -f CBdragon_microfacet_au_depth_46.png ../dae/sky/CBdragon_microfacet_au.dae
#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 4.8 -b 0.7 -f CBdragon_microfacet_au_depth_48.png ../dae/sky/CBdragon_microfacet_au.dae
#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 5.0 -b 0.7 -f CBdragon_microfacet_au_depth_50.png ../dae/sky/CBdragon_microfacet_au.dae
#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 5.2 -b 0.7 -f CBdragon_microfacet_au_depth_52.png ../dae/sky/CBdragon_microfacet_au.dae

#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 4.6 -b 0.5 -f CBdragon_microfacet_au_b_05.png ../dae/sky/CBdragon_microfacet_au.dae
#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 4.6 -b 0.7 -f CBdragon_microfacet_au_b_07.png ../dae/sky/CBdragon_microfacet_au.dae
#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 4.6 -b 0.9 -f CBdragon_microfacet_au_b_09.png ../dae/sky/CBdragon_microfacet_au.dae
#  ./pathtracer -t 8 -s 32 -l 5 -m 5 -r 480 360 -d 4.6 -b 1.1 -f CBdragon_microfacet_au_b_11.png ../dae/sky/CBdragon_microfacet_au.dae


