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


    sample_buffer[y * width + x] = c;
  }

  // Rasterize a point: simple example to help you start familiarizing
  // yourself with the starter code.
  //
  void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width) return;
    if (sy < 0 || sy >= height) return;

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
    
    float u = 
    (((y1-y0)*(y1-y0)+(x1-x0)*(x1-x0)) * ((yp-y0)*(y2-y0)+(xp-x0)*(x2-x0)) - ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)) * ((yp-y0)*(y1-y0)+(xp-x0)*(x1-x0)))/
    (((y2-y0)*(y2-y0)+(x2-x0)*(x2-x0)) * ((y1-y0)*(y1-y0)+(x1-x0)*(x1-x0)) - ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)) * ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)));

    float v = 
    (((y2-y0)*(y2-y0)+(x2-x0)*(x2-x0)) * ((yp-y0)*(y1-y0)+(xp-x0)*(x1-x0)) - ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)) * ((yp-y0)*(y2-y0)+(xp-x0)*(x2-x0)))/
    (((y2-y0)*(y2-y0)+(x2-x0)*(x2-x0)) * ((y1-y0)*(y1-y0)+(x1-x0)*(x1-x0)) - ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)) * ((y2-y0)*(y1-y0)+(x2-x0)*(x1-x0)));

    bary[0] = u;
    bary[1] = v;
    bary[2] = 1. - u - v;
  }

  // Rasterize a triangle.
  void RasterizerImp::rasterize_triangle(float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    Color color) {
    // TODO: Task 1: Implement basic triangle rasterization here, no supersampling
    if (y0 == y1 && y1 == y2) return;

    if (y0 > y1) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }
    if (y0 > y2) {
      std::swap(x0, x2);
      std::swap(y0, y2);
    }
    if (y1 > y2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }

    int total_height = y2 - y0;
    for (int i = 0; i < total_height; i++)
    {
      bool lower_half = (i > y1 - y0) || (y1 == y0);
      int segment_height = 1.0 + (lower_half ? y2-y1 : y1-y0);
      float alpha = (float)i/total_height;
      float beta  = (float)(i - (lower_half ? y1-y0:0))/segment_height;
      int xa = x0 + (x2-x0)*alpha;
      int xb = lower_half ? (x1 + (x2 - x1)*beta):(x0 + (x1 - x0)*beta);
      // int ya = y0 + (y2-y0)*alpha;
      // int yb = lower_half ? (y1 + (y2 - y1)*beta):(y0 + (y1 - y0)*beta);
      if (xa > xb){
        std::swap(xa, xb);
        // std::swap(ya, yb);
      }
      for (int j = xa; j <= xb; j++)
      {
        int curr_x = j;
        int curr_y = y0+i;

        // int step_total = sqrt(sample_rate);
        // float step_len = 1.0 / (step_total + 1);
        // float start_x = curr_x - 0.5 + step_len;
        // float start_y = curr_y - 0.5 + step_len;

        // int valid_cnt = 0;
        // for (int x_step = 0; x_step < step_total; x_step++)
        // {
        //   float super_sample_x = start_x + x_step * step_len;
        //   for (int y_step = 0; y_step < step_total; y_step++)
        //   {
        //     float super_sample_y = start_y + y_step * step_len; 
        //     float bary[3];
        //     calc_bary(x0, y0, x1, y1, x2, y2, super_sample_x, super_sample_y, bary);
        //     if (bary[0] > 0 && bary[0] < 1 && bary[1] > 0 && bary[1] < 1 && bary[2] > 0 && bary[2] < 1) valid_cnt++;
        //   }
          
        // }

        // // float bary[3];
        // // calc_bary(x0, y0, x1, y1, x2, y2, curr_x, curr_y, bary);
        // // rasterize_point(j, y0 + i, Color(bary[0], bary[1], bary[2]));
        // float s = (float)valid_cnt/sample_rate;
        // rasterize_point(curr_x, curr_y, color * s + bg_color * (1.0-s));
        rasterize_point(curr_x, curr_y, color);
        
        
        

      }
      
    }
    

    // TODO: Task 2: Update to implement super-sampled rasterization



  }


  void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
    float x1, float y1, Color c1,
    float x2, float y2, Color c2)
  {
    // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
    // Hint: You can reuse code from rasterize_triangle

    if (y0 == y1 && y1 == y2) return;

    if (y0 > y1) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }
    if (y0 > y2) {
      std::swap(x0, x2);
      std::swap(y0, y2);
    }
    if (y1 > y2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }

    int total_height = y2 - y0;
    for (int i = 0; i < total_height; i++)
    {
      bool lower_half = (i > y1 - y0) || (y1 == y0);
      int segment_height = 1.0 + (lower_half ? y2-y1 : y1-y0);
      float alpha = (float)i/total_height;
      float beta = (float)(i - (lower_half ? y1-y0:0))/segment_height;
      int xa = x0 + (x2-x0)*alpha;
      int xb = lower_half ? (x1 + (x2 - x1)*beta):(x0 + (x1 - x0)*beta);
      // int ya = y0 + (y2-y0)*alpha;
      // int yb = lower_half ? (y1 + (y2 - y1)*beta):(y0 + (y1 - y0)*beta);
      if (xa > xb){
        std::swap(xa, xb);
        // std::swap(ya, yb);
      }
      for (int j = xa; j <= xb; j++)
      {
        int curr_x = j;
        int curr_y = y0+i;

        // int step_total = sqrt(sample_rate);
        // float step_len = 1.0 / (step_total + 1);
        // float start_x = curr_x - 0.5 + step_len;
        // float start_y = curr_y - 0.5 + step_len;

        // int valid_cnt = 0;
        // for (int x_step = 0; x_step < step_total; x_step++)
        // {
        //   float super_sample_x = start_x + x_step * step_len;
        //   for (int y_step = 0; y_step < step_total; y_step++)
        //   {
        //     float super_sample_y = start_y + y_step * step_len; 
        //     float bary[3];
        //     calc_bary(x0, y0, x1, y1, x2, y2, super_sample_x, super_sample_y, bary);
        //     if (bary[0] > 0 && bary[0] < 1 && bary[1] > 0 && bary[1] < 1 && bary[2] > 0 && bary[2] < 1) valid_cnt++;
        //   }
          
        // }

        float bary[3];
        calc_bary(x0, y0, x1, y1, x2, y2, curr_x, curr_y, bary);
        auto c = c0 * bary[0] + c1 * bary[1] + c2 * bary[2];
        rasterize_point(j, y0 + i, c);
        // float s = (float)valid_cnt/sample_rate;
        // rasterize_point(curr_x, curr_y, color * s + bg_color * (1.0-s));
        
        
        

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




  }

  void RasterizerImp::set_sample_rate(unsigned int rate) {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->sample_rate = rate;


    this->sample_buffer.resize(width * height, Color::White);
  }


  void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
    size_t width, size_t height)
  {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;


    this->sample_buffer.resize(width * height, Color::White);
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


    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        Color col = sample_buffer[y * width + x];

        for (int k = 0; k < 3; ++k) {
          this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&col.r)[k] * 255;
        }
      }
    }

  }

  Rasterizer::~Rasterizer() { }


}// CGL
