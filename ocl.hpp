#define CL_HPP_TARGET_OPENCL_VERSION 120
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
};