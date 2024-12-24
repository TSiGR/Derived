#include <iostream>
#include <type_traits>
#include <cassert>
#include <memory>

class Animal
{
    public:

    Animal(const std::string& name, int weight)
    {
        m_name = name;
        m_weight = weight;
    }
    virtual std::ostream& print(std::ostream& out) const = 0;

    virtual const char* sound() const = 0;

    friend std::ostream& operator<<(std::ostream& out, const Animal& a)
    {
        a.print(out);
        return out;
    }

    protected:

    int m_weight;
    std::string m_name;
};

class Dog : public Animal
{
    public:

    Dog(const std::string& name, int weight) : Animal(name, weight)
    {

    }

    virtual std::ostream& print(std::ostream& out) const
    {
        out << "The name: " << m_name << ", the weight: " << m_weight << std::endl;
        return out;
    }

    virtual const char* sound() const override
    {
        return "Baw";
    }
};

class Cat : public Animal
{
    public:

    Cat(const std::string& name, int weight) : Animal(name, weight)
    {

    }

    virtual std::ostream& print(std::ostream& out) const
    {
        out << "The name: " << m_name << ", the weight: " << m_weight << std::endl;
        return out;
    }

    virtual const char* sound() const override
    {
        return "Meow";
    }
};

template<typename T, int size>
class StaticArray_Base
{
    protected:
    T m_array[size];

    public:

    T* getArray()
    {
        return m_array;
    }
    T& operator[](size_t i)
    {
        return m_array[i];
    }

    void print() {};
};

template<typename T, int size>
class StaticArray: public StaticArray_Base<T, size>
{
    public:
    StaticArray()
    {

    }
    template<typename Other>
    StaticArray(StaticArray<Other, size>& another)
    {
        for (int i = 0; i < size; ++i)
            this->m_array[i] = *(another.getArray() + i);
    }
    template<typename Other>
    StaticArray& operator=(StaticArray<Other, size>& another)
    {
        if (this != &another)
        {
            std::swap(this, another);
        }
        return *this;
    }

    void print()
    {
        for (int i = 0; i < size; ++i)
            std::cout << this->m_array[i] << " ";
        std::cout << '\n';
    }
};

template<int size>
class StaticArray<double, size>: public StaticArray_Base<double, size>
{
    public:

    void print()
    {
        for (int i = 0; i < size; ++i)
            std::cout << std::scientific << this->m_array[i] << "  ";
        std::cout << '\n';
    }
};

template <typename U, typename T>
void copy_n(T* mas1, U* mas2, size_t N)
{
    if constexpr (!std::is_class_v<U>)
    {
        std::cout << "Trivial types\n";
        for (int i = 0; i < N; ++i)
        {
            mas1[i] = static_cast<T>(mas2[i]);
        }
    }
    else
    {
        *mas1 = static_cast<T>(*mas2);
    }
}

//CRTP

template<typename Sub>
class Container
{
    private:

    Sub& self()
    {
        return static_cast<Sub&>(*this);
    }

    Sub const& self() const
    {
        return static_cast<Sub const&>(*this);
    }

    public:

    decltype(auto) front()
    {
        return *self().begin();
    }

    decltype(auto) back()
    {
        return *std::prev(self().end());
    }

    decltype(auto) size() const
    {
        return std::distance(self().begin(), self().end());
    }

    decltype(auto) operator[](size_t i)
    {
        return *std::next(self().begin(), i);
    }
};

template<typename T>
class DynArray : public Container<DynArray<T>>
{
    public:

    DynArray(size_t size) : m_size(size), m_data(std::make_unique<T[]>(m_size))
    {}

    T* begin() { return m_data.get(); }

    const T* begin() const { return m_data.get(); }

    T* end() { return m_data.get() + m_size; }

    const T* end() const { return m_data.get() + m_size; }

    private:

    size_t m_size;
    std::unique_ptr<T[]> m_data;
};

int main()
{
    Cat cat("Bars", 42);
    Dog dog("Bob", 31);
    std::cout << cat;
    std::cout << dog;
    cat.sound();
    dog.sound();

    StaticArray<int, 4> intArray;

    for (int i = 0; i < 4; ++i)
        intArray[i] = i;
    intArray.print();

    StaticArray<double, 4> doubleArray;
 
	for (int count = 0; count < 4; ++count)
		doubleArray[count] = (4.0 + 0.1 * count);
	doubleArray.print();

    copy_n(&intArray, &doubleArray, sizeof(doubleArray) / sizeof(doubleArray[0]));

    double arr_double[] = {0.5, 1.5, 2.5, 3.5, 4.5};
    int arr_int[sizeof(arr_double) / sizeof(arr_double[0])];

    copy_n(arr_int, arr_double, sizeof(arr_double) / sizeof(arr_double[0]));

    DynArray<int> arr(10);
    arr.front() = 2;
    arr[2] = 9;
    assert(arr.size() == 10);
}
// char * const (*(* const bar)[5])(int )