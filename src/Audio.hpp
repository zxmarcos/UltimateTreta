#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string>

using namespace std;

namespace Audio
{

class Music;

bool init(int samples);
void close();
void playBGM(ALLEGRO_SAMPLE *sample);
void playBGM_Stream(const char *str);
void stopBGM_Stream();
void stopBGM();

void playMusic(Music *music, float volume = 1.0);
void stopMusic(Music *music);

class Music
{
public:
    Music(string name);
    ~Music();
    ALLEGRO_SAMPLE *getSample();
    ALLEGRO_SAMPLE_INSTANCE *getInstance();
private:
    ALLEGRO_SAMPLE *sample;
    ALLEGRO_SAMPLE_INSTANCE *instance;
};

};
