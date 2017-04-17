#include "mtxvec.h"

#include <vector>
#include <cmath>
#include <cassert>
#include <tuple>

#include <iostream>
using namespace std;

static void sortRawData(std::vector<std::tuple<double, size_t, size_t>>& raw_data);

/*
 * Vector member funciton
 */
Vector::Vector(){}

Vector::Vector(size_t size) {
    data.resize(size);
}

void Vector::resize(size_t size){
    data.resize(size);
}

double Vector::norm2() const {
    double res = .0f;

    for (double num: data) {
        res += num * num;
    }
    return sqrt(res);
}

double Vector::dotV(const Vector& other) const {
    double sum = .0f;

    assert(data.size() == other.size());

    for (size_t i = 0; i < data.size(); i++) {
        sum += data[i] * other.get(i);
    }

    return sum;
}

Vector Vector::add(const Vector& other) const {
    assert(data.size() == other.size());

    Vector res(data.size());

    for (size_t i = 0; i < data.size(); i++) {
        res.set(i, data[i] + other.get(i));
    }

    return res;
}

// same as add, but inplace
Vector Vector::iadd(const Vector& other) {
    assert(data.size() == other.size());

    for (size_t i = 0; i < data.size(); i++) {
        data[i] += other.get(i);
    }

    return *this;
}

Vector Vector::sub(const Vector& other) const {
    assert(data.size() == other.size());

    Vector res(data.size());

    for (size_t i = 0; i < data.size(); i++) {
        res.set(i, data[i] - other.get(i));
    }

    return res;
}

// same as sub, but inplace
Vector Vector::isub(const Vector& other) {
    assert(data.size() == other.size());

    for (size_t i = 0; i < data.size(); i++) {
        data[i] -= other.get(i);
    }

    return *this;
}

Vector Vector::mulS(double scaler) const {
    Vector res(data.size());

    for (size_t i = 0; i < data.size(); i++) {
        res.set(i, data[i] * scaler);
    }

    return res;
}

// same as mulS, but inplace
Vector Vector::imulS(double scaler) {
    for (size_t i = 0; i < data.size(); i++) {
        data[i] *= scaler;
    }

    return *this;
}

/*
 * Matrix member funciton
 */
Matrix::Matrix():n_rows(0),n_cols(0){}

Matrix::Matrix(size_t m, size_t n) {
    data.resize(m);
    n_rows = m;
    n_cols = n;
    for (int i = 0; i < m; i++){
        data[i] = std::vector<double>(n);
    }
}

void Matrix::resize(size_t m, size_t n){
    data.resize(m);
    n_rows = m;
    n_cols = n;
    for(int i = 0; i < m; i++) {
        data[i] = std::vector<double>(n);
    }
}

void Matrix::setRow(size_t row_idx, const Vector& vec) {
    size_t len = vec.size();

    assert(0 <= row_idx);
    assert(row_idx < n_rows);
    assert(len == n_cols);

    for (size_t i = 0; i < len; ++i) {
        data[row_idx][i] = vec.get(i);
    }
}

void Matrix::setCol(size_t col_idx, const Vector& vec) {
    size_t len = vec.size();

    assert(0 <= col_idx);
    assert(col_idx < n_cols);
    assert(len == n_rows);

    for (size_t i = 0; i < len; ++i) {
        data[i][col_idx] = vec.get(i);
    }
}

Vector Matrix::getCol(size_t col_idx) const {
    Vector col(n_rows);

    for (int i = 0; i < n_rows; ++i) {
        col.set(i, data[i][col_idx]);
    }

    return col;
}

Vector Matrix::mul(const Vector& vec) const {
    Vector ret(n_rows);

    for (size_t i = 0; i < n_rows; ++i) {
        double temp = .0f;
        for (size_t j = 0; j < n_cols; ++j) {
            temp += data[i][j] * vec.get(j);
        }
        ret.set(i, temp);
    }

    return ret;
}

Vector Matrix::mulPartial(const Vector& vec, size_t n_cols_) const {
    assert(n_cols >= vec.size());
    Vector ret(n_rows);

    for (size_t i = 0; i < n_rows; ++i) {
        double temp = .0f;
        for (size_t j = 0; j < n_cols_; ++j) {
            temp += data[i][j] * vec.get(j);
        }
        ret.set(i, temp);
    }

    return ret;
}

SparseMatrix::SparseMatrix(
    std::vector<std::tuple<double, size_t, size_t>> raw_data,
    size_t n_rows, size_t n_cols) {

    sortRawData(raw_data);

    indptr.push_back(0);

    for (size_t i = 0; i < raw_data.size(); ++i) {
        double val = std::get<0>(raw_data[i]);
        size_t row_idx = std::get<1>(raw_data[i]);
        size_t col_idx = std::get<2>(raw_data[i]);

        data.push_back(val);
        indices.push_back(row_idx);

        while (indptr.size() <= col_idx) {
            indptr.push_back(i);
        }

    }

    while (indptr.size() <= n_cols) {
        indptr.push_back(data.size());
    }

    // for (size_t i = 0; i < indptr.size(); ++i) {
    //     cout << indptr[i] << endl;
    // }
}

// --- Helpers ---

void sortRawData(std::vector<std::tuple<double, size_t, size_t>>& raw_data) {
    std::sort(
        raw_data.begin(), raw_data.end(),
        [](const std::tuple<double, size_t, size_t> &left,
           const std::tuple<double, size_t, size_t> &right) {
            if (std::get<2>(left) < std::get<2>(right)) {
                return true;
            } else if (std::get<2>(left) > std::get<2>(right)) {
                return false;
            } else {
                return std::get<1>(left) < std::get<1>(right);
            }
        });
}
