#include <cassert>
#include <iostream>
#include <list>
#include <list>
#include <sstream>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace eto {

class var;

class garbage_collector {
public:
  enum state {
    REACHABLE,
    UNREACHABLE,
  };

  typedef void (*delete_function)(void*);
  std::unordered_map<void*, std::pair<state, delete_function>> allocated;

  template<typename X>
  static void delete_ptr(void* ptr)
  {
    X* x = reinterpret_cast<X*>(ptr);
    std::cout << "deleting " << ptr << " `" << x->to_string() << "'" <<
      std::endl;
    delete(reinterpret_cast<X*>(ptr));
  }

  template<typename X, typename... Args>
  X* alloc(Args... args)
  {
    auto ptr = new X(args...);
    allocated[ptr] = {UNREACHABLE, delete_ptr<X>};
    std::cout << "alloc " << ptr << " " << ptr->to_string() << std::endl;
    return ptr;
  }

  virtual ~garbage_collector()
  {
    std::cout << "exiting ..." << std::endl;
    free_all();
  }

  size_t size() const
  {
    return allocated.size();
  }

  void mark_all(const state& n)
  {
    for ( auto& kv : allocated ) {
      auto tup = std::get<1>(kv);
      tup.first = n;
    }
  }

  size_t collect(const var* p)
  {
    mark_all(UNREACHABLE);
    mark(p, REACHABLE);
    auto r = sweep();
    return r;
  }

  size_t sweep()
  {
    size_t r = 0;
    for ( auto kv = allocated.begin(); kv != allocated.end(); ) {
      auto ptr = kv->first;
      if ( kv->second.first == UNREACHABLE ) {
        std::cout << "sweep: unreachable " << ptr << std::endl;
        free(ptr);
        kv = allocated.erase(kv);
        ++r;
      } else
        ++kv;
    }
    return r;
  }

  void mark(const var* p, const state& s);

  void free(void* ptr)
  {
    auto p = allocated[ptr];
    auto deleter = std::get<1>(p);
    deleter(ptr);
  }

  void free_all()
  {
    for ( auto kv : allocated )
      free(std::get<0>(kv));
  }
};

garbage_collector gc;

class cons {
public:
  const class var* car;
  const class var* cdr;

  cons(const var& car_, const var& cdr_);
  cons(const var& car_);

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

  std::string type_name() const
  {
    switch ( type ) {
      case INTEGER: return "integer";
      case STRING: return "string";
      case CONS: return "cons";
    }
  }

  enum type type;
  union {
    int integer;
    std::string string; // whoa, how can this even compile? needs to be pointer
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

  std::string to_string() const
  {
    std::stringstream s;
    switch ( type ) {
      case INTEGER:
        s << "#<integer: " << std::to_string(integer) << ">";
        break;
      case STRING:
        s << "#<string: " << string << ">";
        break;
      case CONS:
        s << "#<cons: car=" << cons.car << " cdr=" << cons.cdr << ">";
        break;
    }

    return s.str();
  }

  var& operator=(const var& v)
  {
    if ( this != &v ) {
      type = v.type;
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
    return *this;
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

cons::cons(const var& car_, const var& cdr_):
  car(gc.alloc<var>(car_)), cdr(gc.alloc<var>(cdr_))
{
}

cons::cons(const var& car_):
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

const var* cdr(const var* p)
{
  assert(p != nullptr);
  assert(p->type == var::CONS);
  return p->cons.cdr;
}

const var* car(const var* p)
{
  assert(p != nullptr);
  assert(p->type == var::CONS);
  return p->cons.car;
}

void garbage_collector::mark(const var* p, const state& flag)
{
  std::cout << "mark: looking at " << p << std::endl;

  if ( !p )
    return;

  auto h = allocated.find(const_cast<void*>(static_cast<const void*>(p)));
  if ( h != allocated.end() ) {
    std::cout << "mark: in-use " << p << std::endl;
    h->second.first = flag;
  }

  if ( p->type == var::CONS ) {
    mark(car(p), flag);
    mark(cdr(p), flag);
  }
}

} // namespace eto

int main(int, char**)
{
  using namespace eto;

  std::cout << "objects: " << gc.size() << std::endl;
  const var *root = gc.alloc<var>(cons(0, cons(1, cons(2))));
  std::cout << "** root: " << *root << std::endl;

  /*
  std::cout << "(cons 1 2) ==> " << cons(1, 2) << std::endl;
  std::cout << "(cons 1 (cons 2 '())) ==> " << cons(1, cons(2)) << std::endl;
  std::cout << "(cons (cons 1 (cons 2 '()) 0) ==> " << cons(cons(1, cons(2)), 0) << std::endl;
  std::cout << "(cons 1 (cons 2 3)) ==> " << cons(1, cons(2, 3)) << std::endl;
  std::cout << "(cons 1 (cons 2 (cons 3 '()))) ==> " << cons(1, cons(2, cons(3))) << std::endl;
  std::cout << "objects before: " << gc.size() << std::endl;
  std::cout << "(cons (cons 1 (cons 2 '())) (cons 3 (cons 4 '()))) ==> "
    << var(cons(cons(1, cons(2)),
                cons(3, cons(4)))) << std::endl;
  */
  std::cout << "objects: " << gc.size() << std::endl;
  root = cdr(root);
  auto col = gc.collect(root);
  std::cout << "collected " << col << " objects" << std::endl;
  std::cout << "** root: " << *root << std::endl;
//  root = cons(root, cons(3, cons(4)));
//  std::cout << "(cons root (cons 3 (cons 4))) ==> " << root << std::endl;

  std::cout << "objects: " << gc.size() << std::endl;
}
