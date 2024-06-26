#ifndef ShapeAnalyzer_Exception_h
#define ShapeAnalyzer_Exception_h

#include <cstdlib>

using namespace std;

namespace util {

///
/// \brief Used to throw and catch exceptions within the ShapeAnalyzer.
/// \details Exceptions that inherit from Error are expected to be catched, or
/// it is generally possible to catch and handle them. Every namespace has its
/// own Error class to allow more specific error handling.
/// @author Emanuel Laude and Zorah Lähner
///
class Error : public exception {
public:
    /// \brief Standard constructor.
    /// \details If possible use the specific constructor Error(const std::string&) and
    /// give a message indicating the kind of problem.
    Error() : what_("An error occured in the Shape Analyzer framework.") {}
    /// \brief Specific constructor.
    /// \details The given string will be shown in an error dialog. Please make it meaningful.
    Error(const std::string& str) {
        what_ = strdup(str.c_str());
    }
    
    /// \brief Returns the what_-message indicating the kind of problem.
    virtual const char* what() const throw() { return what_; }
    
private:
    const char* what_;
};
    
}

#endif
