#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unordered_set>
#include <set>
#include <vector>


//declare class Profile                                                         
class Profile;
//declare explicit specialization for std::hash<Profile>                        
template<>
struct std::hash<Profile>;
//provided Qualities class                                                      
class Qualities {
 public:
  int age_min;
  int age_max;
  double height_min;
  double height_max;
  int attr_min;
  int attr_max;
  std::set<std::string> inter_req;
  Qualities(int a_min,
            int a_max,
            double h_min,
            double h_max,
            int at_min,
            int at_max,
            const std::set<std::string> & i_req):
            age_min(a_min), 
            age_max(a_max), 
            height_min(h_min),
            height_max(h_max),
            attr_min(at_min),
            attr_max(at_max),
            inter_req(i_req){} 
};

class Profile {
 private:
  std::string username;
  int age;
  double height;
  int attractiveness;
  std::set<std::string> interests;

 public:
  Profile() : username(""), age(0), height(0), attractiveness(0), interests() {}
  Profile(const std::string & un, int a, double h, int at):
  username(un), age(a), height(h), attractiveness(at), interests(){}
  void addInterest(const std::string & inter){
      interests.insert(inter);
  }
  bool operator==(const Profile & rhs) const{
      if (username != rhs.username){
          return false;
      }
      if (age != rhs.age){
          return false;
      }
      if (height != rhs.height){
          return false;
      }
      if (attractiveness != rhs.attractiveness){
          return false;
      }
      if (interests.size() != rhs.interests.size()){
          return false;
      }
      for (std::set<std::string>::iterator it= interests.begin(); it != interests.end(); ++it){
          if (rhs.interests.find(*it) == rhs.interests.end()){
              return false;
          }
      }
      return true;
  }
  bool isMatch(const Qualities & q) const{
      if ((age > q.age_max) || (age < q.age_min)){
          return false;
      }
      if ((height > q.height_max) || (height < q.height_min)){
          return false;
      }
      if ((attractiveness > q.attr_max)||(attractiveness < q.attr_min)){
          return false;
      }
      for (std::set<std::string>::iterator it= q.inter_req.begin(); it != q.inter_req.end(); ++it){
          if (interests.find(*it) == interests.end()){
              return false;
          }
      }
      return true;
  }
  friend std::ostream & operator<<(std::ostream & os, const Profile & rhs){
      os << "Username: " << rhs.username << std::endl;
      os << "    Age: "<< rhs.age << std::endl;
      os << "    Height: " << rhs.height << std::endl;
      os << "    Attractiveness: " << rhs.attractiveness << std::endl;
      os << "    Interests:";
      for (std::set<std::string>::iterator it = rhs.interests.begin(); it!=rhs.interests.end();++it){
          os << " " << * it;
      }
      os << std::endl;
      return os;
    }
  //add a friend, maybe 
  friend struct std::hash<Profile>;                                                        
};

//specialize std::hash<Profile>  
template<>
struct std::hash<Profile> {
    size_t operator() (const Profile & rhs) const {
        std::string key = rhs.username;
        size_t hash = std::hash<string>{}(key);
        return hash;
    }
};


// returns next comma-separated substring in str                                
// beginning at startp, storing location of next comma in endp                  
std::string nextSubstr(const std::string & str, size_t * startp, size_t * endp)\
 {
  if (*startp == std::string::npos) {
    return "";
  }
  *endp = str.find(',', *startp);
  std::string ans;
  if (*endp == std::string::npos) {
    ans = str.substr(*startp);
    *startp = *endp;
  }
  else {
    ans = str.substr(*startp, *endp - *startp);
    *startp = *endp + 1;
  }
  return ans;
}

// reads from file stream f and returns unordered_set of Profiles in file       
std::unordered_set<Profile> readProfiles(std::ifstream & f) {
  std::unordered_set<Profile> ans;
  std::string line;
  while (std::getline(f, line)) {
    size_t start = 0;
    size_t end;
    std::string uname = nextSubstr(line, &start, &end);
    int age = std::stoi(nextSubstr(line, &start, &end));
    double height = std::stod(nextSubstr(line, &start, &end));
    int attr = std::stoi(nextSubstr(line, &start, &end));
    Profile p(uname, age, height, attr);
    std::string i;
    while ((i = nextSubstr(line, &start, &end)).compare("")) {
      p.addInterest(i);
    }
    ans.insert(p);
  }
  return ans;
}


// reads from file stream f and returns a Qualities object                      
// describing the desired qualities to make a match                             
Qualities getQuals(std::ifstream & f) {
  std::string line;
  std::getline(f, line);
  size_t start = 0;
  size_t end;
  int a_min = std::stoi(nextSubstr(line, &start, &end));
  int a_max = std::stoi(nextSubstr(line, &start, &end));
  std::getline(f, line);
  start = 0;
  double h_min = std::stod(nextSubstr(line, &start, &end));
  double h_max = std::stod(nextSubstr(line, &start, &end));
  std::getline(f, line);
  start = 0;
  int at_min = std::stoi(nextSubstr(line, &start, &end));
  int at_max = std::stoi(nextSubstr(line, &start, &end));
  std::getline(f, line);
  start = 0;
  std::set<std::string> i_req;
  std::string i;
  while ((i = nextSubstr(line, &start, &end)).compare("")) {
    i_req.insert(i);
  }
  return Qualities(a_min, a_max, h_min, h_max, at_min, at_max, i_req);
}



int main(int argc, char ** argv){
    if (argc != 3){
        std::cerr << "Wrong number of arguments.\n";
        return EXIT_FAILURE;
    }
    // open files
    std::ifstream profile;
    profile.open(argv[1]);
    if (profile.fail()){
        std::cerr << "Fail to open the profile.\n";
        return EXIT_FAILURE;
    }
    std::ifstream qualities;
    qualities.open(argv[2]);
    if (qualities.fail()){
        std::cerr << "Fail to open the qualities file.\n";
        return EXIT_FAILURE;
    }
    // read profiles
    std::unordered_set<Profile> pro = readProfiles(profile);
    // read qualities
    Qualities qual = getQuals(qualities);
    // remove profiles that don't match
    std::unordered_set<Profile>::iterator p  = pro.begin();
    while (p != pro.end()){
        if (!(*p).isMatch(qual)){
            p = pro.erase(p);
            continue;
        }
        ++p;
    }
    //print the profiles that match
    for (std::unordered_set<Profile>::iterator it = pro.begin(); it != pro.end(); ++it){
        std::cout << * it;
    }
    

    
    return EXIT_SUCCESS;
}