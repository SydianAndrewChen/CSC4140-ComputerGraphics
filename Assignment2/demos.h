#ifndef DEMOS_H
#define DEMOS_H 

#include <fstream>
#include "Triangle.hpp"

struct Mesh{
    std::vector<Eigen::Vector3f> pos; 
    std::vector<Eigen::Vector3i> ind; 

    void load_obj(const char * filename){
        std::ifstream in;
        in.open(filename, std::ifstream::in);
        if (in.fail()) return;
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash;
                Eigen::Vector3f v;
                for (int i = 0; i < 3; i++) iss >> v[i];
                pos.push_back(v);
            }
            else if (!line.compare(0, 2, "f ")) {
                std::vector<int> f;
                int vert, uv, norm;
                iss >> trash;
                while (iss >> vert >> trash >> uv >> trash >> norm) {
                    f.push_back(vert - 1);
                }
                ind.push_back(Eigen::Vector3i(f[0], f[1], f[2]));
            }
        }
    }

};


Mesh Triangle(){
    Mesh res;
    res.pos = {Eigen::Vector3f(0.5, 0.0, 1.0), Eigen::Vector3f(-0.5, 0.0, 1.0), Eigen::Vector3f(0.5, 0.5, 1.0)};
    res.ind = {Eigen::Vector3i(0, 1, 2)};
    return res;
}

Mesh Square(){
    Mesh res;
    res.pos = {
        Eigen::Vector3f( 0.5,  0.5,  0.0), 
        Eigen::Vector3f(-0.5,  0.5,  0.0), 
        Eigen::Vector3f( 0.5, -0.5,  0.0),
        Eigen::Vector3f(-0.5, -0.5,  0.0)
    };
    res.ind = {
        Eigen::Vector3i(0, 1, 2),
        Eigen::Vector3i(1, 2, 3)
    };
    return res;

}

Mesh Circle(){
    const double PI = 3.1415926;
    int side_num = 50;
    float radius = 1.0f;
    Mesh res;
    res.pos.resize(side_num + 1);
    res.ind.resize(side_num);
    res.pos[0] = Eigen::Vector3f(0., 0., 0.);
    for (size_t i = 0; i < side_num; i++)
    {
        res.pos[i+1] = Eigen::Vector3f(sin(2 * PI * static_cast<float>(i) / side_num), cos(2 * PI * static_cast<float>(i) / side_num), 0.0);
        res.ind[i] = Eigen::Vector3i(0, i%side_num+1, (i+1)%side_num+1);
    }
    return res;
}

Mesh Cube(){
    // \ref: https://stackoverflow.com/questions/58772212/what-are-the-correct-vertices-and-indices-for-a-cube-using-this-mesh-function
    
    Mesh res;
    
    res.ind = {
        //Top
        Eigen::Vector3i(2, 6, 7),
        Eigen::Vector3i(2, 3, 7),

        //Bottom
        Eigen::Vector3i(0, 4, 5),
        Eigen::Vector3i(0, 1, 5),

        //Left
        Eigen::Vector3i(0, 2, 6),
        Eigen::Vector3i(0, 4, 6),

        //Right
        Eigen::Vector3i(1, 3, 7),
        Eigen::Vector3i(1, 5, 7),

        //Front
        Eigen::Vector3i(0, 2, 3),
        Eigen::Vector3i(0, 1, 3),

        //Back
        Eigen::Vector3i(4, 6, 7),
        Eigen::Vector3i(4, 5, 7)
    };

    res.pos = {
        Vector3f(-0.5, -0.5,  0.5), //0
        Vector3f( 0.5, -0.5,  0.5), //1
        Vector3f(-0.5,  0.5,  0.5), //2
        Vector3f( 0.5,  0.5,  0.5), //3
        Vector3f(-0.5, -0.5, -0.5), //4
        Vector3f( 0.5, -0.5, -0.5), //5
        Vector3f(-0.5,  0.5, -0.5), //6
        Vector3f( 0.5,  0.5, -0.5)  //7
    };
    return res;
}

Mesh LoadObj(const char * filename){
    Mesh res;
    res.load_obj(filename);
    return res;
}
#endif