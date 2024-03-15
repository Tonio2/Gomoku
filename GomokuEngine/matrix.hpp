#include <vector>
#include <iostream>

#pragma once

template<typename Element>
class Matrix {

public:

    struct Index {
        int8_t row;
        int8_t col;

        Index() : row(0), col(0) {}

        Index(int r, int c) : row(r), col(c) {}

        bool is_valid(const Matrix<Element>& matrix) const {
            return matrix.is_in_bound(row, col);
        }
    };

    /** Init */

    Matrix() :
    _width(0), _height(0), _elements(0)
    {}

    Matrix(int width, int height) :
    _width(width), _height(height), _elements(width * height)
    {}

    Matrix(const Matrix<Element>& copy) :
    _width(copy._width), _height(copy._height), _elements(copy._elements)
    {}

    Matrix(Matrix<Element>&& move) noexcept :
    _width(move._width), _height(move._height), _elements(std::move(move._elements))
    {}

    ~Matrix()
    {}

    Matrix& operator=(const Matrix<Element>& copy) {
        if (this != &copy)
        {
            _width = copy._width;
            _height = copy._height;
            _elements = copy._elements;
        }
        return *this;
    }

    Matrix& operator=(Matrix<Element>&& move) noexcept {
        if (this != &move)
        {
            _width = move._width;
            _height = move._height;
            _elements = std::move(move._elements);
        }
        return *this;
    }

    /** Element getter */

    Element& operator()(int row, int col) {
        return _elements[index_at(row, col)];
    }

    const Element& operator()(int row, int col) const {
        return _elements[index_at(row, col)];
    }

    Element& operator[](Index index) {
        return _elements[index_at(index.row, index.col)];
    }

    const Element& operator[](Index index) const {
        return _elements[index_at(index.row, index.col)];
    }

    bool is_in_bound(int row, int col) const {
        return row >= 0 && row < _height
            && col >= 0 && col < _width;
    }

    int get_width() const {
        return _width;
    }

    int get_height() const {
        return _height;
    }


    /** Print */

    void print() const {
        std::cout << "[" << _width << ";" << _height << "]" << std::endl;
        for (int row = 0; row < _height; ++row)
        {
            for (int col = 0; col < _width; ++col)
            {
                std::cout << (*this)(row, col) << ' ';
            }
            std::cout << std::endl;
        }
    }

private:

    int index_at(int row, int col) const {
        return row * _width + col;
    }

    int _width;
    int _height;
    std::vector<Element> _elements;

};
