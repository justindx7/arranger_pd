#include "helloworld.h"
#include "m_pd.h"
#include <filesystem>
#include <fstream>

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
      data = json::parse(file);
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

void HelloWorld::onBang() {
    post("(bang)Hello world !!!");
}

void HelloWorld::onFloat(float input) {
    startpost("(float)Hello world: " );
    postfloat(input);
    endpost();
    int inputInt = static_cast<int>(input);

    if (data["programs"].contains(std::to_string(inputInt))) {
      json program = data["programs"][std::to_string(inputInt)];
      int bpm = program["bpm"];
      std::string file = program["filename"];

      startpost("Program ");
      postfloat(input);
      poststring(": BPM = ");
      postfloat(bpm);
      poststring(", File = ");
      poststring(file.c_str());
      endpost();
    }
}

void HelloWorld::bangCallback(HelloWorld* x) {
    x->onBang();
}

void HelloWorld::floatCallback(HelloWorld* x, t_floatarg f) {
    x->onFloat(f);
}

void HelloWorld::destroyCallback(HelloWorld *x) {
    x->~HelloWorld();
}

void* HelloWorld::create() {
    // Use placement new to create the object at the memory location allocated by pd_new
    return (void*)(new (pd_new(class_ptr_)) HelloWorld());
}

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
    class_addfloat(class_ptr_, (t_method)floatCallback);
}

} // End namespace 

extern "C" {
    void helloworld_setup() {
        pd_helloworld::HelloWorld::setup();
    }
}




