#include <string.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

#include <boost/variant.hpp>

namespace eto {

class cons {
public:
  class var* car;
  class var* cdr;

  cons(var* car_ = nullptr, var* cdr_ = nullptr) :
    car(car_), cdr(cdr_)
  {
  }
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

  var(class cons& c) :
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
      case CONS: {
        o << "(";
        if ( v.cons.car )
          o << v.cons.car;
        else
          o << "'()";
        o << (v.cons.cdr? " " : " . ");
        if ( v.cons.cdr )
          o << v.cons.cdr;
        else
          o << "'()";
        o << ")";
        return o;
      }
    }
  }
};

} // namespace eto

int main(int, char**)
{
  using namespace eto;

  var n = "hello";
  std::cout << n << std::endl;
}
