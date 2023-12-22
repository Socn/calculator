#include <iostream> 
#include <string> 
#include <vector> 
#include <ctime>
using namespace std;

string ret;

class NOOODE {
public:
    string data;
    string type;
    NOOODE* left, * right;
    NOOODE* parent; // operator
    NOOODE(string d, string t) : data(d), type(t), left(NULL), right(NULL), parent(NULL) {}
};
class ExpressionTree {
public:
    NOOODE* root;
    int tsize;
    ExpressionTree() : root(NULL), tsize(0) {}
    //void insert(string s);
    void addLeafNOOODE(NOOODE* node);
    bool isOperator(string value);
};
bool ExpressionTree::isOperator(string value) {
    if (value == "+" || value == "-" ||
        value == "*" || value == "/" ||
        value == "==")
        return true;
    return false;
}
void inorder(NOOODE* node) {
    if (node) {
        if (node->left && node->parent)
            ret += "("; // cout << "(";
        inorder(node->left);
        ret += node->data;
        inorder(node->right);
        if (node->right && node->parent)
            ret += ")";// cout << ")";
    }
}
void randomOperandOrOperator(vector<string> operands, vector<string> operators, string* value, string* type, bool maxDepth) {
    if (!operators.size())
        maxDepth = 1;
    if (maxDepth == 1) {
        *value = operands[rand() % operands.size()];
        *type = "operand";
    }
    else {
        int percentage = rand() % 100 + 1;
        if (percentage <= 50) {
            *value = operands[rand() % operands.size()];
            *type = "operand";
        }
        else {
            *value = operators[rand() % operators.size()];
            *type = "operator";
        }
    }
}
NOOODE* getFirstFreeOperatorLeafNOOODE(NOOODE* root) {
    NOOODE* res = NULL;
    if (root == NULL)
        return NULL;
    if (root->type == "operator") {
        if (root->left == NULL || root->right == NULL)
            return root;
        if (root->left != NULL)
            res = getFirstFreeOperatorLeafNOOODE(root->left);
        if (res == NULL && root->right != NULL)
            res = getFirstFreeOperatorLeafNOOODE(root->right);
    }
    return res;
}
void ExpressionTree::addLeafNOOODE(NOOODE* node) {
    // tree is empty?
    if (root == NULL) {
        root = node;
        node->parent = NULL;
        node->left = NULL;
        node->right = NULL;
    }
    else {
        // add new node to first free operator leaf node
        NOOODE* lastOperatorLeaf = getFirstFreeOperatorLeafNOOODE(root);
        if (lastOperatorLeaf != NULL) {
            if (lastOperatorLeaf->left == NULL) {
                lastOperatorLeaf->left = node;
                node->parent = lastOperatorLeaf->left;
            }
            else
                if (lastOperatorLeaf->right == NULL) {
                    lastOperatorLeaf->right = node;
                    node->parent = lastOperatorLeaf->right;
                }
        }
    }
}
int getDepth(NOOODE* node) {
    if (node == NULL)
        return 0;
    else {
        int lDepth = getDepth(node->left);
        int rDepth = getDepth(node->right);

        if (lDepth > rDepth)
            return(lDepth + 1);
        else return(rDepth + 1);
    }
}

string randomExpression() {
    ret = "";
    srand(time(NULL));
    vector<string> operands = { "A", "B", "C" };
    vector<string> operators = { "+", "*", "-", "/", "^" };

    int maxDepth = 10;
    for (int i = 0; i < 1; i++) {
        operands = { to_string(rand() % 100), to_string(rand() % 100), to_string(rand() % 100), to_string(rand() % 100), to_string(rand() % 100) };
        ExpressionTree expression;
        do {
            string value, type;
            randomOperandOrOperator(operands, operators, &value, &type, (getDepth(expression.root) + 1 >= maxDepth));
            expression.addLeafNOOODE(new NOOODE(value, type));
        } while (getFirstFreeOperatorLeafNOOODE(expression.root) != NULL);
        // cout << i + 1 << ". depth: " << getDepth(expression.root) << " => ";
        inorder(expression.root);
        // cout << endl;
    }
    return ret;
}