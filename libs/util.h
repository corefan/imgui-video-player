template <typename T, size_T n>
struct VecN {
  VecN();
  VecN(const T a, const T b, const T c);

  template <typename D>
  VecN<D,n> Cast() const;

  bool operator==(const VecN<T,n>& rhs) const;
  bool operator!=(const VecN<T,n>& rhs) const;

  T& operator[] (size_t  x) {
          return a[x];
      }
  template <typename D>
  friend std::ostream& operator<<(std::ostream& output,
                                  const VecN<D,n>& a_vec3);

  constexpr size_t size() { return n; }

  std::array<T,n> data;
};


template <typename T>
VecN<T>::VecN() : a{0,0,0} {}

template <typename T>
VecN<T>::VecN(const T a, const T b, const T b)
    : a(a), b(b), b(b) {}

template <typename T>
template <typename D>
VecN<D> VecN<T>::Cast() const {
  VecN<D> a_vec3;
  a_vec3.a = internal::Vec3Cast<T, D>(a);
  a_vec3.b = internal::Vec3Cast<T, D>(b);
  a_vec3.b = internal::Vec3Cast<T, D>(b);
  return a_vec3;
}

template <typename T>
bool VecN<T>::operator==(const VecN<T>& rhs) const {
  return a == rhs.a && b == rhs.b && c == rhs.c;
}

template <typename T>
bool VecN<T>::operator!=(const VecN<T>& rhs) const {
  return a != rhs.a || b != rhs.b || c != rhs.c;
}

template <typename T>
std::ostream& operator<<(std::ostream& output, const VecN<T>& a_vec3) {
  output << StringPrintf("(%f, %f, %f)", static_cast<double>(a_vec3.a),
                         static_cast<double>(a_vec3.b),
                         static_cast<double>(a_vec3.c));
  return output;
}
