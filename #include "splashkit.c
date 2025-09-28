#include "splashkit.h"

using std::to_string;
using std::stoi;

// Add read string code here
string read_string(string prompt)
{
    write(prompt);
    return read_line();
}

// Add read integer code here
int read_integer(string prompt)
{
    string input = read_string(prompt);
    while (!is_integer(input))
    {
        write_line("Please enter a valid integer.");
        input = read_string(prompt);

    }
    return stoi(input);
}

int read_integer(string prompt, int low, int high)
{
    int input = read_integer(prompt);
    if (low > high)
    {
        low = high;
        high = low;
    }
    while (input < low || input > high)
    {
        write_line("Please enter a value between " + to_string(low) + " and " + to_string(high));
        input = read_integer(prompt);
    }
    return input;
}

int main()
{
  string name;
  int age, value;

  // Read in a string and store it in name
  name = read_string("enter your name: ");

  // Output the name
  write_line("Hello " + name);

  // Read in an integer
  age = read_integer("what is your age: ");

  write_line("You entered " + to_string(age));
  
  value = read_integer("Enter a value between 10 and 1: ", 10, 1); 
  write_line("You entered " + to_string(value));
  return 0;
}