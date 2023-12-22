#include "operators.h"
#include<stack>
#include<map>
#include<vector>
#include "myBitset.h"

const int T_SIZE = 1e5 + 10;
const int BITSET_SIZE = 128;
const Operator* nullOperator = new Operator();

std::map<char, const Operator*> charToOp;

enum TYPE { Numbers, Operators, Brackets, Functions };

class Node {
public:
    Node(TYPE t, Fraction n) :type(t), num(n), op(nullOperator), bracketType(false) {}
    Node(TYPE t, const Operator* o) :type(t), num(Fraction(0, 1)), op(o), bracketType(false) {}
    Node(TYPE t, bool b) :type(t), num(Fraction(0, 1)), op(nullOperator), bracketType(b) {}
    Node() :type(Numbers), num(Fraction(0, 1)), op(nullOperator), bracketType(false) {}
    TYPE type;
    Fraction num;
    const Operator* op;
    bool bracketType; //(True )False
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

bool isOperator(char c) {
    for (const Operator* op : _Operators) {
        if (c == op->operatorChar) {
            return true;
        }
    }
    return false;
}

bool isBracket(char c) {
    return c == '(' || c == ')';
}

void init() {
    for (const Operator* op : _Operators) {
        charToOp[op->operatorChar] = op;
    }
}

void initTree() {
    T = new Node[T_SIZE]{};
    Tree = new tree_node[T_SIZE << 2]{};
    bracketPos = new int[T_SIZE] {};
}

bool splitExp(std::string exp, Fraction previousResult) {
    int pos = 0;
    int expLen = exp.length();
    Fraction _num = Fraction(0, 1);
    bool flagNum = false;
    char _c;
    int cntBracket = 0;
    int cntPoint = 0;
    bool flagPoint = false;

    if (previousResult != 0)T[++cnt] = Node(Numbers, previousResult);

    std::stack<int> brackets;
    while (pos < expLen) {
        _c = exp[pos];
        if (isdigit(_c)) {
            _num = _num * 10 + _c - '0';
            flagNum = true;
            if (flagPoint) {
                cntPoint++;
            }
        }
        if (_c == '.' && flagNum) {
            flagPoint = true;
        }
        if (isOperator(_c)) {
            if (flagNum)T[++cnt] = Node(Numbers, _num / pow(10, cntPoint));
            T[++cnt] = Node(Operators, charToOp[_c]);
            _num = 0;
            flagNum = false;
            cntPoint = 0;
            flagPoint = false;
        }
        if (isBracket(_c)) {
            if (flagNum)T[++cnt] = Node(Numbers, _num / pow(10, cntPoint));
            T[++cnt] = Node(Brackets, _c == '(');
            _num = 0;
            flagNum = false;
            cntPoint = 0;
            flagPoint = false;
            if (_c == '(') {
                brackets.push(cnt);
                cntBracket++;
                maxCntBracket = std::max(cntBracket, maxCntBracket);
            }
            if (_c == ')') {
                if (brackets.empty()) {
                    return false;
                }
                bracketPos[cnt] = brackets.top();
                bracketPair[cntBracket].push_back({ brackets.top(), cnt });
                brackets.pop();
                cntBracket--;
            }
        }
        pos++;
    }
    if (flagNum)T[++cnt] = Node(Numbers, _num / pow(10, cntPoint));
    if (!brackets.empty())return false;

    return true;
}

void buildCalcTree(int l, int r, myBitset<BITSET_SIZE> c) {
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
            buildCalcTree(l, i - 1, c << 1);
            buildCalcTree(i + 1, r, c << 1 | 1);
            return;
        }
    }
    // printf("\n");
    buildCalcTree(l + 1, r - 1, c);
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
        return T[Tree[treeNodeHash[c]].id].num;
    }
    Fraction a = calcTree(c << 1), b = calcTree(c << 1 | 1);
    Fraction ret = calc(a, b, T[Tree[treeNodeHash[c]].id].op);
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

class CalcRequest {
public:
    std::string exp;
    Fraction previousResult;
    CalcRequest(std::string e, Fraction p) {
        exp = e;
        previousResult = p;
    }
    CalcRequest() {
        exp = "";
        previousResult = Fraction(0, 1);
    }
    friend bool operator<(const CalcRequest a, const CalcRequest b) {
        return true;
    }
};

std::map<CalcRequest, Fraction> calcCache;
Fraction calcExp(CalcRequest req) {
    if (calcCache.find(req) != calcCache.end()) {
        return calcCache[req];
    }
    myBitset<BITSET_SIZE> bitset1 = { 1 };
    resetCalcTree();
    initTree();
    if (!splitExp(req.exp, req.previousResult)) {
        return Fraction(0, 1);
    }
    buildCalcTree(1, cnt, bitset1);
    // printTree(1);printf("\n");
    Fraction res = calcTree(bitset1);
    return res;
}

std::string to_string(Fraction x) {
    return std::to_string((double)x);
}