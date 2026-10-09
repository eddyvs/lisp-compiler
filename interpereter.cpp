#include <iostream>
#include <deque>
#include <stdexcept>

using namespace std;

char EOF_CHR = '\0';
char COMMENT_CHAR = '!';
string snippet = R"(

(set x (+ 100 23023))
(set y (- blah blah))



)";

bool allDigits(string str){
    for (char c : str){
        if (isdigit(c)==false){
            return false;
        }
    }
    return true;
}

bool allAlpha(string str){
    for (char c : str){
        if (isalpha(c)==false){
            return false;
        }
    }
    return true;
}

deque<string> tokenize(string code){ 
    deque<char> rawCode;
    
    for (char c : code){
        rawCode.push_back(c);
    }
    
    rawCode.push_back(EOF_CHR);
    deque<string> tokens;


    while (rawCode[0] != EOF_CHR){
        char c = rawCode.front();        
        rawCode.pop_front();
        string expressions = "=+-*()";
        
        if (isspace(c) or c == ' ')
        {
            continue;
        }
        else if (isdigit(c)){
            string digit = string(1,c);
            
            while (isdigit(rawCode.front())){
                digit += string(1, rawCode.front());
                rawCode.pop_front();
            }

            tokens.push_back(digit);
        }
        else if (isalpha(c)){
            string name = string(1,c);
            
            while (isalpha(rawCode.front())){
                name += string(1, rawCode.front());
                rawCode.pop_front();
            } 
            
            tokens.push_back(name);
        }
        else if (expressions.contains(c)){
            tokens.push_back(string(1,c));
        }
        else {
            throw invalid_argument(format("invalid character: {}", c));
        }
    }
    tokens.push_back(string(1, EOF_CHR));

    return tokens;

}

deque<string> parse(deque<string> tokens){
string s = tokens[0];
deque<string> result;

tokens.pop_front();

    if (allDigits(s)){
        result.push_back(s);
        return result;
    }
    else if (allAlpha(s)){
        result.push_back(s);
        return result;
    }
    else if (s == "("){
        string expr = tokens[1];
        result.push_back(expr);
        
        while (s != ")"){
            tokens.pop_front();
            s = tokens[0];
            result.push_back(s);
        }
        return result;
    }
}




int main() {
    deque<string> tokens = tokenize(snippet);

    for (auto s : tokens){
        cout << s;
        cout << " ";
    }
}


