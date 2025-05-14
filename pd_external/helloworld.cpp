#include "helloworld.h"
#include "m_pd.h"
#include <filesystem>
#include <fstream>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <pwd.h>
#include <unistd.h>
#endif

std::filesystem::path getDocumentsPath() {
#if defined(_WIN32)
    PWSTR pathTmp = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathTmp))) {
        std::wstring ws(pathTmp);
        CoTaskMemFree(pathTmp);
        return std::filesystem::path(ws);
    } else {
        throw std::runtime_error("Failed to find Documents folder on Windows");
    }

#elif defined(__APPLE__) || defined(__linux__)
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        homeDir = getpwuid(getuid())->pw_dir;
    }
    return std::filesystem::path(homeDir) / "Documents";

#else
    throw std::runtime_error("Unsupported platform");
#endif
}


namespace pd_helloworld {

// Initialize the static class pointer
t_class* HelloWorld::class_ptr_ = nullptr;

HelloWorld::HelloWorld() {
    post("Hello world object created");

    const std::string songDataPath = getDocumentsPath().string() + "/songdata/";
    const std::string filename = "example.json";

    std::ifstream file(songDataPath + filename);

    if (file.is_open()) {
      json data = json::parse(file);
      post(data.dump(4).c_str());
    } else {
      startpost("Error loading song data cannont find:");
      poststring((songDataPath + filename).c_str());
      endpost();
    }
}

HelloWorld::~HelloWorld() {
    post("Hello world object deleted");
}

// Bang handler method
void HelloWorld::onBang() {
    post("Hello world !!!");
}

void HelloWorld::bangCallback(HelloWorld* x) {
    x->onBang();
}

void HelloWorld::destroyCallback(HelloWorld *x) {
    x->~HelloWorld();
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
        (t_method)destroyCallback,   // callback to c++ destructor 
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




