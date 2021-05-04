#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string.h>
#include <fstream>
#include <vector>
#include <map>
#include <set>


typedef std::string str;

// This function reads the categories and words and store them in a map
std::map<str, std::vector<str> > read_words(std::ifstream & f){
    std::map<str, std::vector<str> > wordmap;
    str line;
    while (!f.eof()){
        std::getline(f, line);
        if (line == ""){
            continue;
        }
        // store category
        if (line.find(":") == std::string::npos){
            std::cerr << "Invalid input.\n";
            exit(EXIT_FAILURE);
        }
        if (line.find(":") == 0){
            std::cerr << "Invalid word category.\n";
            exit(EXIT_FAILURE);
        }
        str cat = line.substr(0, line.find(":"));
        // store words
        if (line.length() <= cat.length() + 1){
            std::cerr << "Invalid word.\n";
            exit(EXIT_FAILURE);
        }
        str w = line.substr(line.find(":") + 1);
        // add to the map
        wordmap[cat].push_back(w);
    }
    return wordmap;
}

// This function reads the template 
std::vector<str> readtemp(std::ifstream & f){
    std::vector<str> temp;
    str line;
    while (!f.eof()){
        std::getline(f, line);
        if (line == ""){
            continue;
        }
        temp.push_back(line);
    }
    return temp;
}

// This function finds blanks in the template and replace them with random words
std::vector<str> replaceword(std::vector<str> temp, std::map<str, std::vector<str> > map){
    std::vector<str> used;
    std::vector<str> story;
    std::vector<str>::iterator line = temp.begin();
    
    while (line != temp.end()){
        size_t p1 = 0;
        while ((*line).find("_", p1) != std::string::npos){
            // read the category
            p1 = (* line).find("_") + 1;
            size_t p2 = (* line).find("_", p1);
            str cat = (* line).substr(p1, p2 - p1);
            str pick;
            if (! isdigit(cat[0])){  // find the words
                std::map<str, std::vector<str> >::iterator w = map.find(cat);
                if (w == map.end()){
                    std::cerr << "Category does not exist.\n";
                    exit(EXIT_FAILURE);
                }
                std::vector<str> words = (* w).second;
                size_t n = words.size();
                pick = words[rand() % n];
            } else{ // use used words
                size_t len = used.size();
                size_t pos;
                std::stringstream ss;
                ss << cat;
                ss >> pos;
                if (pos > len){
                    std::cerr << "Word index out of range.\n";
                    exit(EXIT_FAILURE);
                }
                pick = used[len-pos];
            }
            // replace the blank
            (* line).replace(p1 -1, p2 - p1 + 2, pick);
            // update position and used words
            p1 = p2 + 1;
            used.push_back(pick);
        }
        story.push_back(* line);
        ++line;
    }
    return story;
}

// This function prints out the story
void tellstory(std::vector<str> story){
    std::vector<str>::iterator it = story.begin();
    while (it != story.end()){
        std::cout << (* it) << std::endl;
        ++it;
    }
}


int main(int argc, char ** argv){
    if (argc != 4){
        std::cerr << "Wrong number of argument.\n";
        return EXIT_FAILURE;
    }
    if (!isdigit( *argv[3])){
        std::cerr << "Last argument must be an integer.\n";
        return EXIT_FAILURE;
    }
    unsigned seed = atoi(argv[3]);
    if (seed < 1){
        std::cerr << "Random seed must be greater or equal to 1.\n";
        return EXIT_FAILURE;
    }
    // read the words and categories
    std::ifstream f_words;
    f_words.open(argv[2]);
    if (f_words.fail()){
        std::cerr << "Cannot open the word file.\n";
        return EXIT_FAILURE;
    }
    // read the story
    std::ifstream f_story;
    f_story.open(argv[1]);
    if (f_story.fail()){
        std::cerr << "Cannot open the story file.\n";
        return EXIT_FAILURE;
    }
    // process
    srand(seed);
    std::map<str, std::vector<str> > wordmap = read_words(f_words);
    std::vector<str> temp = readtemp(f_story);
    std::vector<str> story = replaceword(temp, wordmap);
    tellstory(story);
   
    return EXIT_SUCCESS;
}
