#include "Matrix.h"

#include <iomanip>
#include <sstream>
#include <cmath>
#include <string>

double** alloc2D(int r, int c)
{
    if (r <= 0 || c <= 0)
    {
        throw "Invalid allocation dimensions";
    }

    double** result = new double*[r];
    for (int i = 0; i < r; ++i)
    {
        result[i] = new double[c];
        for (int j = 0; j < c; ++j)
        {
            result[i][j] = 0.0;
        }
    }
    return result;
}

void dealloc2D(double** arr, int rows)
{
    if (arr != 0 && rows > 0)
    {
        for (int i = 0; i < rows; ++i)
        {
            delete [] arr[i];
            arr[i] = 0;
        }
        delete [] arr;
        arr = 0;
    }
}

Matrix::Matrix() : rows(1), cols(1), values(alloc2D(1, 1))
{
}

Matrix::Matrix(double d) : rows(1), cols(1), values(alloc2D(1, 1))
{
    values[0][0] = d;
}

Matrix::Matrix(int r, int c) : rows(r), cols(c), values(alloc2D(r, c))
{
}

Matrix::Matrix(int r, int c, double** v) : rows(r), cols(c), values(alloc2D(r, c))
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            values[i][j] = v[i][j];
        }
    }
}

Matrix::Matrix(const Matrix& other)
    : rows(other.rows), cols(other.cols), values(alloc2D(other.rows, other.cols))
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            values[i][j] = other.values[i][j];
        }
    }
}

Matrix& Matrix::operator=(const Matrix& right)
{
    if (this == &right)
    {
        return *this;
    }
    dealloc2D(values, rows);
    rows = right.rows;
    cols = right.cols;
    values = alloc2D(right.rows, right.cols);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            values[i][j] = right.values[i][j];
        }
    }
    return *this;
}

Matrix::~Matrix()
{
    dealloc2D(values, rows);
    rows = 0;
    cols = 0;
}

int Matrix::getRows() const
{
    return rows;
}

int Matrix::getCols() const
{
    return cols;
}

double Matrix::get(int r, int c) const
{
    if (r >= rows || c >= cols || r < 0 || c < 0)
    {
        throw OutOfBounds();
    }

    return values[r][c];
}

double& Matrix::operator()(int r, int c) const
{
    if (r >= rows || c >= cols || r < 0 || c < 0)
    {
        throw OutOfBounds();
    }

    return values[r][c];
}

Matrix Matrix::operator+(const Matrix& right) const
{
    if (rows != right.rows || cols != right.cols)
    {
        throw InvalidDimensions();
    }

    Matrix result(*this);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            result.values[i][j] += right.values[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& right) const
{
    return (*this) + (right * (-1));
}

Matrix Matrix::operator*(const Matrix& right) const
{
    if (cols != right.rows)
    {
        throw InvalidDimensions();
    }

    Matrix result(rows, right.cols);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < right.cols; ++j)
        {
            for (int k = 0; k < cols; ++k)
            {
                result.values[i][j] += values[i][k] * right.values[k][j];
            }
        }
    }  
    return result;
}

Matrix Matrix::operator*(double d) const
{
    Matrix result(*this);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            result.values[i][j] *= d;
        }
    }
    return result;
}

bool Matrix::operator==(const Matrix& right) const
{
	if(this == &right){
		return true;
	}
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if(this->values[i][j] != right.values[i][j]){
				return false;
			}
		}
	}
	return true;
}

bool Matrix::operator!=(const Matrix& right) const
{
	return !((*this) == right);
}

Matrix& Matrix::operator+=(const Matrix& right)
{
	return (*this) = (*this) + right;
}

Matrix& Matrix::operator-=(const Matrix& right)
{
	return (*this) = (*this) - right;
}

Matrix& Matrix::operator*=(const Matrix& right)
{
	return (*this) = (*this) * right;
}

Matrix& Matrix::operator*=(double scalar)
{
	return (*this) = (*this) * scalar;
}

Matrix Matrix::transpose() const
{
    Matrix result(cols, rows);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            result.values[j][i] = values[i][j];
        }
    }
    return result;
}

double Matrix::determinant() const
{
    if (!square())
    {
        throw InvalidDimensions();
    }

    if (rows == 1 && cols == 1)
    {
        return values[0][0];
    }

    double sum = 0;
    for (int i = 0; i < cols; ++i)
    {
        sum += values[0][i] * cofactor(0, i);
    }
    return sum;
}

Matrix Matrix::adjoint() const
{
    return cofactor().transpose();
}

bool Matrix::singular() const
{
	return (*this).square() && (*this).determinant() == 0;
}

bool Matrix::square() const
{
	if(rows < 0 || cols < 0){
		throw OutOfBounds();
	}
	return rows == cols;
}

Matrix Matrix::inverse() const
{
	if(singular()){
		throw InvalidDimensions();
	}
	Matrix M((*this).adjoint());
	return M * (1 / (*this).determinant());
}

double Matrix::minorM(int r, int c) const
{
    if (!square())
    {
        throw InvalidDimensions();
    }

    Matrix m(rows - 1, cols - 1);

    int mi = 0;
    int mj = 0;
    for (int i = 0; i < rows; i++)
    {
        if (i != r)
        {
            for (int j = 0; j < cols; j++)
            {
                if (j != c)
                {
                    m.values[mi][mj] = values[i][j];
                    ++mj;
                }
            }
            ++mi;
            mj = 0;
        }
    }
    return m.determinant();
}

double Matrix::cofactor(int r, int c) const
{
	if(!square()){
		throw InvalidDimensions();
	}
	return pow(-1, r + c) * (*this).minorM(r, c);	
}

Matrix Matrix::cofactor() const
{
	Matrix result(*this);
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			result.values[i][j] = cofactor(i , j);
		}
	}
	return result;
}

Matrix Matrix::identity(int size)
{
	Matrix result(size, size);
	int i = 0;
	while(i < size){
		result.values[i][i] = 1;
		i++;
	}
	return result;
}

std::string Matrix::str() const
{
	std::stringstream s;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
        {
            s << values[i][j] << std::setw(5) << " ";
        }
		s << "\n";
	}
	return s.str();
}

int main()
{
    // testing allocations
    /*for (;;)
    {
        Matrix m(10000, 10000);
    }
    return 0;
	*/
	
    // test functions
    Matrix id = Matrix::identity(5);
    std::cout << id << std::endl;

    Matrix m(3, 3);
    m(0, 0) = 1;
    m(0, 1) = 2;
    m(0, 2) = 3;
    m(1, 0) = 0;
    m(1, 1) = 4;
    m(1, 2) = 5;
    m(2, 0) = 1;
    m(2, 1) = 0;
    m(2, 2) = 6;

    std::cout << m << std::endl;
    std::cout << m.cofactor() << std::endl;
    
    return 0;
}