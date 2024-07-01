#include "operators.h"
#include <stack>
#include <map>
#include <vector>
#include "myBitset.h"
#include "functions.h"
#include "constants.h"

const int T_SIZE = 1e5 + 10;
const int BITSET_SIZE = 128;

// Some New Function
// or sth
// ...

std::map<wchar_t, const Operator*> charToOp;
std::map<wchar_t, const Function*> charToFunc;
std::map<wchar_t, const Constant*> charToConst;

enum TYPE { Numbers, Operators, Brackets, Functions, Constants };

class Node {
public:
    Node(TYPE t, Fraction n) :type(t), num(n) {}
    Node(TYPE t, const Operator* o) :type(t), op(o) {}
    Node(TYPE t, bool b) :type(t), bracketType(b) {}
    Node(TYPE t, const Constant* c, Fraction cM = Fraction(1, 1)) :type(t), constant(c), constantMultiplier(cM) {}
    Node() :type(Numbers) {}
    TYPE type;

    Fraction num;

    const Operator* op = nullptr;

    bool bracketType = false; //(True )False

    const Function* func = nullptr;
    bool funcFlag = false;

    const Constant* constant;
    Fraction constantMultiplier = Fraction(1, 1);
};

Node* T;
struct tree_node {
    int id;
};
tree_node* Tree;
int* bracketPos; // ')' to '('
int cnt = 0;
int maxCntBracket = 0;
std::vector<std::pair<int, int>> bracketPair[50];
std::map<myBitset<BITSET_SIZE>, int> treeNodeHash;
int nodeCnt = 0;

class CalcRequest {
public:
    std::wstring exp;
    Fraction previousResult;
    bool hasPreviousResult;
    CalcRequest(std::wstring e, Fraction p, bool h) {
        exp = e;
        previousResult = p;
        hasPreviousResult = h;
    }
    CalcRequest() {
        exp = L"";
        previousResult = Fraction(0, 1);
        hasPreviousResult = false;
    }
    friend bool operator<(const CalcRequest a, const CalcRequest b) {
        return true;
    }
};

bool isOperator(wchar_t c) {
    for (const Operator* op : _Operators) {
        if (c == op->operatorChar) {
            return true;
        }
    }
    return false;
}
bool isFunction(wchar_t c) {
    for (const Function* func : _Functions) {
        if (c == func->functionChar) {
            return true;
        }
    }
    return false;
}
bool isBracket(wchar_t c) {
    return c == '(' || c == ')';
}
bool isConstant(wchar_t c) {
    for (const Constant* constant : _Constants) {
        if (c == constant->constantChar) {
            return true;
        }
    }
    return false;
}

void init() {
    for (const Operator* op : _Operators) {
        charToOp[op->operatorChar] = op;
    }
    for (const Function* func : _Functions) {
        charToFunc[func->functionChar] = func;
    }
    for (const Constant* constant : _Constants) {
        charToConst[constant->constantChar] = constant;
    }
}

void initTree() {
    T = new Node[T_SIZE]{};
    Tree = new tree_node[T_SIZE << 2]{};
    bracketPos = new int[T_SIZE] {};
}

bool splitExp(CalcRequest req) {
    int pos = 0;
    std::wstring exp = req.exp;
    int expLen = exp.length();
    Fraction _num = Fraction(0, 1);
    bool flagNum = false;
    wchar_t _c;
    int cntBracket = 0;
    int cntPoint = 0;
    bool flagPoint = false;

    if (req.hasPreviousResult)T[++cnt] = Node(Numbers, req.previousResult);

    std::stack<int> brackets;
    std::stack<std::pair<int, const Function*>> functionStack;

    bool correctFlag = true;

    Operator lastOp;
    while (pos < expLen) {
        _c = exp[pos];
        if (isdigit(_c)) {
            _num = _num * 10 + _c - '0';
            flagNum = true;
            if (flagPoint) {
                cntPoint++;
            }
            pos++;
            continue;
        }
        if (_c == '.' && flagNum) {
            flagPoint = true;
            pos++;
            continue;
        }
        if (isOperator(_c)) {
            if (flagNum) {
                T[++cnt] = Node(Numbers, _num / pow(10, cntPoint));
                if (!functionStack.empty()) {
                    T[cnt].funcFlag = true;
                    T[cnt].func = functionStack.top().second;
                    functionStack.pop();
                }
            }
            if (charToOp[_c]->inversed == true && charToOp[_c]->operatorChar == lastOp.operatorChar) {
                T[++cnt] = Node(Operators, new Operator(charToOp[_c], charToOp[_c]->priority + 1));
            }
            else T[++cnt] = Node(Operators, charToOp[_c]);
            _num = 0;
            flagNum = false;
            cntPoint = 0;
            flagPoint = false;
            pos++;
            lastOp = *charToOp[_c];
            continue;
        }
        if (isBracket(_c)) {
            if (flagNum) {
                T[++cnt] = Node(Numbers, _num / pow(10, cntPoint));
            }
            T[++cnt] = Node(Brackets, _c == '(');
            _num = 0;
            flagNum = false;
            cntPoint = 0;
            flagPoint = false;
            if (_c == '(') {
                brackets.push(cnt);
                cntBracket++;
                maxCntBracket = std::max(cntBracket, maxCntBracket);
                if (!functionStack.empty() && functionStack.top().first + 1 == cntBracket) {
                    T[cnt].funcFlag = true;
                    T[cnt].func = functionStack.top().second;
                    functionStack.pop();
                }
            }
            if (_c == ')') {
                if (brackets.empty()) {
                    return false;
                }
                bracketPos[cnt] = brackets.top();
                bracketPair[cntBracket].push_back({ brackets.top(), cnt });

                T[cnt].funcFlag = T[brackets.top()].funcFlag;
                T[cnt].func = T[brackets.top()].func;
                brackets.pop();
                cntBracket--;
            }
            pos++;
            continue;
        }
        if (isFunction(_c)) {
            if (flagNum)T[++cnt] = Node(Numbers, _num / pow(10, cntPoint));
            functionStack.push({ cntBracket,charToFunc[_c] });
            _num = 0;
            flagNum = false;
            cntPoint = 0;
            flagPoint = false;
            pos++;
            continue;
        }
        if (isConstant(_c)) {
            if (pos == 0 && req.hasPreviousResult) {
                T[cnt] = Node(Constants, charToConst[_c], (double)req.previousResult);
                pos++;
                continue;
            }
            if (T[cnt].type == Brackets && T[cnt].bracketType == false) {
                T[++cnt] = Node(Operators, charToOp['*']);
            }
            if (flagNum) {
                T[++cnt] = Node(Constants, charToConst[_c], _num / pow(10, cntPoint));
                _num = 0;
                flagNum = false;
                cntPoint = 0;
                flagPoint = false;
            }
            else T[++cnt] = Node(Constants, charToConst[_c]);
            if (!functionStack.empty()) {
                T[cnt].funcFlag = true;
                T[cnt].func = functionStack.top().second;
                functionStack.pop();
            }
            pos++;
            continue;
        }
        pos++;
    }
    if (flagNum) {
        T[++cnt] = Node(Numbers, _num / pow(10, cntPoint));
        if (!functionStack.empty()) {
            T[cnt].funcFlag = true;
            T[cnt].func = functionStack.top().second;
            functionStack.pop();
        }
    }
    if (!brackets.empty())return false;

    return correctFlag;
}

void buildCalcTree(int l, int r, myBitset<BITSET_SIZE> c, const Function* func = nullptr) {
    // printf("%d %d %d\n", l, r, c);
    if (l > r) {
        tree_node _temp = { 0 };
        treeNodeHash[c] = ++nodeCnt;
        Tree[treeNodeHash[c]] = _temp;
        return;
    }
    if (l == r) {
        tree_node _temp = { l };
        treeNodeHash[c] = ++nodeCnt;
        Tree[treeNodeHash[c]] = _temp;
        if (func != nullptr) {
            T[Tree[treeNodeHash[c]].id].funcFlag = true;
            T[Tree[treeNodeHash[c]].id].func = func;
        }
        return;
    }
    bool flagBracket = false;
    bool priorityFlags[10]{};
    for (int i = r;i >= l;i--) {
        if (T[i].type == Brackets && !T[i].bracketType) {
            i = bracketPos[i];
            continue;
        }
        if (T[i].type == Operators) {
            priorityFlags[T[i].op->priority] = true;
        }
        if (T[i].type == Brackets) {
            flagBracket = true;
        }
    }
    for (int i = r;i >= l;i--) {
        if (T[i].type == Brackets && !T[i].bracketType) {
            i = bracketPos[i];
            continue;
        }
        if (T[i].type == Operators) {
            bool continueFlag = false;
            for (int p = T[i].op->priority - 1;p >= 0;p--) {
                if (priorityFlags[p]) {
                    continueFlag = true;
                }
            }
            if (continueFlag) {
                continue;
            }
            tree_node _temp = { i };
            treeNodeHash[c] = ++nodeCnt;
            Tree[treeNodeHash[c]] = _temp;
            if (func != nullptr) {
                T[i].funcFlag = true;
                T[i].func = func;
            }
            buildCalcTree(l, i - 1, c << 1);
            buildCalcTree(i + 1, r, c << 1 | 1);
            return;
        }
    }
    // printf("\n");
    buildCalcTree(l + 1, r - 1, c, T[l].func);
}

void printTree(myBitset<BITSET_SIZE> c) {
    if (T[Tree[treeNodeHash[c]].id].type == Numbers) {
        std::cout << T[Tree[treeNodeHash[c]].id].num;
        return;
    }
    if (Tree[treeNodeHash[c << 1]].id)printTree(c << 1);
    if (Tree[treeNodeHash[c << 1 | 1]].id)printTree(c << 1 | 1);
    std::cout << T[Tree[treeNodeHash[c]].id].op->operatorChar;
}

Fraction calc(Fraction a, Fraction b, const Operator* op) {
    return op->calc(a, b);
}

Fraction calcTree(myBitset<BITSET_SIZE> c) {
    if (!Tree[treeNodeHash[c]].id) {
        return Fraction(0, 1);
    }
    if (T[Tree[treeNodeHash[c]].id].type == Numbers) {
        Fraction ret = T[Tree[treeNodeHash[c]].id].num;
        if (T[Tree[treeNodeHash[c]].id].funcFlag) {
            ret = T[Tree[treeNodeHash[c]].id].func->calc(ret);
        }
        return ret;
    }
    if (T[Tree[treeNodeHash[c]].id].type == Constants) {
        Fraction ret = T[Tree[treeNodeHash[c]].id].constantMultiplier * T[Tree[treeNodeHash[c]].id].constant->value;
        if (T[Tree[treeNodeHash[c]].id].funcFlag) {
            ret = T[Tree[treeNodeHash[c]].id].func->calc(ret);
        }
        return ret;
    }
    Fraction a = calcTree(c << 1), b = calcTree(c << 1 | 1);
    Fraction ret = calc(a, b, T[Tree[treeNodeHash[c]].id].op);
    if (T[Tree[treeNodeHash[c]].id].funcFlag) {
        ret = T[Tree[treeNodeHash[c]].id].func->calc(ret);
    }
    // printf("%d:%f%c%f=%f\n", c, (double)a, T[Tree[c].id].op->operatorChar, (double)b, (double)ret);
    return ret;
}

void resetCalcTree() {
    cnt = 0;
    delete T;
    delete Tree;
    delete bracketPos;
    treeNodeHash.clear();
    nodeCnt = 0;
}

std::map<CalcRequest, Fraction> calcCache;
Fraction calcExp(CalcRequest req) {
    if (calcCache.find(req) != calcCache.end()) {
        return calcCache[req];
    }
    myBitset<BITSET_SIZE> bitset1 = { 1 };
    resetCalcTree();
    initTree();
    if (!splitExp(req)) {
        return Fraction(0, 1);
    }
    // try{
    buildCalcTree(1, cnt, bitset1);
    // }
    // catch(){
    //     return Fraction(0);
    // }
    // printTree(bitset1);printf("\n");
    Fraction res = calcTree(bitset1);
    return res;
}

std::string to_string(Fraction x) {
    return std::to_string((double)x);
}