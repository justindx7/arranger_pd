#include "helloworld.h"
#include "m_pd.h"

namespace pd_helloworld {

// Initialize the static class pointer
t_class* HelloWorld::class_ptr_ = nullptr;

// Constructor
HelloWorld::HelloWorld() {
    post("Hello world object created");
    // No additional initialization needed
}

// Destructor
HelloWorld::~HelloWorld() {
    post("Hello world object deleted");
}

// Bang handler method
void HelloWorld::onBang() {
    post("Hello world !!!");
}

// Static wrapper for the bang method
void HelloWorld::bangCallback(HelloWorld* x) {
    x->onBang();
}

// Static creator function
void* HelloWorld::create() {
    // Use placement new to create the object at the memory location allocated by pd_new
    return (void*)(new (pd_new(class_ptr_)) HelloWorld());
}

// Static setup function - registers the class with Pd
void HelloWorld::setup() {
    class_ptr_ = class_new(
        gensym("helloworld"),        // Object name
        (t_newmethod)create,         // Constructor
        0,                           // No special destructor
        sizeof(HelloWorld),          // Size of the data structure
        CLASS_DEFAULT,               // Normal Pd object
        (t_atomtype)0                // No creation arguments
    );
    
    // Register methods
    class_addbang(class_ptr_, (t_method)bangCallback);
}

} // End namespace pd_helloworld

// External setup function (must be C-style for Pd's dynamic loading)
extern "C" {
    void helloworld_setup() {
        pd_helloworld::HelloWorld::setup();
    }
}




