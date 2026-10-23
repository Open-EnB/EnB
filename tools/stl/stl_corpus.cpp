// Explicit instantiations of VC6's standard containers for common element types, to measure how
// much of E&B's app code is compiler-generated template code. Element types of the same size and
// kind compile to the same code, so a few stand for many.
#pragma warning(disable: 4786)
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <deque>
#include <algorithm>

struct Pod4  { int a; };
struct Pod8  { int a, b; };
struct Pod12 { int a, b, c; };
struct Pod16 { int a, b, c, d; };
inline bool operator == (const Pod8 &x, const Pod8 &y) { return x.a == y.a && x.b == y.b; }
inline bool operator <  (const Pod8 &x, const Pod8 &y) { return x.a < y.a; }
inline bool operator != (const Pod8 &x, const Pod8 &y) { return !(x == y); }
inline bool operator != (const Pod12 &x, const Pod12 &y) { return !(x.a == y.a); }
inline bool operator != (const Pod16 &x, const Pod16 &y) { return !(x.a == y.a); }
inline bool operator == (const Pod12 &x, const Pod12 &y) { return x.a == y.a; }
inline bool operator <  (const Pod12 &x, const Pod12 &y) { return x.a < y.a; }
inline bool operator == (const Pod16 &x, const Pod16 &y) { return x.a == y.a; }
inline bool operator <  (const Pod16 &x, const Pod16 &y) { return x.a < y.a; }
class Obj;

template class std::vector<int>;
template class std::vector<unsigned int>;
template class std::vector<float>;
template class std::vector<char>;
template class std::vector<unsigned char>;
template class std::vector<short>;
template class std::vector<Obj *>;
template class std::vector<std::string>;
template class std::list<int>;
template class std::list<Obj *>;
template class std::list<std::string>;
template class std::deque<int>;
template class std::deque<Obj *>;
template class std::map<int, int>;
template class std::map<int, Obj *>;
template class std::map<unsigned int, Obj *>;
template class std::map<int, std::string>;
template class std::map<std::string, int>;
template class std::map<std::string, Obj *>;
template class std::map<std::string, std::string>;
template class std::map<Obj *, int>;
template class std::set<int>;
template class std::set<Obj *>;
template class std::set<std::string>;
template class std::multimap<int, Obj *>;
template class std::basic_string<char>;
