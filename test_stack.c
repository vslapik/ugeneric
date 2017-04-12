#include <stdio.h>
#include <string.h>
#include "stack.h"
#include "asserts.h"

#define N 129

bool is_balanced(const char *str)
{
    bool ret = true;
    stack_t *s = stack_create();
    for (size_t i = 0; i < strlen(str); i++)
    {
        char c = str[i];
        if (c == '[')
        {
            stack_push(s, G_INT(']'));
        }
        else if (c == '(')
        {
            stack_push(s, G_INT(')'));
        }
        else if (c == '{')
        {
            stack_push(s, G_INT('}'));
        }
        else
        {
            if (stack_is_empty(s) || G_AS_INT(stack_peek(s)) != c)
            {
                ret = false;
                break;
            }
            else
            {
                stack_pop(s);
            }
        }
    }

    ret &= stack_is_empty(s);
    stack_destroy(s);

    return ret;
}

void test_balanced_brackets(void)
{
    ASSERT(is_balanced("[]"));
    ASSERT(is_balanced("{[()]}"));
    ASSERT(is_balanced("{}[]()"));
    ASSERT(is_balanced("[()(){{}}]"));
    ASSERT(!is_balanced("["));
    ASSERT(!is_balanced(")"));
    ASSERT(!is_balanced("[()(){{}]"));
    ASSERT(!is_balanced("[][][]("));
    ASSERT(is_balanced("[[[[[[[[((((((((({{{{{{{{}}}}}}}})))))))))]]]]]]]]"));
    ASSERT(!is_balanced("[[[[[[[[((((((((({{{{{{{{[}}}}}}}})))))))))]]]]]]]]"));
}

void test_queue_on_two_stacks(void)
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

int calc(const char *exp)
{
    int a1;
    int a2;
    char op;
    stack_t *op_stack = stack_create();
    stack_t *arg_stack = stack_create();

    size_t explen = strlen(exp);
    for (size_t i = 0; i < explen; i++)
    {
        switch (exp[i])
        {
            case '+':
            case '-':
            case '/':
            case '*':
                stack_push(op_stack, G_INT(exp[i]));
                break;
            case ' ':
                break;
            case ')':
                a1 = G_AS_INT(stack_pop(arg_stack)) - 0x30;
                a2 = G_AS_INT(stack_pop(arg_stack)) - 0x30;
                op = G_AS_INT(stack_pop(op_stack));
                switch (op)
                {
                    case '+':
                        stack_push(arg_stack, G_INT((a1 + a2)));
                        break;
                    case '-':
                        stack_push(arg_stack, G_INT((a1 - a2)));
                        break;
                    case '/':
                        stack_push(arg_stack, G_INT((a1 / a2)));
                        break;
                    case '*':
                        stack_push(arg_stack, G_INT((a1 * a2)));
                        break;
                }
            default:
                continue;
        }
    }
    while (!stack_is_empty(op_stack))
    {
        a1 = G_AS_INT(stack_pop(arg_stack));
        a2 = G_AS_INT(stack_pop(arg_stack));
        op = G_AS_INT(stack_pop(op_stack));
        switch (op)
        {
            case '+':
                stack_push(arg_stack, G_INT((a1 + a2)));
                break;
            case '-':
                stack_push(arg_stack, G_INT((a1 - a2)));
                break;
            case '/':
                stack_push(arg_stack, G_INT((a1 / a2)));
                break;
            case '*':
                stack_push(arg_stack, G_INT((a1 * a2)));
                break;
        }
    }

    stack_destroy(arg_stack);
    stack_destroy(op_stack);

    return G_AS_INT(stack_pop(arg_stack));
}

void test_calc(void)
{
    //printf("%d", calc("2 + 3"));
}

void test_stack_api(void)
{
    stack_t *s = stack_create();
    ASSERT(stack_is_empty(s));

    stack_push(s, G_INT(N));
    ASSERT(!stack_is_empty(s));
    ASSERT(G_AS_INT(stack_pop(s)) == N);
    ASSERT(stack_is_empty(s));

    for (int i = 0; i < N; i++)
    {
        stack_push(s, G_INT(i));
    }
    ASSERT(!stack_is_empty(s));
    ASSERT(stack_get_size(s) == N);

    for (int i = 0; i < N; i++)
    {
        ASSERT(G_AS_INT(stack_peek(s)) == N - 1 - i);
        ASSERT(G_AS_INT(stack_pop(s)) == N - 1 - i);
    }
    ASSERT(stack_is_empty(s));

    stack_push(s, G_INT(N));
    ASSERT(!stack_is_empty(s));
    ASSERT(G_AS_INT(stack_pop(s)) == N);
    ASSERT(stack_is_empty(s));

    stack_destroy(s);

}

int main(void)
{
    test_calc();
    test_stack_api();
    test_balanced_brackets();

    return 0;
}
