/*
###################################################################
##                                                               ##
##            CRIADO POR GILSON MARQUES ALVES JUNIOR             ##
##       exemplo de uso da lib OCL.HPP com Vetor de Float        ##
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
void SalvarImgP6(std::string nome, vector<float> &PXLS, int w, int h, int c) {
  std::ofstream img(nome + ".ppm");
  img << "P6\n" << w << " " << h << "\n255\n";
  vector<char> saida;
  int s= w*h *3;
  saida.reserve(s);

  for (int i = 0; i < PXLS.size(); i+=c) {
    saida.push_back((PXLS[i]*200));
    saida.push_back((PXLS[i+1]*200));
    saida.push_back((PXLS[i+2]*200));
  }
  img.write(saida.data(), w*h*c);
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

  ocl.AbrirCodigo("exemplo.cl", "mandelbrotVectorFloat");
  system("rm -f IMG/*");
  cout << "\n";

  const int c = 4;
  const int h = 5000;
  const int w = h;
  vector<float> img(w * h * c, 1.0f);

  cl::Buffer b_a = ocl.CriarBufferESCRITA(img);
  ocl.VetorParaBuffer(b_a, img);


  ocl.SetarArgumentos(0, b_a);
//#define anima
#ifndef anima
  // DESENHA UMA IMAGEM
  float zoom = .0201025f;
  float x_pos = 1; // x
  float y_pos = -0.05f;
  ocl.SetarArgumentos(1, 100);
  ocl.SetarArgumentos(2, zoom);
  ocl.SetarArgumentos(3, x_pos);
  ocl.SetarArgumentos(4, y_pos);
  cout << "PROCESSANDO\n";
  ocl.finalizar({(size_t)w, (size_t)h});
  cout << "FINALIZADO\nRECUPERANDO\n";
  ocl.recuperar(b_a, img);
  cout << "RECUPERADO\nSALVANDO PRA ARQUIVO\n";
  SalvarImgP6("IMG/" + to_zero_lead(1, 5), img, w, h, c);
  /*   cout<<"SALVO\nCONVERTENDO PARA PNG\n";
    //CONVERTER PPM EM PNG E APAGAR O PPM
    system("ffmpeg -i ./IMG/00001.ppm ./IMG/00001.png -loglevel quiet  -y ; rm
    -f ./IMG/*.ppm "); */
  cout << "CONCLUIDO\n";
#endif
#ifdef anima
  // DESENHA UMA SEQUENCIA DE IMAGENS E DEPOIS CRIA UM VIDEO USANDO FFMPEG
  cout << "Criando Frames\n";
  float zoom = 4.0501025f;
  float x_pos = 0.02090f; // x quanto maior mais pra direita -> voce vẽ
  float y_pos = 0.30299f; // y quanto maior mais pra baixo voce vẽ
  int frames = 300;
  for (int i = 0; i < frames; i += 1) {

    ocl.SetarArgumentos(1, int(i * 1.5));
    ocl.SetarArgumentos(2, zoom);
    float ii = float(i * 4) / (frames - i);
    ii = ii > 1.0 ? 1.0f : ii < 0 ? 0 : ii;
    ocl.SetarArgumentos(3, InterpolacaoLinear(0, x_pos, ii));
    ocl.SetarArgumentos(4, InterpolacaoLinear(-2.0, y_pos, ii));
    ocl.finalizar({(size_t)w, (size_t)h});

    ocl.recuperar(b_a, img);
    SalvarImgP6("IMG/" + to_zero_lead(i, 5), img, w, h, c);
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
