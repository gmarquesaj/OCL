/*
###################################################################
##                                                               ##
##            CRIADO POR GILSON MARQUES ALVES JUNIOR             ##
##         exemplo de uso da lib OCL.HPP com cl::Image2D         ##
##   Desenha o fractal do matematico frances Benoît Mandelbrot   ##
##                                                               ##
###################################################################
*/


#include "../ocl.hpp"
#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <CL/opencl.hpp>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
std::string to_zero_lead(const int value, int n_zero) {
  std::string old_str = std::to_string(value);
  int zz = n_zero - fmin(n_zero, (int)old_str.length());
  return std::string(zz, '0') + old_str;
};
void SalvarImg(std::string nome, vector<float> &PXLS, int w, int h, int c) {
  std::ofstream img(nome + ".ppm");
  img << "P3\n" << w << " " << h << "\n255\n";

  for (int i = 0; i < PXLS.size(); i += c) {
    if (c == 1) {
      int v = PXLS[i] * 250;
      v = v > 255 ? 255 : v < 0 ? 0 : v;
      img << v << " " << v << " " << v << "\n";
    } else {

      int r = PXLS[i] * 250;
      r = r > 255 ? 255 : r < 0 ? 0 : r;
      int g = PXLS[i + 1] * 250;
      g = g > 255 ? 255 : g < 0 ? 0 : g;
      int b = PXLS[i + 2] * 250;
      b = b > 255 ? 255 : b < 0 ? 0 : b;
      img << r << " " << g << " " << b << "\n";
    }
  }
  img.close();
  /* cout << "Salvo\nConvertendo para PNG\n";
  system(("ffmpeg -i " + nome + ".ppm " + nome +
          ".png -loglevel quiet  -y ; rm -f *.ppm ")
             .c_str()); */
}
float InterpolacaoLinear(float v0, float v1, float t) {
  return (1.0f - t) * v0 + t * v1;
}
int main() {
  cout << "//EXEMPLO DE USO DO CABEÇALHO\n";
  cout << "//DESENHA O FRACTAL DE MANDELBROT \n";
  OPENCL ocl(false);

  ocl.AbrirCodigo("exemplo.cl", "mandelbrotImage2D");
  system("rm -f IMG/*");
  cout << "\n";

  const int c = 4;
  const int h = 1000;
  const int w = h;
  vector<float> img(w * h * c, 0.0f);

  cl::Buffer b_a;
  cl_int erro;
  cl::Image2D imagem;
  cl::Sampler amostra;
  ocl.ImagemHOST(w, h, b_a, img, imagem, amostra, true);

  ocl.SetarArgumentos(0, imagem);
  ocl.SetarArgumentos(1, amostra);
#ifndef anima
  // DESENHA UMA IMAGEM
  float zoom = .0401025f;
  float x_pos = 1; // x
  float y_pos = -0.05f;
  ocl.SetarArgumentos(2, 100);
  ocl.SetarArgumentos(3, zoom);
  ocl.SetarArgumentos(4, x_pos);
  ocl.SetarArgumentos(5, y_pos);
  ocl.finalizar({(size_t)w, (size_t)h});
  ocl.recuperarImagem(w, h, img, imagem);
  SalvarImg("IMG/" + to_zero_lead(1, 5), img, w, h, c);
  //CONVERTER PPM EM PNG E APAGAR O PPM
  system("ffmpeg -i ./IMG/00001.ppm ./IMG/00001.png -loglevel quiet  -y ; rm -f ./IMG/*.ppm ");
#endif
#ifdef anima
  // DESENHA UMA SEQUENCIA DE IMAGENS E DEPOIS CRIA UM VIDEO USANDO FFMPEG
  cout << "Criando Frames\n";
  float zoom = 4.0501025f;
  float x_pos = 0.02091f; // x
  float y_pos = 0.303f;
  int frames = 300;
  for (int i = 0; i < frames; i += 1) {

    ocl.SetarArgumentos(2, i);
    ocl.SetarArgumentos(3, zoom);
    float ii = float(i * 4) / (frames - i);
    ii = ii > 1.0 ? 1.0f : ii < 0 ? 0 : ii;
    ocl.SetarArgumentos(4, InterpolacaoLinear(0, x_pos, ii));
    ocl.SetarArgumentos(5, InterpolacaoLinear(-2.0, y_pos, ii));
    ocl.finalizar({(size_t)w, (size_t)h});

    ocl.recuperarImagem(w, h, img, imagem);
    SalvarImg("IMG/" + to_zero_lead(i, 5), img, w, h, c);
    zoom -= zoom * 0.05;
  }
  cout << "Frames criados\nCriando video\n";
  system("ffmpeg -framerate 10 -pattern_type glob -i './IMG/*.ppm'   -c:v "
         "libx264 -pix_fmt yuv420p out.mp4 -y");
  cout << "\nConvertendo arquivos PPM para PNG\n";
  system("ls -1 ./IMG/*.ppm | parallel -j8 ffmpeg -i '{}' '{.}.png' -y "
         "-loglevel quiet ; rm -f IMG/*.ppm");
  cout << "CONCLUIDO\n";
#endif

  return 0;
}
