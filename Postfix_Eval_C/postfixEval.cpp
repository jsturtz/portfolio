/********************************************************************* 
** Author: Jordan Sturtz
** Date: 05-30-2018
** Description: The function postFixEval will evaluate a c-style 
** string representing a mathematical postfix expression and return
** the result. The input is a string with numbers or operators 
** delimited by whitespace. There is no input validation for this
** function. 
 *********************************************************************/
#include <iostream>
#include <stack>
#include <cstring>

using namespace std;


double postfixEval(char * input) {
    
    // strtok will grab the first token in input before the first whitespace char
    char * token = strtok(input, " ");
    
    // num will hold the double version of the parsed numbers
    double num;

    // the code will grab the last two numbers on the stack once an operator is found
    stack<double> stack;
    do {
        
        // checks to see whether token is a number or operator
        if (isdigit(token[0])){
            num = atof(token);
            stack.push(num);

        // if the token is an operator
        } else {
            
            // the top element on the stack is the first element in the binary operations
            // the second to top element on the stack is the second element in the binary operations
            double right = stack.top();
            stack.pop();
            double left = stack.top();
            stack.pop();
            
            // result stores the outcome of the relevant operation to push back to the stack
            double result;

            // switch chooses the correct operation for left and right numbers
            switch(token[0]) {
                case '+': 
                    result = left + right;
                    break;
                case '-': 
                    result = left - right;
                    break;
                case '*': 
                    result = left * right;
                    break;
                case '/': 
                    result = left / right;
                    break;

                default:
                    cout << "error, operator not found" << endl;
            }

            // adds result back to the stack
            stack.push(result);
        }

    // will continue so long as token isn't NULL
    } while((token = strtok(NULL, " ")));
    
    // the top of the stack will hold the final evaluation
    return stack.top();

}
