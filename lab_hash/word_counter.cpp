/**
 * @file word_counter.cpp
 * Implementation of the WordFreq class.
 *
 * @author Chase Geigle
 * @date Spring 2011
 * @date Spring 2012
 */

using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::pair;
using std::string;
using std::vector;

template <template <class K, class V> class Dict>
WordFreq<Dict>::WordFreq(const string& filename) : dict(256) {
    /**
     * @todo Implement this function.
     * @see char_counter.cpp if you're having trouble.
     */

    TextFile infile(filename);

    while (infile.good()) {
        string word = infile.getNextWord();
        if (!word.empty()) {
            dict[word]++;
        }
    }

    
}

template <template <class K, class V> class Dict>
vector<pair<string, int>> WordFreq<Dict>::getWords(int threshold) {
    /**
     * @todo Implement this function.
     * @see char_counter.cpp if you're having trouble.
     */

    vector<pair<string, int>> ret;

    for (const auto& kv : dict) {
        if (kv.second >= threshold) ret.push_back(kv);
    }


    return ret;
}
