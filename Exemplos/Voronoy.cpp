/*
####################################################
##                                                ##
##     CRIADO POR GILSON MARQUES ALVES JUNIOR     ##
##          EM 27/Novembro/2022- Domingo          ##
##               Pirapora MG Brasil               ##
##                                                ##
##           exemplo de uso da lib OCL            ##
##   Desenha o diagrama do matematico ucraniano   ##
##           Georgy Feodosevich Voronoy           ##
##                                                ##
##                    CONTATOS                    ##
##                 55038984177956                 ##
##              gmarquesaj@gmail.com              ##
####################################################
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
    /* cout << "Salvo\nConvertendo para PNG\n";
    system(("ffmpeg -i " + nome + ".ppm " + nome +
            ".png -loglevel quiet  -y ; rm -f *.ppm ")
               .c_str()); */
}
typedef struct
{
    int x, y;
    float r, g, b;
} ponto;
float randf()
{
    return (rand() % 255) / 255.0;
}
int main()
{
    srand(time(0));
    OPENCL ocl(false);

    ocl.AbrirCodigo("exemplo.cl", "voronoy");
    //system("rm -f IMG/*");

    const int c = 4;
    const int h = 2000;
    const int w = h;
    const int q = 500;
    vector<float> img(w * h * c, 1.0f);
    vector<ponto> pontos;
    for (int i = 0; i < q; i++)
    {
        pontos.push_back({rand() % w, rand() % h, randf(), randf(), randf()});
    }
    cl::Buffer b_a = ocl.CriarBufferESCRITA(img);
    ocl.VetorParaBuffer(b_a, img);
    ocl.SetarArgumentos(0, b_a);

    cl::Buffer b_b = ocl.CriarBuffer(pontos);
    ocl.VetorParaBuffer(b_b, pontos);
    ocl.SetarArgumentos(1, b_b);

    ocl.SetarArgumentos(2, q);

    cout << "PROCESSANDO\n";
    ocl.finalizar({(size_t)w, (size_t)h});
    cout << "FINALIZADO\nRECUPERANDO\n";
    ocl.recuperar(b_a, img);
    cout << "RECUPERADO\nSALVANDO PRA ARQUIVO\n";
    SalvarImgP6("IMG/voronoy" , img, w, h, c);
    cout << "CONCLUIDO\n";
}