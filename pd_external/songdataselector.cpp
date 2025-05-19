#include "songdataselector.h"
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


namespace pd_songdataselector {

// Initialize the static class pointer
t_class* SongDataSelector::class_ptr_ = nullptr;

void SongDataSelector::initOutlets() {
    out_songBPM = outlet_new(&obj_, &s_float);
    out_fileBPM = outlet_new(&obj_, &s_float);

    for(int i = 0; i < 15; i++) {   
        out_section[i] = outlet_new(&obj_, &s_symbol);

    }

    /* out_intro = outlet_new(&obj_, &s_symbol);
    out_verse1 = outlet_new(&obj_, &s_symbol);
    out_verse2 = outlet_new(&obj_, &s_symbol);
    out_verse3 = outlet_new(&obj_, &s_symbol);
    out_fillin1 = outlet_new(&obj_, &s_symbol);
    out_fillin2 = outlet_new(&obj_, &s_symbol);
    out_outro = outlet_new(&obj_, &s_symbol);

    out_sample1 = outlet_new(&obj_, &s_symbol);
    out_sample2 = outlet_new(&obj_, &s_symbol);
    out_sample3 = outlet_new(&obj_, &s_symbol);
    out_sample4 = outlet_new(&obj_, &s_symbol);
    out_sample5 = outlet_new(&obj_, &s_symbol);
    out_sample6 = outlet_new(&obj_, &s_symbol);
    out_sample7 = outlet_new(&obj_, &s_symbol);
    out_sample8 = outlet_new(&obj_, &s_symbol);
    */
}

SongDataSelector::SongDataSelector() {
    post("Hello world object created");
    initOutlets();

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

void SongDataSelector::freeOutlets() {
    outlet_free(out_songBPM);
    outlet_free(out_fileBPM);

    for(int i = 0; i < 15; i++) {   
        outlet_free(out_section[i]);

    }

    /*
    outlet_free(out_intro);
    outlet_free(out_verse1);
    outlet_free(out_verse2);
    outlet_free(out_verse3);
    outlet_free(out_fillin1);
    outlet_free(out_fillin2);
    outlet_free(out_outro);

    outlet_free(out_sample1);
    outlet_free(out_sample2);
    outlet_free(out_sample3);
    outlet_free(out_sample4);
    outlet_free(out_sample5);
    outlet_free(out_sample6);
    outlet_free(out_sample7);
    outlet_free(out_sample8);
    */

}

SongDataSelector::~SongDataSelector() {
    post("Hello world object deleted");
}

void SongDataSelector::onBang() {
    post("(bang)Hello world !!!");

}

void SongDataSelector::onFloat(float input) {
  startpost("(float)Hello world: ");
  postfloat(input);
  endpost();

  int inputInt = static_cast<int>(input);
  std::string inputStr = std::to_string(inputInt);

  if (!data.contains("programs") || !data["programs"].contains(inputStr)) {
    startpost("Program not found.");
    endpost();
    return;
  }

  json program = data["programs"][inputStr];
  int songbpm = program.value("songbpm", 0);

  startpost("Program ");
  postfloat(inputInt);
  poststring(": songBPM = ");
  postfloat(songbpm);
  endpost();

  t_float bpmFloat = static_cast<t_float>(songbpm);
  outlet_float(out_songBPM, bpmFloat);

  // List of expected section keys
  const std::vector<std::string> sectionKeys = {
      "filebpm",  "intro",   "verse1",  "verse2",  "verse3",  "fill-in1",
      "fill-in2", "outro",   "sample1", "sample2", "sample3", "sample4",
      "sample5",  "sample6", "sample7", "sample8"};

  if (!program.contains("sections")) {
    startpost("  No sections found.");
    endpost();
    return;
  }

  const json &sections = program["sections"];
  int index = 0; 
  for (const auto &key : sectionKeys) {
    if (sections.contains(key)) {
      startpost("  ");
      poststring(key.c_str());
      poststring(" = ");
      const auto &val = sections[key];
      if (val.is_string()) {
        poststring(val.get<std::string>().c_str());

        if (val.get<std::string>() == "") {
          t_symbol *symB = gensym("empty");
          outlet_symbol(out_section[index - 1], symB);
        } else {
          t_symbol *symB = gensym(val.get<std::string>().c_str());
          outlet_symbol(out_section[index - 1], symB);
        }

      } else if (val.is_number_integer()) {
        postfloat(val.get<int>());
        outlet_float(out_fileBPM, val.get<int>());
      } else if (val.is_number()) {
        postfloat(val.get<float>());
      } else {
        poststring("<unknown>");
      }
      endpost();
    }
    index++;
  }


  //t_symbol *symB = gensym(file.c_str());
  //outlet_symbol(out_intro, symB);
}

void SongDataSelector::bangCallback(SongDataSelector* x) {
    x->onBang();
}

void SongDataSelector::floatCallback(SongDataSelector* x, t_floatarg f) {
    x->onFloat(f);
}

void SongDataSelector::destroyCallback(SongDataSelector *x) {
    x->~SongDataSelector();
}

void* SongDataSelector::create() {
    // Use placement new to create the object at the memory location allocated by pd_new
    return (void*)(new (pd_new(class_ptr_)) SongDataSelector());
}

void SongDataSelector::setup() {
    class_ptr_ = class_new(
        gensym("songdataselector"),        // Object name
        (t_newmethod)create,         // Constructor
        (t_method)destroyCallback,   // callback to c++ destructor 
        sizeof(SongDataSelector),          // Size of the data structure
        CLASS_DEFAULT,               // Normal Pd object
        (t_atomtype)0                // No creation arguments
    );
    
    // Register methods
    class_addbang(class_ptr_, (t_method)bangCallback);
    class_addfloat(class_ptr_, (t_method)floatCallback);


}

} // End namespace 

extern "C" {
    void songdataselector_setup() {
        pd_songdataselector::SongDataSelector::setup();
    }
}




