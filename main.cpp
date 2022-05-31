#include "ocl.hpp"

int main() {
  OPENCL ocl(true);
  cout << "teste\n";
  vector<int> A = {10, 12, 2, 3, 42, 5, 6, 7, 8, 9};
  vector<int> B = {0, 1, 2, 10, 1, 2, 0, 13, 2, 0};
  vector<int> C(10, 0);
  ocl.SetarCodigo(
      "   void kernel adicao(global const int* A, global const int* B, "
      "global int* C){       "
      "       C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];     "
      "            "
      "   }                                                                    "
      "           ");

  cl::Buffer b_a = ocl.CriarBuffer(A);
  cl::Buffer b_b = ocl.CriarBuffer(B);
  cl::Buffer b_c = ocl.CriarBuffer(C);
  ocl.SetarPrograma("adicao");
  ocl.SetarArgumentos(0, b_a);
  ocl.SetarArgumentos(1, b_b);
  ocl.SetarArgumentos(2, b_c);
  ocl.finalizar(10);
  ocl.recuperar(b_c, C);
  for (int i = 0; i < 10; i++) {
    cout << C[i] << " ";
  }
  cout << "\n";
  return 0;
}
