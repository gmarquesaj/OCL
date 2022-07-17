/*
##################################################
##                                              ##
##    CRIADO POR GILSON MARQUES ALVES JUNIOR    ##
##       EM 20/JUNHO/2022- SEGUNDA FEIRA        ##
##              Pirapora MG Brasil              ##
##    COM O OBJETIVO DE FACILITAR A CRIAÇÃO     ##
##   DE PEQUENAS APLICAÇÕES COM PROCESSAMENTO   ##
##     EM GPU USANDO OPENCL DE FORMA FACIL      ##
##                                              ##
##                   CONTATOS                   ##
##                55038984331917                ##
##             gmarquesaj@gmail.com             ##
##################################################
*/


#include <CL/cl.h>
#include <cstddef>
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
  std::string CodigoSTR;
  cl::Kernel kernel;
  cl::CommandQueue Fila;
  // INICIALIZAR A CLASSE
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
  // LER ARQUIVO DE TEXTO E SETAR COMO CODIGO ALEM DE DEFINIR O NOME DO KERNEL
  void AbrirCodigo(std::string codigo, std::string nomeProg) {
    std::ifstream t(codigo);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
                    CodigoSTR = str;
    SetarCodigo(str);
    SetarPrograma(nomeProg);
  }
  // PASSAR STRING COMO CODIGO PARA A CLASSE
  void SetarCodigo(std::string codigo) {
    Codigo.clear();
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
  // DEFINIR O NOME DO KERNEL
  void SetarPrograma(std::string NomeFuncao) {
    kernel = cl::Kernel(Programa, NomeFuncao.c_str());
  }
  // DEFINIR O VALOR DE UM PARAMETRO DO KERNEL NO INDEX ESPECIFICADO
  template <typename T> void SetarArgumentos(cl_uint index, const T &value) {
    kernel.setArg(index, value);
  }
  // EXECUTA O KERNEL NO RANGE ESPECIFICADO E AGUARDA A EXECUÇÃO FINALIZAR
  void finalizar(cl::NDRange range = cl::NullRange) {
    Fila.enqueueNDRangeKernel(kernel, cl::NullRange, range, cl::NullRange);
    Fila.finish(); //*/
  }
  // PASSAR CONTEUDO DO BUFFER PARA VETOR
  template <typename T>
  void recuperar(cl::Buffer &Buffer, std::vector<T> &Vetor) {
    Fila.enqueueReadBuffer(Buffer, CL_TRUE, 0, sizeof(T) * Vetor.size(),
                           Vetor.data());
  }
  // COPIAR VETOR PARA O BUFFER
  template <typename T>
  void VetorParaBuffer(cl::Buffer &Buffer, std::vector<T> &Vetor) {
    
    Fila.enqueueWriteBuffer(Buffer, CL_TRUE, 0, sizeof(T) * Vetor.size(),
                            Vetor.data());
  }


  //ALOCAR MEMORIA NO DISPOSITVO host
  template <typename T> cl::Buffer CriarBufferHOST(std::vector<T> &Vetor) {
    return cl::Buffer(contexto, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                      sizeof(T) * Vetor.size(), Vetor.data());
  }
  //ALOCAR MEMORIA SOMENTE ESCRITA NO DISPOSITIVO HOST
  template <typename T> cl::Buffer CriarBufferHOST_ESCRITA(std::vector<T> &Vetor) {
    return cl::Buffer(contexto, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                      sizeof(T) * Vetor.size(), Vetor.data());
  }


//ALOCAR MEMORIA NO DISPOSITVO
  template <typename T> cl::Buffer CriarBuffer(std::vector<T> &Vetor) {
    return cl::Buffer(contexto,CL_MEM_READ_WRITE, sizeof(T) * Vetor.size(),NULL,NULL);
  }
  //ALOCAR MEMORIA SOMENTE ESCRITA NO DISPOSITIVO
  template <typename T> cl::Buffer CriarBufferESCRITA(std::vector<T> &Vetor) {
    return cl::Buffer(contexto,CL_MEM_WRITE_ONLY, sizeof(T) * Vetor.size(),NULL,NULL);
   
  }  

  //DEFINI O FORMATO DA IMAGEM PARA VERMELHOR VERDE AZUL ALPHA EM FLOAT
  void SetarImgFormat(cl::ImageFormat &formato) {
    formato.image_channel_order = CL_RGBA;
    formato.image_channel_data_type = CL_FLOAT;
  }
  //CRIA IMAGEM 2D BASEADA NO VETOR RETORNA 0 PARA SUCESSO OU NUMERO PARA ERRO
  int ImagemHOST(int w, int h, cl::Buffer &B_img, vector<float> &img,
             cl::Image2D &imagem, cl::Sampler &amostra, bool exibir = false) {
    B_img = CriarBufferHOST(img);
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

  //COPIAR DADOS DA IMAGEM 2D PARA O VETOR
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