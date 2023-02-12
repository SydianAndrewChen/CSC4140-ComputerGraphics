#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

#include "demos.h"
// add some other header files you need

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;
    // std::clog << "view" << std::endl << view << std::endl;  // check data

    return view;
}


Eigen::Matrix4f get_model_matrix(float rotation_angle, Eigen::Vector3f T, Eigen::Vector3f S, Eigen::Vector3f P0, Eigen::Vector3f P1)
{

    //Step 1: Build the Translation Matrix M_trans:
    Eigen::Matrix4f M_trans = Eigen::Matrix4f::Identity();
    M_trans << 1, 0, 0, T[0], 0, 1, 0, T[1], 0, 0, 1, T[2], 0, 0, 0, 1; 

    //Step 2: Build the Scale Matrix S_trans:
    Eigen::Matrix4f S_trans = Eigen::Matrix4f::Identity();
    S_trans(0, 0) *= S(0);
    S_trans(1, 1) *= S(1);
    S_trans(2, 2) *= S(2);

    //Step 3: Implement Rodrigues' Rotation Formular, rotation by angle theta around axis u, then get the model matrix
	// The axis u is determined by two points, u = P1-P0: Eigen::Vector3f P0 ,Eigen::Vector3f P1  
    // Create the model matrix for rotating the triangle around a given axis. // Hint: normalize axis first
    Eigen::Matrix4f R_trans = Eigen::Matrix4f::Identity();
    Eigen::Vector3f axis = (P1 - P0).normalized();
    float radian = rotation_angle * MY_PI / 180.0;

    R_trans <<     
    cos(radian)+(1-cos(radian))*axis(0)*axis(0),            -sin(radian)*axis(2)+(1-cos(radian))*axis(0)*axis(1),   sin(radian)*axis(1)+(1-cos(radian))*axis(0)*axis(2),    0,
    sin(radian)*axis(2)+(1-cos(radian))*axis(0)*axis(1),    cos(radian)+(1-cos(radian))*axis(1)*axis(1),            -sin(radian)*axis(0)+(1-cos(radian))*axis(1)*axis(2),   0,
    -sin(radian)*axis(1)+(1-cos(radian))*axis(0)*axis(2),   sin(radian)*axis(0)+(1-cos(radian))*axis(1)*axis(2),    cos(radian)+(1-cos(radian))*axis(2)*axis(2),            0,
    0, 0, 0, 1;

	//Step 4: Use Eigen's "AngleAxisf" to verify your Rotation
	Eigen::AngleAxisf rotation_vector(radian, Vector3f(axis[0], axis[1], axis[2]));  
	Eigen::Matrix3f rotation_matrix;
	Eigen::Matrix3f rotation_m = rotation_vector.toRotationMatrix();

    // std::cout << R_trans << std::endl;
    // std::cout << rotation_m << std::endl;

    Eigen::Matrix4f model = M_trans * R_trans * S_trans;

	return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    float radian = eye_fov * MY_PI / 180;
    projection(0, 0) *= (1.0 / (aspect_ratio * tan(radian / 2)));
    projection(1, 1) *= (1.0 / tan(radian / 2));
    projection(2, 2) *= -(zFar + zNear) / (zFar - zNear);
    projection(2, 3) *= -2 * zNear * zFar /(zFar - zNear);
    projection(3, 2) = -1;
    projection(3, 3) = 0;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "result.png";
    
    Mesh m;
    if (strcmp(argv[1], "triangle") == 0) {
        m = Triangle();
    }
    else if (strcmp(argv[1], "square") == 0) {
        m = Square();
    }
    else if (strcmp(argv[1], "circle") == 0) {
        m = Circle();
    }
    else if (strcmp(argv[1], "cube") == 0) {
        m = Cube();
    }
    else{
        std::ifstream in;
        in.open(argv[1], std::ifstream::in);
        if (in.fail()){
            std::cout << "No obj file found." << std::endl;
            return -1;
        }
        m = LoadObj(argv[1]);
    }

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(1024, 1024);

    // define your eye position "eye_pos" to a proper position
    Eigen::Vector3f eye_pos(0,0,2);

    // define a triangle named by "pos" and "ind"

    auto pos_id = r.load_positions(m.pos);
    auto ind_id = r.load_indices(m.ind);

    int key = 0;
    int frame_count = 0;

    // added parameters for get_projection_matrix(float eye_fov, float aspect_ratio,float zNear, float zFar)
    float eye_fov = 90;
    float aspect_ratio = 1.0;
    float zNear = 0.01;
    float zFar = 100.0;

    // Eigen::Vector3f axis(0, 0, 1);
    Eigen::Vector3f axis(0, 1, 0);
    
    Eigen::Vector3f T(0, 0, 0);
    Eigen::Vector3f S(1, 1, 1);
    Eigen::Vector3f P0(0, 0, 0);
    Eigen::Vector3f P1 = P0 + axis;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }
    
    while (key != 27) { // Escape
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        // std::cout << "frame count: " << frame_count++ << '\n';
        // std::clog << "angle: " << angle << std::endl;
    

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
