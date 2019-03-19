// Copyright 2019 Konakov Alexandr

#include <omp.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

#define PGM_IMAGE_WHITE 255
#define PGM_IMAGE_BLACK 0

#define ushort uint16_t
#define pixel uint16_t

struct pgm_image {
  int width;
  int height;
  int depth;
  pixel *pixels;
};

int pgm_image_create(struct pgm_image *img, int nrows, int ncols,
                     pixel color = 0, int depth = 255) {
  img->width = ncols;
  img->height = nrows;
  img->depth = depth;
  int img_size = nrows * ncols;
  img->pixels = new pixel[img_size];
  memset(reinterpret_cast<void *>(img->pixels), color,
         img_size * sizeof(pixel));
  return 0;
}

int pgm_image_destroy(struct pgm_image *img) {
  if (img->pixels != nullptr) delete[] img->pixels;
  return 0;
}

int pgm_image_copy(struct pgm_image *img_to, struct pgm_image *img_from) {
  if ((img_from == nullptr) || (img_to == nullptr)) return -1;

  int status = pgm_image_create(img_to, img_from->height, img_from->width,
                                img_from->depth);

  memcpy(img_to->pixels, img_from->pixels,
         img_from->height * img_from->width * sizeof(pixel));

  return status;
}

int pgm_image_set_pixel(struct pgm_image *img, int r, int c, pixel color) {
  if ((img == nullptr) || r > img->height || c > img->width) return -1;
  img->pixels[r * img->width + c] = color;
  return 0;
}

pixel pgm_image_get_pixel(struct pgm_image *img, int r, int c) {
  if ((r > img->height) || (c > img->width)) return -1;
  return img->pixels[r * img->width + c];
}

double range_map(double value, double val_min, double val_max, double range_min,
                 double range_max) {
  return range_min +
         (value - val_min) * (range_max - range_min) / (val_max - val_min);
}

int pgm_image_normalize(pgm_image *img) {
  double max_value = 0.0;
  for (int i = 0; i < (img->width * img->height); ++i)
    if (img->pixels[i] > max_value) max_value = img->pixels[i];

  for (int i = 0; i < (img->width * img->height); ++i)
    img->pixels[i] = static_cast<uint16_t>(
        range_map(img->pixels[i], 0, max_value, 0, img->depth));
  return 0;
}

double gaussian(double x, double y, double sigma = 0.6) {
  const double pi = 3.1415972;
  return exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * pi * sigma * sigma);
}

int constrain(int min, int val, int max) {
  return min > val ? min : (max < val ? max : val);
}

int pgm_image_convolution_filter(pgm_image *img_dest, pgm_image *img_src,
                                 double *conv_mat = nullptr, int cm_size = 5) {
  pgm_image_create(img_dest, img_src->height, img_src->width, PGM_IMAGE_BLACK);

  for (int r = 0; r < img_src->height; ++r)
#pragma omp parallel for
    for (int c = 0; c < img_src->width; ++c) {
      for (int cmr = 0; cmr < cm_size; ++cmr)
        for (int cmc = 0; cmc < cm_size; ++cmc)
          img_dest->pixels[r * img_dest->width + c] += static_cast<uint16_t>(
              img_src->pixels[constrain(0, r + cmr - cm_size / 2,
                                        img_src->height - 1) *
                                  img_dest->width +
                              constrain(0, c + cmc - cm_size / 2,
                                        img_src->width - 1)] *
              conv_mat[cm_size * cmr + cmc]);
    }
  pgm_image_normalize(img_dest);
  return 0;
}

int pgm_image_gaussian_smooth(pgm_image *img_dest, pgm_image *img_src,
                              double sigma = 2.0, int ker_size = 3) {
  double coeff_sum = 0.0;
  double *gauss_mat = new double[ker_size * ker_size];
  for (int i = 0; i < ker_size; ++i)
    for (int j = 0; j < ker_size; ++j) {
      gauss_mat[i * ker_size + j] =
          gaussian(i - ker_size / 2, j - ker_size / 2, sigma);
      coeff_sum += gauss_mat[i * ker_size + j];
    }
  for (int i = 0; i < ker_size * ker_size; ++i) gauss_mat[i] /= coeff_sum;
  int ret_val =
      pgm_image_convolution_filter(img_dest, img_src, gauss_mat, ker_size);
  delete[] gauss_mat;
  return ret_val;
}

const int ker_size = 3;
const double sigma = 7.0;

int main(int argc, char **argv) {
  pgm_image *gen_img = new pgm_image;
  pgm_image_create(gen_img, 1024, 1024, 0, 255);
  for (int i = 0; i < gen_img->height; ++i)
    for (int j = 0; j < gen_img->width; ++j) {
      pixel color = 0;
      if (((i / 4) % 2 == 0) && ((j / 4) % 2 == 0)) color = 255;
      pgm_image_set_pixel(gen_img, i, j, color);
    }
  pgm_image *smoothed_ser = new pgm_image;
  pgm_image *smoothed_par = new pgm_image;

  std::cout << "================Blurring start=============" << std::endl;
  omp_set_num_threads(1);
  double ser_time = omp_get_wtime();
  pgm_image_gaussian_smooth(smoothed_ser, gen_img, sigma, ker_size);
  ser_time = (omp_get_wtime() - ser_time);
  std::cout << "Serial time:" << ser_time << " sec" << std::endl;
  const int nt = omp_get_num_procs();
  omp_set_num_threads(nt);
  double par_time = omp_get_wtime();
  pgm_image_gaussian_smooth(smoothed_ser, gen_img, sigma, ker_size);
  par_time = (omp_get_wtime() - par_time);
  std::cout << "Parallel time(" << nt << " threads):" << par_time << " sec"
            << std::endl;
  std::cout << "Acceleration: " << (ser_time / par_time) << std::endl;
  std::cout << "================Blurring end===============" << std::endl;
  pgm_image_destroy(gen_img);
  pgm_image_destroy(smoothed_par);
  pgm_image_destroy(smoothed_ser);
  return 0;
}
