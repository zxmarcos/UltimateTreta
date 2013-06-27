#pragma once
#include <map>
#include <string>
#include <utility>
#include <allegro5/allegro_audio.h>
#include "Util.hpp"

using namespace std;

namespace Snd
{

struct SoundFileHeader
{
    unsigned char signature[12];
    unsigned int version;
    unsigned int samples;
    unsigned int offset;
    unsigned char __reserved[488];
} __attribute__((packed));

struct SoundSampleHeader
{
    unsigned int next;
    unsigned int length;
    int group;
    int index;
};

class Sample
{
public:
    Sample(short grp, short idx, ALLEGRO_SAMPLE *smp);
    ~Sample();
    void play() const;
    void stop() const;
private:
    short group;
    short index;
    ALLEGRO_SAMPLE *sample;
    ImplementObjectCounter();
};

class Sounds
{

public:
    Sounds();
    ~Sounds();
    bool open(string name);
    const Sample *get(int group, int index);
    void playSound(int group, int index);
private:
    SoundFileHeader header;
    std::map<pair<int, int>, Sample*> samples;
};

}
