// Exceptions.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <exception>

struct MyException : std::runtime_error {
    MyException(const std::string& what) : std::runtime_error(what) {}
};

bool do_even_more_custom_application_logic()
{
    // DONE: Throw any standard exception
    // SOLUTION: I chose invalid_argument because I thought it was funny

    std::cout << "Running Even More Custom Application Logic." << std::endl;
    throw std::invalid_argument("Invalid Argument (for sake of argument)");

    return true;
}

void do_custom_application_logic()
{
    // DONE: Wrap the call to do_even_more_custom_application_logic()
    //  with an exception handler that catches std::exception, displays
    //  a message and the exception.what(), then continues processing
    std::cout << "Running Custom Application Logic." << std::endl;

    try 
    {
        if (do_even_more_custom_application_logic())
        {
            std::cout << "Even More Custom Application Logic Succeeded." << std::endl;
        }
    }
    catch (std::invalid_argument const& error) 
    {
        std::cerr << "Error in even more custom application logic: " << error.what() << std::endl;
    }

    // DONE: Throw a custom exception derived from std::exception
    //  and catch it explictly in main
    // SOLUTION: I threw my custom exception here

    throw MyException("Oh shoosh something went wrong, here is my custom exception struct!");

    std::cout << "Leaving Custom Application Logic." << std::endl;

}

float divide(float num, float den)
{
    // DONE: Throw an exception to deal with divide by zero errors using
    //  a standard C++ defined exception
    // SOLUTION: Checks for 0 denominator and throws overflow error

    if (den == 0)
    {
        throw std::overflow_error("Divide by Zero is undefined!");
        return num;
    }
    return (num / den);
}

void do_division() noexcept
{
    //  DONE: create an exception handler to capture ONLY the exception thrown
    //  by divide. 
    //  SOLUTION: Implemented overflow_error std exception to catch divide by zero errors

    float numerator = 10.0f;
    float denominator = 0;

    try {
       auto result = divide(numerator, denominator);
       std::cout << "divide(" << numerator << ", " << denominator << ") = " << result << std::endl;
    }
    catch (std::overflow_error const& error) {
        std::cerr << "Error in divide(): " << error.what() << std::endl;
    }

}

int main()
{
    std::exception_ptr eptr;

    try {
        std::cout << "Exceptions Tests!" << std::endl;

        // DONE: Create exception handlers that catch (in this order):
        //  your custom exception
        //  std::exception
        //  uncaught exception 
        //  that wraps the whole main function, and displays a message to the console.
        do_division();
        do_custom_application_logic();
    }
    // This is for my custom exception 
    catch (MyException const& error)
    {
        std::cerr << "Error in custom application logic: " << error.what() << std::endl;
    }
    catch (std::exception const& error)
    {
        std::cerr << "Error in custom application logic: " << error.what() << std::endl;
    }
    catch (...)
    {
        eptr = std::current_exception();
    }

    //for handling uncaught exceptions (in a real program this should probably be a method somewhere)
    try
    {
        if (eptr)
            std::rethrow_exception(eptr);
    }
    catch (const std::exception const& error)
    {
        std::cerr << "Error in custom application logic: " << error.what() << std::endl;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu