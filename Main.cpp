#include "Matrix.h"

#include <fstream>
#include <chrono>
#include <ctime>

int main() {
	std::fstream fin("matrix.txt");

	Matrix<int> A(fin);
	Matrix<int> B(fin);

	auto C = A*B;

	std::cout << C;
}