#include <iostream>
#include <eigen3/Eigen/Dense>
// #include <eigen3/Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

int main(){
    
    /* Question 1 */
    std::cout << "****************\nQuestion1\n****************" << std::endl; 
    Eigen::Vector4f v(1, 1.5, 2, 3);
    Eigen::Vector4f w(0, 1, 2, 4);
    std::cout << "v + w: \n" << v + w << std::endl;
    std::cout << "v * w = \n" << v.hnormalized().dot(w.hnormalized()) << std::endl;
    std::cout << "v x w = \n" << v.hnormalized().cross(w.hnormalized()) << std::endl;

    /* Question 2 */
    std::cout << "****************\nQuestion2\n****************" << std::endl; 
    Eigen::Matrix4f m1(4,4);
    Eigen::Matrix4f m2(4,4);
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            m1(i,j) = i * 4 + j+1;
            m2(i,j) = i * 4 + 4-j;
        }
    }
    std::cout << "m1\n" << m1 << std::endl;
    std::cout << "m2\n" << m2 << std::endl;
    std::cout << "m1 + m2 \n" << m1 + m2 << std::endl; 
    std::cout << "m1 * m2 \n" << m1 * m2 << std::endl;
    std::cout << "m1 * v \n" << m1 * v << std::endl; 


    /* Question 3 */
    std::cout << "****************\nQuestion3\n****************" << std::endl; 
    cv::Mat image;
    image = cv::imread( "../img/input/lenna.png");
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    cv::normalize(image, image, 1.0, 0.0, cv::NORM_MINMAX);
    
    cv::Mat img = image.reshape(1, 0);
    Eigen::MatrixXd mat;
    cv::cv2eigen(img, mat);

    Eigen::BDCSVD<Eigen::MatrixXd> svd(mat, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::MatrixXd U = svd.matrixU();
    Eigen::MatrixXd V = svd.matrixV();
    Eigen::MatrixXd S = U.inverse() * mat * V.transpose().inverse();
    
    Eigen::MatrixXd mat1 = U * S.block<512, 1> (0, 0) * V.transpose().block<1, 512>(0,0);
    cv::Mat img1;
    cv::eigen2cv(mat1, img1);
    cv::imwrite("../img/output/lenna_1.png", img1 * 255);
    
    Eigen::MatrixXd mat10 = U * S.block<512, 10> (0, 0) * V.transpose().block<10, 512>(0,0);
    cv::Mat img10;
    cv::eigen2cv(mat10, img10);
    cv::imwrite("../img/output/lenna_10.png", img10 * 255);

    Eigen::MatrixXd mat50 = U * S.block<512, 50> (0, 0) * V.transpose().block<50, 512>(0,0);
    cv::Mat img50;
    cv::eigen2cv(mat50, img50);
    cv::imwrite("../img/output/lenna_50.png", img50 * 255);
    std::cout << "All images written into path: img/output/\n";

    /* Question 4 */
    std::cout << "****************\nQuestion4\n****************" << std::endl; 
    Eigen::Vector3d Point1(1,2,3);

    Eigen::AngleAxisd rollAngle(60, Eigen::Vector3d::UnitZ());
    Eigen::AngleAxisd yawAngle(30, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd pitchAngle(45, Eigen::Vector3d::UnitX());
    Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;
    Eigen::Matrix3d rotationMatrix = q.matrix();

    Eigen::Vector3d center(4,5,6);

    Eigen::Vector3d rotatedPoint1 = rotationMatrix*(Point1 - center) + center;
    std::cout << "Rotated Point1 \n" << rotatedPoint1 << std::endl; 

}