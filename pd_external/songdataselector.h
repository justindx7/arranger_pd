#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "m_pd.h"

// C++ namespace to avoid name collisions
namespace pd_songdataselector {
using json = nlohmann::json;

// Class declaration
class SongDataSelector {
private:
    t_object obj_; // The Pd object - must be first member
    t_outlet *out_songBPM,*out_fileBPM;

    t_outlet *out_section[15];

    //t_outlet *out_intro, *out_verse1, *out_verse2,*out_verse3, *out_fillin1, *out_fillin2, *out_outro;
    //t_outlet *out_sample1, *out_sample2, *out_sample3, *out_sample4, *out_sample5, *out_sample6, *out_sample7, *out_sample8;
    json data;

public:
    // Static class pointer required by Pd
    static t_class* class_ptr_;
    
    SongDataSelector();
    ~SongDataSelector();
    
    void onBang();
    void onFloat(float input);
    void initOutlets();
    void freeOutlets();
    
    // Static wrappers
    static void destroyCallback(SongDataSelector* x);
    static void bangCallback(SongDataSelector* x);
    static void floatCallback(SongDataSelector* x, t_floatarg f);
    
    // Static creator function
    static void* create();
    
    // Static setup function - registers the class with Pd
    static void setup();
};

} // end namespace 

// External setup function (must be C-style for Pd's dynamic loading)
extern "C" {
    void songdataselector_setup(void);
}

