#include <stdio.h>
#include <string.h>
#include "stack.h"
#include "asserts.h"
#include "ut_utils.h"

#define N 129

bool is_balanced(const char *str)
{
    bool ret = true;
    ustack_t *s = ustack_create();
    for (size_t i = 0; i < strlen(str); i++)
    {
        char c = str[i];
        if (c == '[')
        {
            ustack_push(s, G_INT(']'));
        }
        else if (c == '(')
        {
            ustack_push(s, G_INT(')'));
        }
        else if (c == '{')
        {
            ustack_push(s, G_INT('}'));
        }
        else
        {
            if (ustack_is_empty(s) || G_AS_INT(ustack_peek(s)) != c)
            {
                ret = false;
                break;
            }
            else
            {
                ustack_pop(s);
            }
        }
    }

    ret &= ustack_is_empty(s);
    ustack_destroy(s);

    return ret;
}

void test_balanced_brackets(void)
{
    UASSERT(is_balanced("[]"));
    UASSERT(is_balanced("{[()]}"));
    UASSERT(is_balanced("{}[]()"));
    UASSERT(is_balanced("[()(){{}}]"));
    UASSERT(!is_balanced("["));
    UASSERT(!is_balanced(")"));
    UASSERT(!is_balanced("[()(){{}]"));
    UASSERT(!is_balanced("[][][]("));
    UASSERT(is_balanced("[[[[[[[[((((((((({{{{{{{{}}}}}}}})))))))))]]]]]]]]"));
    UASSERT(!is_balanced("[[[[[[[[((((((((({{{{{{{{[}}}}}}}})))))))))]]]]]]]]"));
}

void test_uqueue_on_two_stacks(void)
{
    /*

        stack<int> push_stack, pop_stack;
        void push(int x) {
            push_stack.push(x);
        }

        void pop() {
            refill_pop_stack();
            pop_stack.pop();
        }

        int front() {
            refill_pop_stack();
            return pop_stack.top();
        }
        void refill_pop_stack(void) {
           if (pop_stack.empty()) {
                while (!push_stack.empty()) {
                    pop_stack.push(push_stack.top());
                    push_stack.pop();
                }
            }
        }
        */
}

// Dijkstra's shunting-yard algo, only integers (one digit wide), no
// operator precedence (should be explicitely defined by braces).
long int calc(const char *exp)
{
    int a1;
    int a2;
    char op;
    ustack_t *op_stack = ustack_create();
    ustack_t *arg_stack = ustack_create();

    size_t explen = strlen(exp);
    for (size_t i = 0; i < explen; i++)
    {
        switch (exp[i])
        {
            case '+':
            case '-':
            case '/':
            case '*':
            case '(':
                ustack_push(op_stack, G_INT(exp[i]));
                break;
            case ' ':
                break;
            case ')':
                while (G_AS_INT(ustack_peek(op_stack)) != '(')
                {
                    a1 = G_AS_INT(ustack_pop(arg_stack));
                    a2 = G_AS_INT(ustack_pop(arg_stack));
                    op = G_AS_INT(ustack_pop(op_stack));
                    switch (op)
                    {
                        case '+':
                            ustack_push(arg_stack, G_INT((a1 + a2)));
                            break;
                        case '-':
                            ustack_push(arg_stack, G_INT((a1 - a2)));
                            break;
                        case '/':
                            ustack_push(arg_stack, G_INT((a1 / a2)));
                            break;
                        case '*':
                            ustack_push(arg_stack, G_INT((a1 * a2)));
                            break;
                        default:
                            UABORT("parse error");
                    }
                }
                ustack_pop(op_stack);
                break;
            default:
                ustack_push(arg_stack, G_INT(exp[i] - '0'));
        }
    }
    while (!ustack_is_empty(op_stack))
    {
        a1 = G_AS_INT(ustack_pop(arg_stack));
        a2 = G_AS_INT(ustack_pop(arg_stack));
        op = G_AS_INT(ustack_pop(op_stack));
        switch (op)
        {
            case '+':
                ustack_push(arg_stack, G_INT((a1 + a2)));
                break;
            case '-':
                ustack_push(arg_stack, G_INT((a1 - a2)));
                break;
            case '/':
                ustack_push(arg_stack, G_INT((a1 / a2)));
                break;
            case '*':
                ustack_push(arg_stack, G_INT((a1 * a2)));
                break;
            default:
                UABORT("parse error");
        }
    }

    ugeneric_t result = ustack_pop(arg_stack);
    ustack_destroy(arg_stack);
    ustack_destroy(op_stack);

    return G_AS_INT(result);
}

void test_calc(void)
{
    #define C(exp) printf("%s = %ld\n", #exp, calc(exp));
    //printf("%ld\n", calc("2 + 3"));
    //C("2 + 3");
    //C("2 * 3");
    //C("(2 * 3) + 4");
    //C("(2 + 1 + 1) * 3");
    //C("((2 + 1 + 1) * 3) + (1 * 2)");
    UASSERT_INT_EQ(calc("((2 + 1 + 1) * 3) + (1 * 2)"), 14);
}

void test_ustack_api(void)
{
    ustack_t *s = ustack_create();
    UASSERT(ustack_is_empty(s));

    ustack_push(s, G_INT(N));
    UASSERT(!ustack_is_empty(s));
    UASSERT(G_AS_INT(ustack_pop(s)) == N);
    UASSERT(ustack_is_empty(s));

    for (int i = 0; i < N; i++)
    {
        ustack_push(s, G_INT(i));
    }
    UASSERT(!ustack_is_empty(s));
    UASSERT(ustack_get_size(s) == N);

    for (int i = 0; i < N; i++)
    {
        UASSERT(G_AS_INT(ustack_peek(s)) == N - 1 - i);
        UASSERT(G_AS_INT(ustack_pop(s)) == N - 1 - i);
    }
    UASSERT(ustack_is_empty(s));

    ustack_push(s, G_INT(N));
    UASSERT(!ustack_is_empty(s));
    UASSERT(G_AS_INT(ustack_pop(s)) == N);
    UASSERT(ustack_is_empty(s));

    ustack_destroy(s);

}

int main(void)
{
    test_calc();
    test_ustack_api();
    test_balanced_brackets();

    return 0;
}
