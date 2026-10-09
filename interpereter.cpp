#include <iostream>
#include <deque>
#include <stdexcept>
#include <vector>
#include <unordered_map>

using namespace std;

char EOF_CHR = '\0';
char COMMENT_CHAR = '!';
string snippet = R"(
(block
(set x 1)
(while (< x 10)
(block
(set x (+ 1 x))
(put x)
)
)

)
)";

bool allDigits(string str){
    for (char c : str){
        if (isdigit(c)==false){
            return false;
        }
    }
    return true;
};

bool allAlpha(string str){
    for (char c : str){
        if (isalpha(c)==false){
            return false;
        }
    }
    return true;
};

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
        string expressions = "<=+-*()";
        
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

};


enum class Types {Int, Var, Call};

struct Node {
    Types type;
    int value;
    string name;
    vector<Node> args;
};

Node makeInt(int v){
    Node n;
    n.type = Types::Int;
    n.value = v;
    return n;
};
Node makeVar(string s){
    Node n;
    n.type = Types::Var;
    n.name = s;
    return n;
};
Node makeCall(string cmd, vector<Node> args){
    Node n;
    n.type = Types::Call;
    n.name = cmd;
    n.args = args;
    return n;
};
Node parse(deque<string>& tokens){

    string token = tokens.front();
    tokens.pop_front();

    if (allDigits(token)){
        return makeInt(stoi(token));
    }
    else if (allAlpha(token)){
        return makeVar(token);

    }
    else if (token == "(") {
        string cmd = tokens.front();
        tokens.pop_front();
        vector<Node> args;
        while (tokens.front() != ")"){
           args.push_back(parse(tokens)); 
        }
        tokens.pop_front();
        return makeCall(cmd, args);
    }
    else {
        throw invalid_argument("EXCEPTION!");
    }
    
};

unordered_map<string, int> globalVars;

int add(vector<Node> args);
int multiply(vector<Node> args);
int divide(vector<Node> args);
int subtract(vector<Node> args);
int lessThan(vector<Node> args);

int evaluate(Node& parseTree){
    
    if (parseTree.type == Types::Int){
        return parseTree.value;
    }
    else if (parseTree.type == Types::Var){

        return globalVars.at(parseTree.name);

    }
    else if (parseTree.type == Types::Call){
        string cmd = parseTree.name;
        vector<Node> args = parseTree.args;

        if (cmd == "set"){
            string varName = args[0].name;
            int varVal = evaluate(args[1]);
            globalVars.insert_or_assign(varName, varVal);
            return varVal;
        }

        else if (cmd == "block"){
            for (Node arg : args){
                evaluate(arg);
            }
            return -1;
        }

        else if (cmd == "put"){
            cout << evaluate(args[0]);
            return -1;
        }

        else if (cmd == "if"){
            if (evaluate(args[0])==1){
                return evaluate(args[2]);
            }
            else {
                return evaluate(args[1]);
            }
        }

        else if (cmd == "while"){

            while (evaluate(args[0])==1){
                evaluate(args[1]);
            }

            return -1;

        }


        else if (cmd == "+"){
            return add(args);
        }

        else if (cmd == "-"){
            return subtract(args);
        }

        else if (cmd == "*"){
            return multiply(args);
        }

        else if (cmd == "/"){
            return divide(args);
        }

        else if (cmd == "<"){
            return lessThan(args);
        }
    }

};


int add(vector<Node> args){
    int sum = 0;

    for (Node arg : args){
        sum += evaluate(arg);
    }

    return sum;
}

int subtract(vector<Node> args){
    int result = 0;
    for (int i = 0; i <args.size(); i++){
        if (i == 0){
            result += evaluate(args[i]);
        }
        else{
            result -= evaluate(args[i]);
        }
    }

    return result;
}

int multiply(vector<Node> args){
    int result = 1;

    for (Node arg : args){
        result *= evaluate(arg);
    }

    return result;
}

int divide(vector<Node> args){
    int result = 0;
    for (int i = 0; i <args.size(); i++){
        if (i == 0){
            result += evaluate(args[i]);
        }
        else{
            result /= evaluate(args[i]);
        }
    }

    return result;
}

int lessThan(vector<Node> args){
    
    if (evaluate(args[0]) < evaluate(args[1])){
        return 1;
    }
    else {
        return 0;
    }

}

int main() {
    deque<string> tokens = tokenize(snippet);
    Node parsed = parse(tokens);

    evaluate(parsed);
    
};


