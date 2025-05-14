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
    t_outlet *out_A, *out_B; 
    json data;

public:
    // Static class pointer required by Pd
    static t_class* class_ptr_;
    
    HelloWorld();
    ~HelloWorld();
    
    void onBang();
    void onFloat(float input);
    
    // Static wrappers
    static void destroyCallback(HelloWorld* x);
    static void bangCallback(HelloWorld* x);
    static void floatCallback(HelloWorld* x, t_floatarg f);
    
    // Static creator function
    static void* create();
    
    // Static setup function - registers the class with Pd
    static void setup();
};

} // end namespace 

// External setup function (must be C-style for Pd's dynamic loading)
extern "C" {
    void helloworld_setup(void);
}

