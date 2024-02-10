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
std::string to_zero_lead(const int value, int n_zero)
{
  std::string old_str = std::to_string(value);
  int zz = n_zero - fmin(n_zero, (int)old_str.length());
  return std::string(zz, '0') + old_str;
};
void SalvarImgP6(std::string nome, vector<float> &PXLS, int w, int h, int c)
{
  std::ofstream img(nome + ".ppm");
  img << "P6\n"
      << w << " " << h << "\n255\n";
  vector<char> saida;
  int s = w * h * 3;
  saida.reserve(s);

  for (int i = 0; i < PXLS.size(); i += c)
  {
    saida.push_back((PXLS[i] * 200));
    saida.push_back((PXLS[i + 1] * 200));
    saida.push_back((PXLS[i + 2] * 200));
  }
  img.write(saida.data(), w * h * c);
  img.close();
}

float InterpolacaoLinear(float v0, float v1, float t)
{
  return (1.0f - t) * v0 + t * v1;
}
int main()
{
  cout << "//EXEMPLO DE USO DO CABEÇALHO\n";
  cout << "//DESENHA O FRACTAL DE MANDELBROT \n";
  OPENCL ocl(false);

  ocl.AbrirCodigo("exemplo.cl", "mandelbrotVectorFloat");
  //system("rm -f IMG/*");
  cout << "\n";

  const int c = 4;
  const int h = 5000;
  const int w = h;
  vector<float> img(w * h * c, 1.0f);

  cl::Buffer b_a = ocl.CriarBufferESCRITA(img);
  ocl.VetorParaBuffer(b_a, img);

  ocl.SetarArgumentos(0, b_a);

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
  SalvarImgP6("IMG/mandelbrot", img, w, h, c);

  cout << "CONCLUIDO\n";

  return 0;
}
