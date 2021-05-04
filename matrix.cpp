#include <cstdlib>
#include <ostream>
#include <assert.h>
#include <ostream>

class IntArray {
private:
  int * data;
  int numElements;
public:
  IntArray();
  IntArray(int n);
  IntArray(const IntArray & rhs);

  IntArray & operator=(const IntArray & rhs);
  int & operator[](int index);
  const int & operator[](int index) const;
  int size() const;
  bool operator==(const IntArray& rhs) const;
  bool operator!=(const IntArray& rhs) const;
  ~IntArray();
};

std::ostream & operator<<(std::ostream & s, const IntArray & rhs);

///////////////////////////////////////////////////////////////////////////

IntArray::IntArray(): data(NULL), numElements(0){}

IntArray::IntArray(int n): data(new int[n]), numElements(n){}

IntArray::IntArray(const IntArray & rhs): data(new int[rhs.numElements]),
    numElements(rhs.numElements){
        for (int i = 0; i < rhs.numElements; i++){
            data[i] = rhs.data[i];
        }
    }
IntArray::~IntArray(){
    delete[] data;
}


IntArray & IntArray::operator=(const IntArray & rhs){
    if (this != &rhs){
        int * temp = new int[rhs.numElements];
        for (int i = 0; i < rhs.numElements; i++){
            temp[i] = rhs.data[i];
        }
        delete[] data;
        data = temp;
        numElements = rhs.numElements;
    }
    return *this;
}

int & IntArray::operator[](int index){
    assert( (index >= 0) && (index < numElements) );
    return data[index];
}

const int & IntArray::operator[](int index) const {
    assert((index >= 0) && (index < numElements));
    return data[index];
}

int IntArray::size() const{
    return(numElements);
}

bool IntArray::operator==(const IntArray & rhs) const{
    if (numElements != rhs.numElements){
        return false;
    }
    for (int i = 0; i < numElements; i++){
        if (data[i] != rhs.data[i]){
            return false;;
        }
    }
    return true;
}

bool IntArray::operator!=(const IntArray & rhs) const{
    if (*this == rhs){
        return false;
    }
    return true;
}

std::ostream & operator<<(std::ostream & s, const IntArray & rhs){
    if (rhs.size() == 0){
        s << "{}";
        return s;
    }
    s << "{";
    for (int i = 0; i < rhs.size()-1; i++){
        s << rhs[i] << ", "; // use rhs[] because we have overload the [] operator!
    }
    s << rhs[rhs.size() - 1] << "}";
    return s;

}



/////////////////////////////////////////////////////// 075 ////////////////////////////

class IntMatrix {
 private:
  int numRows;
  int numColumns;
  IntArray ** rows;
 public:
  IntMatrix();
  IntMatrix(int r, int c);
  IntMatrix(const IntMatrix & rhs);
  ~IntMatrix(); // executed when write : delete matrix
  IntMatrix & operator=(const IntMatrix & rhs);
  int getRows() const;
  int getColumns() const;
  const IntArray & operator[](int index) const;
  IntArray & operator[](int index);
  bool operator==(const IntMatrix & rhs) const;
  IntMatrix operator+(const IntMatrix & rhs) const;


};
std::ostream & operator<<(std::ostream & s, const IntMatrix & rhs);

///////////////////////////////

IntMatrix::IntMatrix(): numRows(0), numColumns(0), rows(NULL){}

IntMatrix::IntMatrix(int r, int c): numRows(r), numColumns(c), rows(new IntArray *[r]){
    for (int i = 0; i < r; i++){
        rows[i] = new IntArray(c);
    }
}

IntMatrix::IntMatrix(const IntMatrix & rhs): numRows(rhs.numRows), numColumns(rhs.numColumns),
rows(new IntArray * [rhs.numRows]){
    for (int i = 0; i < rhs.numRows; i++){
        rows[i] = new IntArray(rhs[i]);  
    }
}

IntMatrix::~IntMatrix(){
    for (int i = 0; i < numRows; i++){
        delete rows[i]; // use delete when new one thing with certain type
    }
    delete[] rows; // use delete[] when new an array of something
}

IntMatrix & IntMatrix::operator=(const IntMatrix & rhs){
    if (this != &rhs){
        // new
        IntArray ** temp = new IntArray * [rhs.numRows];
        for (int i = 0; i < rhs.numRows; i++){
            temp[i] = new IntArray(rhs[i]);
        }
        // delete
        for (int i = 0; i < numRows; i++){
            delete rows[i];
        }
        delete[] rows;
        // assign 
        numRows = rhs.numRows;
        numColumns = rhs.numColumns;
        rows = temp;
    }
    return * this;
}

int IntMatrix::getRows() const{
    return numRows;
}

int IntMatrix::getColumns() const{
    return numColumns;
}

IntArray & IntMatrix::operator[](int index){
    assert((index >= 0) && (index < numRows));
    return * rows[index];
}

const IntArray & IntMatrix::operator[](int index) const{
    assert((index >= 0) && (index < numRows));
    return * rows[index];
}

bool IntMatrix::operator==(const IntMatrix & rhs) const {
    if (numRows != rhs.numRows){
        return false;
    }
    if (numColumns != rhs.numColumns){
        return false;
    }
    for (int i = 0; i < numRows; i++){
        if ((* this)[i] != rhs[i]){ // use (* this) to match the type of rhs, which is const..
            return false;
        }
    }
    return true;
}

IntMatrix IntMatrix::operator+(const IntMatrix & rhs) const {
    assert( (numRows == rhs.numRows) && (numColumns == rhs.numColumns));
    IntMatrix ans(numRows, numColumns); // initialize a class
    for (int i = 0; i < numRows; i++){
        for (int j = 0; j < numColumns; j++){
            ans[i][j] = (* this)[i][j] + rhs[i][j];
        }
    }
    return ans;
}

std::ostream & operator<<(std::ostream & s, const IntMatrix & rhs){
    if (rhs.getRows() == 0){
        s << "[ ]";
        return s;
    }
    s << "[";
    for (int i = 0; i < rhs.getRows() - 1; i++){
        s << rhs[i] << ",\n";
    }
    s << rhs[rhs.getRows() - 1] << "]";
    return s;
}