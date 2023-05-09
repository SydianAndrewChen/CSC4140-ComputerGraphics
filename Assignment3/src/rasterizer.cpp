#include "rasterizer.h"

using namespace std;

namespace CGL {

  RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
    size_t width, size_t height,
    unsigned int sample_rate) {
    this->psm = psm;
    this->lsm = lsm;
    this->width = width;
    this->height = height;
    this->sample_rate = sample_rate;
    this->bg_color = Color::White;

    sample_buffer.resize(width * height * sample_rate, this->bg_color);
  }

  // Used by rasterize_point and rasterize_line
  void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
    // TODO: Task 2: You might need to this function to fix points and lines (such as the black rectangle border in test4.svg)
    // NOTE: You are not required to implement proper supersampling for points and lines
    // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)


    sample_buffer[y * width * static_cast<int>(sqrt(sample_rate)) + x] = c;
  }

  // Rasterize a point: simple example to help you start familiarizing
  // yourself with the starter code.
  //
  void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width * sqrt(sample_rate)) return;
    if (sy < 0 || sy >= height * sqrt(sample_rate)) return;

    fill_pixel(sx, sy, color);
    return;
  }

  // Rasterize a line.
  void RasterizerImp::rasterize_line(float x0, float y0,
    float x1, float y1,
    Color color) {
    if (x0 > x1) {
      swap(x0, x1); swap(y0, y1);
    }

    float pt[] = { x0,y0 };
    float m = (y1 - y0) / (x1 - x0);
    float dpt[] = { 1,m };
    int steep = abs(m) > 1;
    if (steep) {
      dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
      dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
    }

    while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
      rasterize_point(pt[0], pt[1], color);
      pt[0] += dpt[0]; pt[1] += dpt[1];
    }
  }

  void calc_bary(float x0, float y0, 
    float x1, float y1, 
    float x2, float y2, 
    float xp, float yp,
    float * bary) {
    float div = (((y2-y0)*(y2-y0)+(x2-x0)*(x2-x0)) * ((y1-y0)*(y1-y0)+(x1-x0)*(x1-x0)) - ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)) * ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)));
    float u = (((y1-y0)*(y1-y0)+(x1-x0)*(x1-x0)) * ((yp-y0)*(y2-y0)+(xp-x0)*(x2-x0)) - ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)) * ((yp-y0)*(y1-y0)+(xp-x0)*(x1-x0)))/div;
    float v = (((y2-y0)*(y2-y0)+(x2-x0)*(x2-x0)) * ((yp-y0)*(y1-y0)+(xp-x0)*(x1-x0)) - ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)) * ((yp-y0)*(y2-y0)+(xp-x0)*(x2-x0)))/div;

    bary[0] = 1.-u-v;
    bary[1] = v;
    bary[2] = u;
  }

  // Rasterize a triangle.
  void RasterizerImp::rasterize_triangle(float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    Color color) {
    // TODO: Task 1: Implement basic triangle rasterization here, no supersampling
    
    // int x_min = std::min(std::min(x0, x1), x2);
    // int y_min = std::min(std::min(y0, y1), y2);
    // int x_max = std::max(std::max(x0, x1), x2);
    // int y_max = std::max(std::max(y0, y1), y2);
    // x_min = std::max(std::min(x_min, (int)this->width), 0);
    // y_min = std::max(std::min(y_min, (int)this->height), 0);
    // x_max = std::max(std::min(x_max, (int)this->width), 0);
    // y_max = std::max(std::min(y_max, (int)this->height), 0);
    
    // for (int x = x_min; x <= x_max; x++)
    // {
    //   float bary[3];
    //   for (int y = y_min; y <= y_max; y++)
    //   { 
    //     calc_bary(x0, y0, x1, y1, x2, y2, x, y, bary);
    //     if (bary[0] >= 0 && bary[0] <= 1 && bary[1] >= 0 && bary[1] <= 1 && bary[2] >= 0 && bary[2] <= 1){
    //       rasterize_point(x, y, color);
    //     }
    //   } 
    // }


    // TODO: Task 2: Update to implement super-sampled rasterization
    int s = static_cast<int>(sqrt(sample_rate));
    int x_min = std::min(std::min(x0, x1), x2);
    int y_min = std::min(std::min(y0, y1), y2);
    int x_max = std::max(std::max(x0, x1), x2);
    int y_max = std::max(std::max(y0, y1), y2);
    x_min = s * std::max(std::min(x_min, (int)this->width), 0);
    y_min = s * std::max(std::min(y_min, (int)this->height), 0);
    x_max = s * std::max(std::min(x_max, (int)this->width), 0);
    y_max = s * std::max(std::min(y_max, (int)this->height), 0);
    
    for (int x = x_min; x <= x_max; x++)
    {
      float bary[3];
      for (int y = y_min; y <= y_max; y++)
      { 
        calc_bary(x0 * s, y0 * s, x1 * s, y1 * s, x2 * s, y2 * s, x, y, bary);
        if (bary[0] >= 0 && bary[0] <= 1 && bary[1] >= 0 && bary[1] <= 1 && bary[2] >= 0 && bary[2] <= 1){
          rasterize_point(x, y, color);
        }
      } 
    }

  }


  void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
    float x1, float y1, Color c1,
    float x2, float y2, Color c2)
  {
    // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
    // Hint: You can reuse code from rasterize_triangle
    int s = static_cast<int>(sqrt(sample_rate));
    int x_min = std::min(std::min(x0, x1), x2);
    int y_min = std::min(std::min(y0, y1), y2);
    int x_max = std::max(std::max(x0, x1), x2);
    int y_max = std::max(std::max(y0, y1), y2);
    x_min = s * std::max(std::min(x_min, (int)this->width), 0);
    y_min = s * std::max(std::min(y_min, (int)this->height), 0);
    x_max = s * std::max(std::min(x_max, (int)this->width), 0);
    y_max = s * std::max(std::min(y_max, (int)this->height), 0);
    
    for (int x = x_min; x <= x_max; x++)
    {
      float bary[3];
      for (int y = y_min; y <= y_max; y++)
      { 
        calc_bary(x0 * s, y0 * s, x1 * s, y1 * s, x2 * s, y2 * s, x, y, bary);
        float eps = 1e-3;
        if (bary[0] > 0-eps && bary[0] < 1+eps && bary[1] > 0-eps && bary[1] < 1+eps && bary[2] > 0-eps && bary[2] < 1+eps){
          // rasterize_point(x, y, Color(bary[0], bary[1], bary[2]));
          rasterize_point(x, y, c0 * bary[0] + c1 * bary[1] + c2 * bary[2]);
        }
      } 
    }
  }


  void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
    float x1, float y1, float u1, float v1,
    float x2, float y2, float u2, float v2,
    Texture& tex)
  {
    // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
    // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
    // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle

    int s = static_cast<int>(sqrt(sample_rate));
    int x_min = std::min(std::min(x0, x1), x2);
    int y_min = std::min(std::min(y0, y1), y2);
    int x_max = std::max(std::max(x0, x1), x2);
    int y_max = std::max(std::max(y0, y1), y2);
    x_min = s * std::max(std::min(x_min, (int)this->width), 0);
    y_min = s * std::max(std::min(y_min, (int)this->height), 0);
    x_max = s * std::max(std::min(x_max, (int)this->width), 0);
    y_max = s * std::max(std::min(y_max, (int)this->height), 0);
    
    for (int x = x_min; x <= x_max; x++)
    {
      float bary[3];
      for (int y = y_min; y <= y_max; y++)
      { 
        calc_bary(x0 * s, y0 * s, x1 * s, y1 * s, x2 * s, y2 * s, x, y, bary);
        float eps = 1e-3;
        // float eps = 0;
        if (bary[0] > 0-eps && bary[0] < 1+eps && bary[1] > 0-eps && bary[1] < 1+eps && bary[2] > 0-eps && bary[2] < 1+eps){
          SampleParams sample_params;
          sample_params.lsm = this->lsm;
          sample_params.psm = this->psm;
          sample_params.p_uv = Vector2D(bary[0] * u0 + bary[1] * u1 + bary[2] * u2, bary[0] * v0 + bary[1] * v1 + bary[2] * v2);
          
          // \todo: Here, we should check different neighbours in four directions to find the most reasonable one. 
          //dx
          calc_bary(x0 * s, y0 * s, x1 * s, y1 * s, x2 * s, y2 * s, x+1, y, bary);
          sample_params.p_dx_uv = Vector2D(bary[0] * u0 + bary[1] * u1 + bary[2] * u2, bary[0] * v0 + bary[1] * v1 + bary[2] * v2);

          //dy
          calc_bary(x0 * s, y0 * s, x1 * s, y1 * s, x2 * s, y2 * s, x, y+1, bary);
          sample_params.p_dy_uv = Vector2D(bary[0] * u0 + bary[1] * u1 + bary[2] * u2, bary[0] * v0 + bary[1] * v1 + bary[2] * v2);
          
          rasterize_point(x, y, tex.sample(sample_params));
        }
      } 
    }


  }

  void RasterizerImp::set_sample_rate(unsigned int rate) {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->sample_rate = rate;


    this->sample_buffer.resize(width * height * sample_rate, Color::White);
  }


  void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
    size_t width, size_t height)
  {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;


    this->sample_buffer.resize(width * height * sample_rate, Color::White);
  }


  void RasterizerImp::clear_buffers() {
    std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
    std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
  }


  // This function is called at the end of rasterizing all elements of the
  // SVG file.  If you use a supersample buffer to rasterize SVG elements
  // for antialising, you could use this call to fill the target framebuffer
  // pixels from the supersample buffer data.
  //
  void RasterizerImp::resolve_to_framebuffer() {
    // TODO: Task 2: You will likely want to update this function for supersampling support

    int s = static_cast<int>(sqrt(sample_rate));
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        Color col(0., 0., 0.);

        for (int i = 0; i < s; ++i)
          for (int j = 0; j < s; ++j)
          {
            int index = (y*s+j)*width*s+x*s+i;
            col += sample_buffer[index];
          }
        col = col * (1. / static_cast<float>(sample_rate));
        for (int k = 0; k < 3; ++k) {
          this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&col.r)[k] * 255;
        }
      }
    }
  }

  Rasterizer::~Rasterizer() { }


}// CGL
