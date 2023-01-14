#include <iostream>
#include <eigen3/Eigen/Dense>
// #include <eigen3/Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

int main(){
    
    /* Section 2.1 */
    Eigen::Vector4f v(1, 1.5, 2, 3);
    Eigen::Vector4f w(0, 1, 2, 4);
    // std::cout << "v: \n" << v << std::endl;
    // std::cout << "w: \n" << w << std::endl;
    std::cout << "v + w: \n" << v + w << std::endl;
    std::cout << "v.transpose() * w \n" << v.transpose() * w << std::endl; 
    std::cout << "v * w.transpose() \n" << v * w.transpose() << std::endl; 

    /* Section 2.2 */
    Eigen::Matrix4f m1(4,4);
    Eigen::Matrix4f m2(4,4);
    float m1_arr[16];
    float m2_arr[16];
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            m1(i,j) = i * 4 + j+1;
            m2(i,j) = i * 4 + 4-j;
        }
    }
    std::cout << m1 << std::endl;
    std::cout << m2 << std::endl;
    std::cout << "m1 + m2 \n" << m1 + m2 << std::endl; 
    std::cout << "m1 * m2 \n" << m1 * m2 << std::endl;
    std::cout << "m1 * v \n" << m1 * v << std::endl; 

    cv::Mat image;
    image = cv::imread( "./lenna.png");
    // image.convertTo(image, CV_32F);
    // cv::normalize(image, image, 1.0, 0.0, cv::NORM_MINMAX);
    // Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> eigen_image;
    // eigen_image.resize(image.rows, image.cols);
    // cv::cv2eigen(image, eigen_image);

    int chs = image.channels();
    
    cv::Mat img = image.reshape(1, 0);
    std::cout << img.size() << ", " << img.channels() << std::endl;
    // typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixXd;
    Eigen::MatrixXd mat;

    cv::cv2eigen(img, mat);
    cv::Mat img2;
    cv::eigen2cv(mat, img2);
    img2 = img2.reshape(chs, 0);
    cv::imshow("doube color image ", img2*(1.0/255));
    cv::imwrite("dst_double.png", img2);

    // Eigen::JacobiSVD<Eigen::MatrixXf, Eigen::ComputeThinU | Eigen::ComputeThinV> svd(mat);
    // Eigen::MatrixXf U = svd.matrixU();
    // Eigen::MatrixXf V = svd.matrixV();
    // Eigen::MatrixXf S = svd.singularValues();
    // auto img3 = U * S(Eigen::all, Eigen::seqN(0, 1)) * V(Eigen::all, Eigen::seqN(0, 1));
    // std::cout << "image \t\t" << image << std::endl; 
    // Eigen::MatrixXf img(image.rows, image.cols);
    // std::cout << "img.cols() \t\t" << img.cols() << std::endl; 
    // std::cout << "image.type() \t\t" << image.type() << std::endl; 
    // // std::cout << "img.dtype() \t\t" << img.() << std::endl; 
    // Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> img(image.data, image.rows, image.cols);

}