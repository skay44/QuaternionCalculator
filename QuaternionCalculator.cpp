#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

using namespace std;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

class Tokens {
    constexpr static char tokens[2][2] = { { '*','/' },{ '+','-' } };
    constexpr static char open = '(';
    constexpr static char close = ')';

public:

    static bool ifToken(char x, int value) {
        bool result = false;
        for (char t : tokens[value]) {
            if (x == t) {
                result = true;
                break;
            }
        }
        return result;
    }
};

class Tree {
public:
    string fragment;
    bool last;
    string operation;
    Tree* left;
    Tree* right;


    Tree(string expression) {
        fragment = expression;
        last = false;
        operation = "";
        left = nullptr;
        right = nullptr;
    }
};

struct Number {
public:
    string number;
    bool negative;

    Number() {
        number = "";
        negative = false;
    }
};

struct ComplexPart {
public:
    string var;
    string complex;

    ComplexPart() {
        var = "";
        complex = "";
    }
};


string removeSpace(const string& expression) {
    string result;
    for (char x : expression) {
        if (x != ' ') {
            result.push_back(x);
        }
    }
    return result;
}

int findLeastImportant(const string& expression, int& best) {
    int result = expression.length();
    int bestValue = 0;
    int brackets = 0;
    for (int i = expression.length() - 1; i >= 0; i--) {
        if (expression[i] == ')') {
            brackets++;
        }
        if (expression[i] == '(') {
            brackets--;
        }
        if (brackets == 0) {
            for (int j = 0; j < 2; j++) {
                if (Tokens::ifToken(expression[i], j)) {
                    if (bestValue <= j + 1) {
                        result = i;
                        bestValue = j + 1;
                    }
                }
            }
        }
    }
    best = bestValue;
    return result;
}

void split(Tree& tree, const string& expression, int wh) {
    //cout << "wh:" << wh << "\n";
    //cout << "l:" << expression.length() << "\n";
    //cout << "s:" << expression.length() - wh - 2 << "\n";
    tree.left = new Tree(expression.substr(0, wh));
    tree.right = new Tree(expression.substr(wh + 1, expression.length() - wh - 1));
}

bool removeBrackets(string& expression) {
    size_t l = expression.length();
    if (expression[0] == '(' && expression[expression.length() - 1] == ')') {
        expression = expression.substr(1, l - 2);
        return true;
    }
    return false;
}

void createTree(Tree* ex) {
    int v, pos;
    bool ifContinue = true;
    pos = findLeastImportant(ex->fragment, v);
    if (v == 0) {
        ifContinue = removeBrackets(ex->fragment);
        if (ifContinue) {
            createTree(ex);
        }
        else {
            ex->last = true;
        }
    }
    else {
        split(*ex, ex->fragment, pos);
        ex->operation = ex->fragment[pos];
        //cout << ex->left->fragment << ";" << ex->right->fragment << "\n";

        createTree(ex->left);
        createTree(ex->right);
    }
}

void printFragment(Tree* ex, int depth) {
    for (int i = 0; i < depth; i++) {
        cout << "|";
        for (int j = 0; j < 3; j++) {
            cout << " ";
        }
    }

    depth++;
    cout << "{";
    SetConsoleTextAttribute(hConsole, 6);
    cout << ex->fragment;
    SetConsoleTextAttribute(hConsole, 7);
    cout << "}" << "operation: <";
    SetConsoleTextAttribute(hConsole, 5);
    cout << ex->operation;
    SetConsoleTextAttribute(hConsole, 7);
    cout << ">\n";

    if (ex->last == false) {
        printFragment(ex->left, depth);
        printFragment(ex->right, depth);
    }
}

void printTree(Tree* ex) {
    int depth = 0;
    printFragment(ex, depth);
}



void devideIndependent(string x, vector<Number>& values) {
    Number* tmp;
    bool isnegative = false;
    string value = "";
    for (int i = 0; i < x.length(); i++) {
        if (x[i] == '+' || x[i] == '-') {
            //cout << "DFSF";
            if (value.length() > 0) {
                //MEMORY LEAK
                tmp = new Number;
                tmp->number = value;
                tmp->negative = isnegative;
                value = "";
                values.push_back(*tmp);
            }
            if (x[i] == '-') {
                isnegative = true;
            }
            else
                isnegative = false;
        }
        else {
            value.push_back(x[i]);
        }
    }
    if (value.length() > 0) {
        //MEMORY LEAK
        tmp = new Number;
        tmp->number = value;
        tmp->negative = isnegative;
        value = "";
        values.push_back(*tmp);
    }
}


string multiply(string l, string r) {
    //cout << "AAAA";
    //cout << l << " " << r << "\n";
    string result;

    vector<Number> lValues;
    vector<Number> rValues;


    devideIndependent(l, lValues);
    devideIndependent(r, rValues);

    //cout << "v";
    /*
    cout << "\n";
    for (int i = 0; i < lValues.size(); i++) {
        cout << "v:" << lValues[i].negative << " ";
    }
    cout << "\n";
    */

    for (int i = 0; i < lValues.size(); i++) {
        for (int j = 0; j < rValues.size(); j++) {
            if ((lValues[i].negative && rValues[j].negative) || ((!lValues[i].negative) && (!rValues[j].negative))) {
                result.append("+");
            }
            else {
                result.append("-");
            }
            result.append(lValues[i].number);
            result.append("*");
            result.append(rValues[j].number);
        }
    }
    return result;
}

string compound(Tree* l, Tree* r, string operation) {
    string lcomp = l->fragment;
    string rcomp = r->fragment;
    if (!l->last)
        lcomp = compound(l->left, l->right, l->operation);
    if (!r->last)
        rcomp = compound(r->left, r->right, r->operation);

    if (operation == "+") {
        return (lcomp + "+" + rcomp);
    }

    if (operation == "-") {
        return (lcomp + "-" + rcomp);
    }

    if (operation == "*") {
        return multiply(lcomp, rcomp);
    }

}

void devideIntoComplexParts(Number fragment, vector <ComplexPart>& part) {
    string numb = fragment.number;

    string tmp;
    string complexTmp;
    ComplexPart p;

    for (int a = 0; a < numb.size(); a++) {
        if (numb[a] == 'i' || numb[a] == 'j' || numb[a] == 'k') {
            complexTmp.push_back(numb[a]);
        }
        else if (numb[a] == '*') {
            p.var = tmp;
            p.complex = complexTmp;
            part.push_back(p);
            tmp = "";
            complexTmp = "";
        }
        else {
            tmp.push_back(numb[a]);
        }
    }
    p.var = tmp;
    p.complex = complexTmp;
    part.push_back(p);
    tmp = "";
    complexTmp = "";

    cout << "========\n";
    for (int i = 0; i < part.size(); i++) {
        cout << part[i].var << " " << part[i].complex << "\n";
    }
}


string simplifyQuaternion(string equation) {
    vector<Number> fragments;
    devideIndependent(equation, fragments);

    cout << "---------\n";
    for (int i = 0; i < fragments.size(); i++) {
        cout << fragments[i].negative << " " << fragments[i].number << "\n";
    }

    for (int i = 0; i < fragments.size(); i++) {
        vector<ComplexPart> parts;
        devideIntoComplexParts(fragments[i], parts);

    }

    return "";
}



string calculate(Tree* ex) {
    string comp = compound(ex->left, ex->right, ex->operation);
    cout << comp << "\n";
    return simplifyQuaternion(comp);
}

int main()
{

    string expression;
    getline(cin, expression);

    expression = removeSpace(expression);
    Tree* main = new Tree(expression);

    createTree(main);
    printTree(main);
    cout << calculate(main);

}