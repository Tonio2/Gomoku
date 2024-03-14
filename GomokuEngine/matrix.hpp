#include <vector>
#include <iostream>

#pragma once

template<typename Element>
class Matrix {

public:

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

    bool is_in_bound(int row, int col) const {
        return row >= 0 && row < _height
            && col >= 0 && col < _width;
    }

    const Element& safe_get_at(int row, int col) const {
        if (is_in_bound(row, col))
            return _elements[index_at(row, col)];
        else
            return out_value;
    }

    void safe_set_at(int row, int col, const Element& value) {
        if (is_in_bound(row, col))
            _elements[index_at(row, col)] = value;
    }

    int get_width() const {
        return _width;
    }

    int get_height() const {
        return _height;
    }


    /** Print */

    void print() const {
        std::cout << "[" << _width << ";" << _height << "] (out=" << out_value << ")" << std::endl;
        for (int row = 0; row < _height; ++row)
        {
            for (int col = 0; col < _width; ++col)
            {
                std::cout << (*this)(row, col) << ' ';
            }
            std::cout << std::endl;
        }
    }

    Element out_value;

private:

    int index_at(int row, int col) const {
        return row * _width + col;
    }

    int _width;
    int _height;
    std::vector<Element> _elements;

};
