#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#include <iostream>
#include <vector>
using std::cout;
using std::vector;
#include "/usr/include/CL/opencl.hpp"
#include <fstream>
class OPENCL {
public:
  std::vector<cl::Platform> Plataformas;
  cl::Platform PlataformaPadrao;
  std::vector<cl::Device> TodosDispositivos;
  cl::Device Dispositivo;
  cl::Context contexto;
  cl::Program Programa;
  cl::Program::Sources Codigo;
  cl::Kernel kernel;
  cl::CommandQueue Fila;
  OPENCL(bool printar = false) {
    cl::Platform::get(&Plataformas);

    if (Plataformas.size() == 0) {
      std::cout << "Nenhuma plataforma encontrada!\n";
      exit(1);
    }
    if (printar) {
      cout << "PLATAFORMAS DISPONIVEIS :\n";
      for (int i = 0; i < Plataformas.size(); i++) {
        cl::Platform *PA = &Plataformas[i];
        std::vector<cl::Device> TDA;
        PA->getDevices(CL_DEVICE_TYPE_ALL, &TDA);
        cout << "\nNOME         : " << PA->getInfo<CL_PLATFORM_NAME>()
             << "\nFORNECIDA    : " << PA->getInfo<CL_PLATFORM_VENDOR>()
             << "\nVERSÃO       : " << PA->getInfo<CL_PLATFORM_VERSION>()
             << "\nEXTENSÕES    : " << PA->getInfo<CL_PLATFORM_EXTENSIONS>()
             << "\n";
        if (TDA.size() != 0) {
          cout << "DISPOSITIVOS :\n";
          for (int d = 0; d < TDA.size(); d++) {
            cl::Device *DA = &TDA[d];
            cout << "\t\t" << DA->getInfo<CL_DEVICE_NAME>() << "\n";
          }
        } else {
          cout << "\tNENHUM DISPOSITIVOS NESTA PLATAFORMA\n";
        };
      };
    }
    PlataformaPadrao = Plataformas[0];

    PlataformaPadrao.getDevices(CL_DEVICE_TYPE_ALL, &TodosDispositivos);
    if (TodosDispositivos.size() == 0) {
      std::cout << " Nenhum dispositivo encontrado!\n";
      exit(1);
    }

    Dispositivo = TodosDispositivos[0];
    cout << "\n\nDISPOSITO SELECIONADO : "
         << Dispositivo.getInfo<CL_DEVICE_NAME>() << "\n";
    contexto = cl::Context({Dispositivo});
  }
  void AbrirCodigo(std::string codigo,std::string nomeProg) {
    std::ifstream t(codigo);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    SetarCodigo(str);
    SetarPrograma(nomeProg);

  }
  void SetarCodigo(std::string codigo) {

    Codigo.push_back({codigo.c_str(), codigo.length()});
    Programa = cl::Program(contexto, Codigo);
    cl_int erro = Programa.build({Dispositivo});
    if (erro != CL_SUCCESS) {
      std::cout << " Error building: \n";
      std::ofstream arquivoerro("ArquivoErro.txt");
      arquivoerro << erro;
      arquivoerro << Programa.getBuildInfo<CL_PROGRAM_BUILD_LOG>(Dispositivo);
      arquivoerro.close();

      exit(1);
    }
    Fila = cl::CommandQueue(contexto, Dispositivo);
  }
  void SetarPrograma(std::string NomeFuncao) {
    kernel = cl::Kernel(Programa, NomeFuncao.c_str());
  }
  template <typename T> void SetarArgumentos(cl_uint index, const T &value) {
    kernel.setArg(index, value);
  }
  void finalizar(cl::NDRange range = cl::NullRange) {
    Fila.enqueueNDRangeKernel(kernel, cl::NullRange, range, cl::NullRange);
    Fila.finish(); //*/
  }
  template <typename T>
  void recuperar(cl::Buffer &Buffer, std::vector<T> &Vetor) {
    Fila.enqueueReadBuffer(Buffer, CL_TRUE, 0, sizeof(T) * Vetor.size(),
                           Vetor.data());
  }
  template <typename T>
  void GuardaBuffer(cl::Buffer &Buffer, std::vector<T> &Vetor) {
    Fila.enqueueWriteBuffer(Buffer, CL_TRUE, 0, sizeof(T) * Vetor.size(),
                            Vetor.data());
  }
  template <typename T> void GuardaBuffer(std::vector<T> &Vetor) {
    cl::Buffer Buffer(contexto, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                      sizeof(T) * Vetor.size(), Vetor.data());
    Fila.enqueueWriteBuffer(Buffer, CL_TRUE, 0, sizeof(T) * Vetor.size(),
                            Vetor.data());
  }
  template <typename T> cl::Buffer CriarBuffer(std::vector<T> &Vetor) {
    return cl::Buffer(contexto, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                      sizeof(T) * Vetor.size(), Vetor.data());
  }

  template <typename T> cl::Buffer CriarBufferESCRITA(std::vector<T> &Vetor) {
    return cl::Buffer(contexto, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                      sizeof(T) * Vetor.size(), Vetor.data());
  }
  void SetarImgFormat(cl::ImageFormat &formato) {
    formato.image_channel_order = CL_RGBA;
    formato.image_channel_data_type = CL_FLOAT;
  }
  int Imagem(int w, int h, cl::Buffer &B_img, vector<float> &img,
             cl::Image2D &imagem, cl::Sampler &amostra, bool exibir = false) {
    B_img = CriarBuffer(img);
    cl_int erro;
    cl::ImageFormat formato;
    SetarImgFormat(formato);
    imagem =
        cl::Image2D(contexto, formato, B_img, (size_t)w, (size_t)h, 0, &erro);
    if (erro != 0) {
      cout << "Erro na criaçao da imagem " << erro << "\n";
      return erro;
    } else if (exibir) {
      cout << "sucesso na criação da imagem\n";
    }
    Fila.enqueueWriteImage(imagem, CL_TRUE, {0, 0, 0},
                           {(size_t)w, (size_t)h, 1}, 0, 0, img.data(), 0,
                           NULL);
    if (erro != 0) {
      cout << "Erro em  colocar imagem na fila " << erro << "\n";
      return erro;
    } else if (exibir) {
      cout << "sucesso em colocar imagem  na fila \n";
    }

    amostra = cl::Sampler(contexto, CL_TRUE, CL_ADDRESS_CLAMP_TO_EDGE,
                          CL_FILTER_NEAREST, &erro);
    if (erro != 0) {
      cout << "Erro em  criar amostra " << erro << "\n";
      return erro;
    } else if (exibir) {
      cout << "sucesso em  criar amostra \n";
    }
    return erro;
  }
  int recuperarImagem(int w, int h, vector<float> &img, cl::Image2D &imagem,
                      bool exibir = false) {
    int erro = Fila.enqueueReadImage(imagem, CL_TRUE, {0, 0, 0},
                                     {(size_t)w, (size_t)h, 1}, 0, 0,
                                     img.data(), NULL, NULL);
    if (erro != 0) {
      cout << "Erro em recuperar os dados " << erro << "\n";
      return erro;
    } else if (exibir) {
      cout << "sucesso em recuperar os dados \n";
    }
    return erro;
  }
};