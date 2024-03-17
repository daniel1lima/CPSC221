/**
 * @file common_words.cpp
 * Implementation of the CommonWords class.
 *
 * @author Zach Widder
 * @date Fall 2014
 */

#include "common_words.h"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;
using std::feof;

string remove_punct(const string& str)
{
    string ret;
    std::remove_copy_if(str.begin(), str.end(), std::back_inserter(ret),
                        [](int c) { return std::ispunct(c); });
    return ret;
}

CommonWords::CommonWords(const vector<string>& filenames)
{
    // initialize all member variables
    init_file_word_maps(filenames);
    init_common();
}

void CommonWords::init_file_word_maps(const vector<string>& filenames)
{
    // make the length of file_word_maps the same as the length of filenames
    file_word_maps.resize(filenames.size());

    // go through all files
    for (size_t i = 0; i < filenames.size(); i++) {
        // get the corresponding vector of words that represents the current file
        vector<string> words = file_to_vector(filenames[i]);

        // go through all the words in the file

        for (size_t a = 0; a < words.size(); a++) {
            auto lookup = file_word_maps[i].find(words[a]);
            if (lookup != file_word_maps[i].end()) {
                // Do something if the word is found in the map
                file_word_maps[i].at(words[a]) =  file_word_maps[i].at(words[a]) += 1;
            } else {
                // Do something if the word is not found in the map
                file_word_maps[i].insert({words[a], 1});
            }
        }
    }
}

void CommonWords::init_common()
{ 
    // Iterate through the first map in file_word_maps
    for (const auto& pair : file_word_maps[0]) {
        const string& word = pair.first;
        int count = pair.second;

        bool isCommon = true;

        // Iterate through the remaining maps in file_word_maps
        for (size_t i = 1; i < file_word_maps.size(); i++) {
            auto lookup = file_word_maps[i].find(word);
            if (lookup == file_word_maps[i].end()) {
                isCommon = false;
                break;
            } else {
                count++;
            }
        }

        // If the word appears in all maps >= n times or appears only in one map, add it to the common words
        if (isCommon || count == 1) {
            common[word] = count;
        }
    }
}

/**
 * @param n The number of times to word has to appear.
 * @return A vector of strings. The vector contains all words that appear
 * in each file >= n times.
 */
vector<string> CommonWords::get_common_words(unsigned int n) const
{
    vector<string> out;
    
    // Iterate through the common words map
    for (const auto& pair : common) {
        const string& word = pair.first;
        unsigned int count = pair.second;
        
        // Check if the word appears >= n times in each file
        if (count >= n) {
            bool isCommon = true;
            
            // Iterate through the file_word_maps
            for (const auto& file_map : file_word_maps) {
                auto lookup = file_map.find(word);
                if (lookup == file_map.end() || lookup->second < n) {
                    isCommon = false;
                    break;
                }
            }
            
            // If the word appears >= n times in each file, add it to the output vector
            if (isCommon) {
                out.push_back(word);
            }
        }
    }
    
    return out;
}

/**
 * Takes a filename and transforms it to a vector of all words in that file.
 * @param filename The name of the file that will fill the vector
 */
vector<string> CommonWords::file_to_vector(const string& filename) const
{
    ifstream words(filename);
    vector<string> out;

    if (words.is_open()) {
        std::istream_iterator<string> word_iter(words);
        while (!words.eof()) {
            out.push_back(remove_punct(*word_iter));
            ++word_iter;
        }
    }
    return out;
}
