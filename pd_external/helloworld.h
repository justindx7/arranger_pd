#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "m_pd.h"

// C++ namespace to avoid name collisions
namespace pd_helloworld {
using json = nlohmann::json;

// Class declaration
class HelloWorld {
private:
    t_object obj_; // The Pd object - must be first member

public:
    // Static class pointer required by Pd
    static t_class* class_ptr_;
    
    // Constructor & Destructor
    HelloWorld();
    ~HelloWorld();
    
    // Bang handler method
    void onBang();
    
    // Static wrapper for the bang method
    static void bangCallback(HelloWorld* x);
    static void destroyCallback(HelloWorld* x);
    
    // Static creator function
    static void* create();
    
    // Static setup function - registers the class with Pd
    static void setup();
};

} // End namespace pd_helloworld

// External setup function (must be C-style for Pd's dynamic loading)
extern "C" {
    void helloworld_setup(void);
}

