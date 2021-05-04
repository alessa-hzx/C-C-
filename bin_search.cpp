
#include <math.h>
#include <cstdio>
#include <cstdlib>


template<typename R, typename A>
class Function {
public:
  virtual R invoke(A arg) = 0;
  virtual ~Function() {}
};
/////////////////////////////////////////////////////////////////////

int binarySearchForZero(Function<int, int> * f, int low, int high);

// always negative
class negFunction: public Function<int, int>{
public:
    virtual int invoke(int arg){
        return -1;
    }
};

// always positive
class posFunction: public Function<int, int>{
public:
    virtual int invoke(int arg){
        return 1;
    }
};

// linear function
class linFunction: public Function<int, int>{
public:
    virtual int invoke(int arg){
        return (arg);
    }
};


// sin function 
class SinFunction : public Function<int, int> {
public:
  virtual int invoke(int arg) {
    return 10000000 * (sin(arg/100000.0) - 0.5);
  }
};

class CountedInFn: public Function<int, int>{
protected:
    unsigned remaining;
    Function<int, int> * f;
    const char * mesg;
public:
    CountedInFn(unsigned n, Function<int, int> * fn, const char * m):
    remaining(n), f(fn), mesg(m){}
    virtual int invoke(int arg){
        if (remaining == 0){
            fprintf(stderr, "Too many functino invocations in %s\n", mesg);
            exit(EXIT_FAILURE);
        }
        remaining--;
        return f->invoke(arg);
    }
};

void testfn(const char * m, Function<int, int> * f, int low, int high, int ans){
    unsigned maxinvok;
    if (high > low){
        maxinvok = log(high - low) / log(2) + 1;
    } else{
        maxinvok = 1;
    }
    CountedInFn * fn = new CountedInFn(maxinvok, f, m); // create a pointer to function

    int result = binarySearchForZero(fn, low, high);

    if (result != ans){
        fprintf(stderr, "Wrong answer in %s.\n", m);
        exit(EXIT_FAILURE);
    }

}

int main(void){
    SinFunction sinf;
    negFunction negf;
    posFunction posf;
    linFunction linf;

    testfn("sin function", & sinf, 0, 150000, 52359);
    testfn("negative function", & negf, 1, 5, 4);
    testfn("negative function", &negf, -10000, -10, -11);
    testfn("positive function", & posf, 2, 12, 2);
    testfn("linear function", & linf, -99, 100, 0);
    testfn("postive linear", &linf, 3, 10, 3);
    testfn("high = low", &linf, 0, 0, 0);


}