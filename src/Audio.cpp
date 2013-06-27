// gerenciador rudimentar de sons...
// Marcos Medeiros
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Audio.hpp"
#include <iostream>

using namespace std;

namespace Audio
{

Music *currentMusic = NULL;

ALLEGRO_SAMPLE_INSTANCE *bgmInstance = NULL;
ALLEGRO_AUDIO_STREAM *stream = NULL;

bool init(int samples)
{
    al_init();
    if (!al_install_audio())
        return false;
    if (!al_init_acodec_addon())
        return false;
    // reserva samples para serem tocados simultaneamente
    if (!al_reserve_samples(samples))
        return false;

    return true;
}

void close()
{
}

void playBGM(ALLEGRO_SAMPLE *sample)
{
    if (!sample)
        return;
    bgmInstance = al_create_sample_instance(sample);
    if (!bgmInstance)
        return;
    al_attach_sample_instance_to_mixer(bgmInstance, al_get_default_mixer());
    al_play_sample_instance(bgmInstance);
}
void stopBGM()
{
    if (!bgmInstance)
        return;
    al_stop_sample_instance(bgmInstance);
}

void playBGM_Stream(const char *str)
{
    if (!str)
        return;
    stream = al_load_audio_stream(str, 2, 1024);
    if (!stream)
        return;
    al_set_audio_stream_playing(stream, true);
    al_set_audio_stream_playmode(stream, ALLEGRO_PLAYMODE_LOOP);
    al_attach_audio_stream_to_mixer(stream, al_get_default_mixer());
}

void stopBGM_Stream()
{
    if (!stream)
        return;
    al_detach_audio_stream(stream);
}

void playMusic(Music *music, float volume)
{
    if (!music)
        return;
    if (currentMusic)
        stopMusic(currentMusic);
    ALLEGRO_SAMPLE_INSTANCE *instance = music->getInstance();
    if (instance)
    {
        al_set_sample_instance_gain(instance, volume);
        al_play_sample_instance(instance);
        currentMusic = music;
    }
}

void stopMusic(Music *music)
{
    if (!music)
        return;
    ALLEGRO_SAMPLE_INSTANCE *instance = music->getInstance();
    if (instance)
    {
        al_stop_sample_instance(instance);
        if (music == currentMusic)
            currentMusic = NULL;
    }
}

Music::Music(string name)
{
    sample = NULL;
    instance = NULL;
    sample = al_load_sample(name.c_str());
    if (!sample)
        return;
    instance = al_create_sample_instance(sample);
    if (!instance)
    {
        al_destroy_sample(sample);
    }
    al_attach_sample_instance_to_mixer(instance, al_get_default_mixer());
    al_set_sample_instance_gain(instance, 1.0);
    al_set_sample_instance_playmode(instance, ALLEGRO_PLAYMODE_LOOP);
}

Music::~Music()
{
    stopMusic(this);
    if (sample)
        al_destroy_sample(sample);

    if (instance)
    {
        al_detach_sample_instance(instance);
        al_destroy_sample_instance(instance);
    }
}

ALLEGRO_SAMPLE *Music::getSample()
{
    return sample;
}

ALLEGRO_SAMPLE_INSTANCE *Music::getInstance()
{
    return instance;
}

}
