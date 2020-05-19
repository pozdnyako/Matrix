#pragma once

#include <iostream>
#include <thread> 
#include <vector>
#include <future>

template<class T>
class Matrix {
public:
	Matrix();
	Matrix(int, int, bool isRand = false);
	Matrix(std::istream&);
	Matrix(const Matrix<T>&);

	~Matrix();

	T* const operator[](int) const;
	T* operator[](int);

	friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& mat) {
		os << "Matrix " << mat.nRow << "x" << mat.nCol << "\n";
		for(int y = 0; y < mat.nRow; y++) {
			for(int x = 0; x < mat.nCol; x++) {
				os << mat[y][x] << "\t";
			}
			os << "\n";
		}
		return os;
	}

	const Matrix<T> operator*(const Matrix<T>&) const;
	
private:
	int nCol, nRow;
	T* data;

	bool is_mem_allocated;

	void allocate_mem();
	void free_mem();
};

template<class T>
Matrix<T>::Matrix() :
	nCol(0),
	nRow(0),
	data(NULL),
	is_mem_allocated(false){
}

template<class T>
Matrix<T>::Matrix(int row, int col, bool isRand) :
	nCol(col),
	nRow(row),
	data(NULL),
	is_mem_allocated(false) {

	allocate_mem();

	for(int i = 0; i < nCol * nRow; i++) {
		data[i] = T();
	}
}

Matrix<int>::Matrix(int row, int col, bool isRand) :
	nCol(col),
	nRow(row),
	data(NULL),
	is_mem_allocated(false) {

	allocate_mem();

	if(isRand) {
		for(int i = 0; i < nCol * nRow; i++) {
			data[i] = rand();
		}
	}
	else {
		for(int i = 0; i < nCol * nRow; i++) {
			data[i] = 0;
		}
	}
}

template<class T>
Matrix<T>::Matrix(std::istream &is) :
	nCol(0),
	nRow(0),
	data(NULL),
	is_mem_allocated(false) {

	is >> nRow >> nCol;
	allocate_mem();

	for(int y = 0; y < nRow; y++) {
		for(int x = 0; x < nCol; x++) {
			is >> data[x + y*nCol];
		}
	}
}

template<class T>
void move(T *a, T *b) {
	*b = *a;
}

template<class T>
Matrix<T>::Matrix(const Matrix<T>& a) :
	Matrix<T>(a.nRow, a.nCol) {

	for(int i = 0; i < nCol*nRow; i++) {
		data[i] = a.data[i];
	}
}



template<class T>
Matrix<T>::~Matrix() {
	free_mem();
}

template<class T>
void Matrix<T>::allocate_mem() {
	if(is_mem_allocated) {
		std::cout << "Mem already allocated\n";
		throw(std::bad_alloc());
	}
	if(nCol <= 0 || nRow <= 0) {
		std::cout << "Incorrect size of matrix\n";
		throw(std::bad_alloc());
	}

	data = new T[nCol * nRow];
	is_mem_allocated = true;
}

template<class T>
void Matrix<T>::free_mem() {
	if(!is_mem_allocated) {
		throw(std::logic_error("mem isn't allocated"));
	}

	delete data;
	is_mem_allocated = false;
}	

template<class T>
T* const Matrix<T>::operator[](int y) const {
	return data + nCol * y;
}

template<class T>
T* Matrix<T>::operator[](int y) {
	return data + nCol * y;
}


template<class T>
const Matrix<T> Matrix<T>::operator* (const Matrix<T>& B) const {
	if(nCol != B.nRow)
		throw(std::runtime_error("1.nCol != 2.nRow"));
	
	Matrix<T> C(nRow, B.nCol);

	std::vector<std::future<void>> futures(nRow * B.nCol);

	auto compute = [this, &B, &C](int y, int x) {
		for(int k = 0; k < B.nRow; k++) {
			C[y][x] += (*this)[y][k] * B[k][x];
		}
	};

	for(int y = 0; y < nRow; y++) {
		for(int x = 0; x < B.nCol; x++) {
			futures[y * B.nCol + x] = std::async(std::launch::async, compute, y, x);
		}
	}

	
	for(int i = 0; i < futures.size(); i++) {
		futures[i].get();
	}

	return C;
}

/*

template<class T>
const Matrix<T> Matrix<T>::operator* (const Matrix<T>& B) const {
	if(nCol != B.nRow)
		throw(std::runtime_error("1.nCol != 2.nRow"));
	
	Matrix<T> C(nRow, B.nCol);

	auto compute = [this, &B, &C](int y, int x) {
		for(int k = 0; k < B.nRow; k++) {
			C[y][x] += (*this)[y][k] * B[k][x];
		}
	};

	for(int y = 0; y < nRow; y++) {
		for(int x = 0; x < B.nCol; x++) {
			compute(y, x);
		}
	}

	return C;
}
*/