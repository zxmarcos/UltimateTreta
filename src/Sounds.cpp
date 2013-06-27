#include "Sounds.hpp"

#include <string>
#include <utility>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_memfile.h>
#include <fstream>
#include <iostream>
#include "Console.hpp"

using namespace std;

namespace Snd
{
InitObjectCounter(Sample);

Sample::Sample(short grp, short idx, ALLEGRO_SAMPLE *smpl)
    : group(grp), index(idx), sample(smpl)
{
    IncrementObjectCounter();
}

Sample::~Sample()
{
    if (sample)
        al_destroy_sample(sample);
    DecrementObjectCounter();
}

void Sample::play() const
{
    al_play_sample(sample, 3, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
}

Sounds::Sounds()
{
}
Sounds::~Sounds()
{
    cout << "Liberando samples..." << endl;
    int nsamples = Sample::getObjectCounter();
    for (auto i = samples.begin(); i != samples.end(); i++)
    {
        delete i->second;
    }
    nsamples = (nsamples- Sample::getObjectCounter());

    cout << nsamples << " samples desalocados com sucesso!" << endl;
}
bool Sounds::open(string name)
{
    unsigned char *buffer = NULL;
    unsigned int bufferSize = 0;
    ALLEGRO_FILE *memFile = NULL;
    Console::print("Sounds: carregando sons: %s", name.c_str());

    int nsamples = Sample::getObjectCounter();

    cout << "Carregando arquivo de sons: " << name << endl;
    double startTime = al_get_time();
    // Abre o arquivo e pega o seu tamanho
    ifstream is(name.c_str(), ios_base::binary);
    is.seekg(0, ios_base::end);
    bufferSize = is.tellg();
    is.seekg(0, ios_base::beg);

    // copia o arquivo para memória
    buffer = new unsigned char[bufferSize];
    if (!buffer)
    {
        cerr << "Falta de memoria!" << endl;
        exit(-1);
    }

    is.read((char *) buffer, bufferSize);
    memFile = al_open_memfile(buffer, bufferSize, "rb");
    if (!memFile)
    {
        delete buffer;
        cerr << "Erro ao criar memfile" << endl;
        exit(-1);
    }
    al_fseek(memFile, 0, ALLEGRO_SEEK_SET);
    // Lê o cabeçalho do arquivo
    al_fread(memFile, &header, sizeof(SoundFileHeader));
    al_fseek(memFile, header.offset, ALLEGRO_SEEK_SET);
    for (unsigned i = 0; i < header.samples; i++) {
        ALLEGRO_SAMPLE *alsample = NULL;

        SoundSampleHeader sheader;
        al_fread(memFile, &sheader, sizeof(SoundSampleHeader));

        alsample = al_load_sample_f(memFile, ".wav");
        if (alsample)
        {
            Sample *sample = new Sample(sheader.group, sheader.index, alsample);
            samples[make_pair(sheader.group, sheader.index)] = sample;
        }
        else
        {
            cout << "Falha ao carregar o sample " << sheader.group << "," << sheader.index << endl;
        }

        al_fseek(memFile, sheader.next, ALLEGRO_SEEK_SET);
    }

    double elapsed = al_get_time() - startTime;
    nsamples = (Sample::getObjectCounter() - nsamples);
    cout << nsamples << " samples alocados com sucesso!" << endl;
    cout << "Arquivo carregado com sucesso em " << elapsed << "s" << endl;

    delete [] buffer;
    al_fclose(memFile);

    return true;
}

const Sample *Sounds::get(int group, int index)
{
    return samples[make_pair(group, index)];
}

void Sounds::playSound(int group, int index)
{
    const Sample *sample = get(group, index);
    if (sample)
        sample->play();
}

}
