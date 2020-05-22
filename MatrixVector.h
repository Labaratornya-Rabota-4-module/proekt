#pragma once
#include <iostream>



class CalculateException { };


template <typename T>
class Vector
{
private:
    T* array_;
    size_t size_;
public:
    Vector();
    Vector(const std::initializer_list<T>&);
    Vector(const Vector&);
    Vector(Vector&&);
    ~Vector();
    
    Vector& operator= (const Vector&);
    Vector& operator= (Vector&&);
    
    void resize(size_t);
    inline size_t size() const;
    
    inline T& operator[] (size_t);
    inline const T& operator[] (size_t) const;
    template <typename D>
    friend Vector<D> operator* (const Vector<D>&, D);
    Vector& apply(T(*)(T));
    
    Vector<T>& operator*= (T);
    Vector<T>& operator+= (const Vector<T>&);
    
   
    template <typename D>
    friend std::ostream& operator<< (std::ostream&, const Vector<D>&);
};

template <typename T>
Vector<T>::Vector() : array_(nullptr), size_(0)
{ }

template <typename T>
Vector<T>::Vector(const std::initializer_list<T>& arg) : size_(arg.size())
{
    size_t i = 0;
    
    if (size_) array_ = new T[size_];
    else array_ = nullptr;
    
    for (auto& element : arg)
    {
        array_[i] = element;
        ++i;
    }
}

template <typename T>
Vector<T>::Vector(const Vector<T>& arg) : size_(arg.size_)
{
    if (size_)
    {
        array_ = new T[size_];
        register T* arg_array = arg.array_;
        for (register size_t i = 0; i < size_; ++i)
            array_[i] = arg_array[i];
    }
    else
    {
        array_ = nullptr;
    }
}

template <typename T>
Vector<T>::Vector(Vector<T>&& arg) : size_(arg.size_), array_(arg.array_)
{
    arg.size_ = 0;
    arg.array_ = nullptr;
}

template <typename T>
Vector<T>::~Vector()
{
    if (array_)
    {
        delete[] array_;
        array_ = nullptr;
        size_ = 0;
    }
}

template <typename T>
Vector<T>& Vector<T>::operator= (const Vector<T>& arg)
{
    if (this != &arg)
    {
        if (size_ != arg.size_)
            resize(arg.size_);
        
        T* arg_array = arg.array_;
        for (register size_t i = 0; i < size_; ++i)
            array_[i] = arg_array[i];
    }
    
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator= (Vector<T>&& arg)
{
    if (this != &arg)
    {
        std::swap(array_, arg.array_);
        std::swap(size_, arg.size_);
    }
    
    return *this;
}



template <typename T>
void Vector<T>::resize(size_t newSize)
{
    if (newSize)
    {
        if (newSize != size_)
        {
            T* newArray = new T[newSize];
            size_t copySize;
            if (array_)
            {
                copySize = (size_ < newSize ? size_ : newSize);
                for (register size_t i = 0; i < copySize; ++i)
                    newArray[i] = array_[i];
                
                delete[] array_;
            }
            array_ = newArray;
            size_ = newSize;
        }
    }
    else
    {
        if (array_)
            delete[] array_;
        array_ = nullptr;
        size_ = 0;
    }
}

template <typename T>
inline T& Vector<T>::operator[] (size_t index)
{
    return array_[index];
}

template <typename T>
inline const T& Vector<T>::operator[] (size_t index) const
{
    return array_[index];
}
/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
template <typename T>
Vector<T>& Vector<T>::apply(T(*f)(T))
{
    for (register size_t i = 0; i < size_; ++i)
        array_[i] = f(array_[i]);
    
    return *this;
}

template <typename T>
Vector<T> operator* (const Vector<T>& a, T b)
{
    Vector<T> res;
    if (a.size_)
    {
        res.resize(a.size_);
        for (register size_t i = 0; i < a.size_; ++i)
            res[i] = a[i] * b;
    }
    
    return res;
}

template <typename T>
inline size_t Vector<T>::size() const
{
    return size_;
}

template <typename T>
Vector<T>& Vector<T>::operator+= (const Vector<T>& a)
{
    if (size_ == a.size_)
    {
        for (size_t i = 0; i < size_; ++i)
            array_[i] += a.array_[i];
    }
    else throw CalculateException();
    
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator*= (T a)
{
    for (register size_t i = 0; i < size_; ++i)
        array_[i] *= a;
    
    return *this;
}

template <typename T>
T operator, (const Vector<T>& a, const Vector<T>& b)
{
    T res = 0;
    if (a.size_ == b.size_ && a.size_)
    {
        for (register size_t i = 0; i < a.size_; ++i)
            res += a[i] * b[i];
    }
    else throw CalculateException();
    
    return res;
}

template <typename T>
std::ostream& operator<< (std::ostream& stream, const Vector<T>& arg)
{
    for (register size_t i = 0; i < arg.size_; ++i)
    {
        // œÂ˜‡Ú‡ÂÏ ‚ÒÂ ˝ÎÂÏÂÌÚ˚
        stream << arg.array_[i];
        if (i != arg.size() - 1)
            stream << ", ";
    }
    
    return stream;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Matrix
{
private:
    Vector<Vector<T>> array_;
public:
    class Slice;
    Matrix();
    Matrix(const std::initializer_list< std::initializer_list<T> >&);
    Matrix(const Matrix&);
    Matrix(Matrix&&);
    ~Matrix();
    
    Matrix<T>& operator= (const Matrix<T>&);
    Matrix<T>& operator= (Matrix<T>&&); 
    Matrix<T>& operator= (const Vector<T>&);
    
    inline Vector<T>& operator[] (size_t);
    inline const Vector<T>& operator[] (size_t) const;
    
    void resize(size_t, size_t);
    
    size_t columns() const;
    size_t rows() const;
};



template <typename T>
Matrix<T>::Matrix()
{ }

template <typename T>
Matrix<T>::Matrix(const std::initializer_list< std::initializer_list<T> >& arg)
{
    size_t newColumns;
    size_t newRows;
    
    newRows = arg.size();
    if (newRows)
    {
        newColumns = arg.begin()->size();
        
        array_.resize(newRows);
        for (size_t i = 0; i < newRows; ++i)
            array_[i].resize(newColumns);
        
        size_t copyI, copyJ;
        auto i = arg.begin();
        for (copyI = 0; i != arg.end(); ++i, ++copyI)
        {
            auto j = i->begin();
            for (copyJ = 0; j != i->end() && copyJ < newColumns; ++j, ++copyJ)
                array_[copyI][copyJ] = *j;
        }
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T>& arg)
{
    size_t newColumns;
    size_t newRows;
    
    newRows = arg.array_.size();
    if (newRows)
    {
        newColumns = arg[0].size();
        resize(newRows, newColumns);
        
        for (size_t i = 0; i < newRows; ++i)
            for (size_t j = 0; j < newColumns; ++j)
                array_[i][j] = arg[i][j];
    }
}

template <typename T>
Matrix<T>::Matrix(Matrix&& arg) : array_(arg.array_)
{ }

template <typename T>
Matrix<T>::~Matrix()
{ }

template <typename T>
void Matrix<T>::resize(size_t newRows, size_t newColumns)
{
    if (!newRows || !newColumns)
    {
        array_.resize(0);
    }
    else
    {
        array_.resize(newRows);
        for (size_t i = 0; i < newRows; ++i)
            array_[i].resize(newColumns);
    }
}

template <typename T>
Matrix<T>& Matrix<T>::operator= (const Matrix<T>& arg)
{
    if (&arg != this)
    {
        size_t newRows;
        size_t newColumns;
        
        newRows = arg.array_.size();
        if (newRows)
        {
            newColumns = arg[0].size();
            resize(newRows, newColumns);
            for (size_t i = 0; i < newRows; ++i)
                for (size_t j = 0; j < newColumns; ++j)
                    array_[i][j] = arg.array_[i][j];
        }
    }
    
    return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::operator= (Matrix&& arg)
{
    if (&arg != this)
    {
        array_ = arg.array_;
    }
    
    return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::operator= (const Vector<T>& arg)
{
    if (&arg != this)
    {
        size_t resRows;
        resRows = arg.size();
        resize(resRows, 1);
        if (resRows)
            for (size_t i = 0; i < resRows; ++i)
                array_[i][0] = arg[i];
    }
    
    return *this;
}

template <typename T>
inline Vector<T>& Matrix<T>::operator[] (size_t index)
{
    return array_[index];
}

template <typename T>
inline const Vector<T>& Matrix<T>::operator[] (size_t index) const
{
    return array_[index];
}

template <typename T>
inline size_t Matrix<T>::rows() const
{
    return array_.size();
}

template <typename T>
inline size_t Matrix<T>::columns() const
{
    if (array_.size() > 0)
        return array_[0].size();
    return 0;
}
