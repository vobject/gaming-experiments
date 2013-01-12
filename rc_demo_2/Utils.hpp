#ifndef UTILS_HPP
#define UTILS_HPP

#include <memory>
#include <utility>

class Vector
{
public:
   constexpr Vector(const double x, const double y) : mX(x), mY(y) { }
   ~Vector() { }

   Vector& operator=(const Vector& v) { mX = v.mX; mY = v.mY; return *this;}
   Vector operator+(const Vector v) const { return { mX + v.mX, mY + v.mY }; }
   Vector operator-(const Vector v) const { return { mX - v.mX, mY - v.mY }; }
   Vector operator*(const double d) const { return { mX * d, mY * d }; }

   double GetX() const { return mX; }
   double GetY() const { return mY; }

private:
   double mX;
   double mY;
};

constexpr int operator"" _ms(unsigned long long milliseconds)
{
   return milliseconds;
}

// Source:
//  http://herbsutter.com/gotw/_102/
//  http://stackoverflow.com/questions/10149840/c-arrays-and-make-unique
template<typename T, typename ...Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&& ...args)
{
   return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(std::size_t n)
{
   typedef typename std::remove_extent<T>::type RT;
   return std::unique_ptr<T>(new RT[n]);
}

// Source:
// http://channel9.msdn.com/Events/GoingNative/GoingNative-2012/C-11-VC-11-and-Beyond
template<class T>
auto cbegin(const T& t) -> decltype(t.cbegin()) { return t.cbegin(); }

template<class T>
auto cend(const T& t) -> decltype(t.cend()) { return t.cend(); }

#endif // UTILS_HPP
