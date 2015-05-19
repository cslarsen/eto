#include <iostream>
#include <list>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace eto {

class garbage_collector {
public:
  typedef std::tuple<std::type_index, bool, void*> typed_ptr;
  std::vector<typed_ptr> allocated;

  template<typename X, typename... Args>
  X* alloc(Args... args)
  {
    X* ptr = new X(args...);
    allocated.push_back({typeid(X), false, ptr});
    std::cout << "allocated " << ptr << std::endl;
    return ptr;
  }

  virtual ~garbage_collector()
  {
    free();
  }

  void free()
  {
    for ( auto i : allocated ) {
      std::cout << "free " << std::get<0>(i).name() << " " << std::get<1>(i) <<
        " " << std::get<2>(i) << std::endl;
      //delete(reinterpret_cast<i.first>(i.second));
    }

    /*
    while ( !allocated.empty() ) {
      typed_ptr v = allocated.pop_back();
      delete(static_cast<v.first>(v.second)),
    }
    */
  }
};

garbage_collector gc;

class cons {
public:
  const class var* car;
  const class var* cdr;

  cons(const var&& car_, const var&& cdr_);
  cons(const var&& car_);

  cons(const var* car_ = nullptr, const var* cdr_ = nullptr) :
    car(car_), cdr(cdr_)
  {
  }

  friend std::ostream& operator<<(std::ostream& o, const cons& v);
};

class var {
public:
  enum type {
    INTEGER,
    STRING,
    CONS
  };

  enum type type;
  union {
    int integer;
    std::string string;
    class cons cons;
  };

  var(const var& v) :
    type(v.type)
  {
    switch ( type ) {
      case INTEGER:
        integer = v.integer;
        break;
      case STRING:
        string = v.string;
        break;
      case CONS:
        cons = v.cons;
        break;
    }
  }

  var(const char* s) :
    type(STRING)
  {
    string = std::string(s);
  }

  virtual ~var()
  {
  }

  var(const int& n) :
    type(INTEGER), integer(n)
  {
  }

  var(const std::string& s) :
    type(STRING), string(s)
  {
  }

  var(const class cons& c) :
    type(CONS), cons(c)
  {
  }

  friend std::ostream& operator<<(std::ostream& o, const var& v)
  {
    switch ( v.type ) {
      case INTEGER:
        return o << v.integer;
      case STRING:
        return o << v.string;
      case CONS:
        return o << v.cons;
    }
  }
};

cons::cons(const var&& car_, const var&& cdr_):
  car(gc.alloc<var>(car_)), cdr(gc.alloc<var>(cdr_))
{
}

cons::cons(const var&& car_):
  car(gc.alloc<var>(car_)), cdr(nullptr)
{
}

std::ostream& operator<<(std::ostream& o, const cons& v)
{
  const bool dot = v.cdr && v.cdr->type != var::CONS;

  if ( v.car)
    o << *v.car;

  if ( dot )
    o << " . ";
  else if ( v.cdr )
    o << " ";

  if ( v.cdr )
    o << *v.cdr;

  return o;
}

} // namespace eto

int main(int, char**)
{
  using namespace eto;

  std::cout << "(cons 1 2) ==> " << cons(1, 2) << std::endl;
  std::cout << "(cons 1 (cons 2 '())) ==> " << cons(1, cons(2)) << std::endl;
  std::cout << "(cons (cons 1 (cons 2 '()) 0) ==> " << cons(cons(1, cons(2)), 0) << std::endl;
  std::cout << "(cons 1 (cons 2 3)) ==> " << cons(1, cons(2, 3)) << std::endl;
  std::cout << "(cons 1 (cons 2 (cons 3 '()))) ==> " << cons(1, cons(2, cons(3))) << std::endl;
  std::cout << "(cons (cons 1 (cons 2 '())) (cons 3 (cons 4 '()))) ==> "
    << var(cons(cons(1, cons(2)),
                cons(3, cons(4)))) << std::endl;
}
