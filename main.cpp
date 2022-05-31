

#include "ocl.hpp"

int main() {
  cout << "//EXEMPLO DE USO DO CABEÇALHO\n";
  cout << "//SOMA OS VALORES DOS VETORES 'A' e 'B' SALVANDO O RESULTADO EM "
          "'C'\n\n";
  OPENCL ocl(true);
  vector<int> A = {10, 12, 2, 35, 42, 51, 16, 17, -8, 9};
  vector<int> B = {32, 21, 5, 10, 15, 23, 10, 13, 2, -10};
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
    cout << A[i] << " + " << B[i] << " = " << C[i] << "\n";
  }
  cout << "\n";
  return 0;
}
