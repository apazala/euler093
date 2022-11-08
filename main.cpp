// EulerHacker093.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>
#include <set>
#include <string>

using namespace std;

int mat[210][4];
int nrows;
int row[4];

struct qnum
{
    int num;
    int den;

    explicit qnum(int num) : num(num), den(1) {}
    explicit qnum(int num, int den) : num(num), den(den) {}
    qnum(const qnum &other) : num(other.num), den(other.den) {}

    bool is_positive_int()
    {
        if (den == 1)
            return (num > 0);
        if (num % den)
            return false;
        num /= den;
        den = 1;
        return (num > 0);
    }

    qnum operator+(const qnum &b) const
    {
        return qnum(b.den * num + den * b.num, den * b.den);
    }

    qnum operator-(const qnum &b) const
    {
        return qnum(b.den * num - den * b.num, den * b.den);
    }

    qnum operator*(const qnum &b) const
    {
        return qnum(num * b.num, den * b.den);
    }

    qnum operator/(const qnum &b) const
    {
        return qnum(num * b.den, den * b.num);
    }
};

vector<string> expressions_sintaxis;

void valid_sintaxes(string expression, int noperands, int noperators, int nvalues)
{
    if (noperands == 0 && noperators == 0)
    {
        if (nvalues == 1)
            expressions_sintaxis.push_back(expression);
        return;
    }

    if (noperators && nvalues >= 2)
    {
        valid_sintaxes(expression + "?", noperands, noperators - 1, nvalues - 1);
    }

    if (noperands)
    {
        valid_sintaxes(expression + "a", noperands - 1, noperators, nvalues + 1);
    }
}

int m;
vector<qnum> svec;

vector<vector<qnum>> sperms;

int permutation[5];
void spermutations(int mask, int i)
{
    if (i == m)
    {
        vector<qnum> aux;
        for (int j = 0; j < m; j++)
        {
            aux.push_back(svec[permutation[j]]);
        }

        sperms.push_back(aux);
        return;
    }

    for (int j = 0; j < m; j++)
    {
        if (mask & (1 << j))
            continue;
        permutation[i] = j;
        spermutations(mask | (1 << j), i + 1);
    }
}

int solve()
{
    char operatorscol[] = "+*-/";
    int nop = m - 1;
    char *operators = new char[m];
    operators[nop] = 0;

    int nbits = 2 * nop;
    int limit = 1 << nbits;

    // iterate through every operation combination
    set<int> values;
    for (vector<qnum> &perm : sperms)
    {

        for (int opcode = 0; opcode < limit; opcode++)
        {
            for (int i = 0; i < nop; i++)
            {
                int ind = (opcode >> 2 * i) & 3;
                operators[i] = operatorscol[ind];
            }

            for (string &expr : expressions_sintaxis)
            {
                vector<qnum> stack;
                int ioperand = 0, ioperator = 0;
                for (char &c : expr)
                {
                    if (c == 'a')
                    {
                        stack.push_back(perm[ioperand]);
                        ioperand++;
                    }
                    else
                    {
                        qnum operand2 = stack.back();
                        stack.pop_back();

                        if (operators[ioperator] == '/' && operand2.num == 0)
                        {
                            stack[0].num = 0; // This trigger invalid result
                            break;
                        }

                        qnum operand1 = stack.back();
                        stack.pop_back();

                        switch (operators[ioperator])
                        {
                        case '+':
                            stack.push_back(operand1 + operand2);
                            break;
                        case '-':
                            stack.push_back(operand1 - operand2);
                            break;
                        case '*':
                            stack.push_back(operand1 * operand2);
                            break;
                        case '/':
                            stack.push_back(operand1 / operand2);
                            break;
                        }

                        ioperator++;
                    }
                }

                if (!stack[0].is_positive_int())
                    continue;

                values.insert(stack[0].num);
            }
        }
    }

    auto it = values.lower_bound(1);
    if (it == values.end() || *it != 1)
        return 0;

    int lastval = 1;
    for (++it; it != values.end() && *it == lastval + 1; ++it)
        lastval++;

    return lastval;
}

void fillmat(int firstval, int col)
{

    for (int v = firstval; v <= 9; v++)
    {
        row[col] = v;

        if (col == 3)
        {
            for (int j = 0; j < 4; j++)
            {
                mat[nrows][j] = row[j];
            }
            nrows++;
        } else{
            fillmat(v+1, col + 1);
        }
        
    }
}

int main()
{
    m = 4;

    fillmat(0, 0);

    valid_sintaxes(string(""), m, m - 1, 0);


    int optim = 0;
    int ioptim = -1;

    for (int i = 0; i < 210; i++)
    {
        svec.clear();
        for (int j = 0; j < 4; j++)
            svec.push_back(qnum(mat[i][j]));

        sperms.clear();
        spermutations(0, 0);

        int sol = solve();
        if (sol > optim)
        {
            optim = sol;
            ioptim = i;
        }
    }

    for (int i = 0; i < 4; i++)
        cout << mat[ioptim][i];
    cout << endl;

    return 0;
}
