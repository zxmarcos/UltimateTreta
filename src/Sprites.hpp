#ifndef __SPRITES_HPP
#define __SPRITES_HPP

#include <string>
#include <map>
#include <allegro5/allegro_memfile.h>
#include <utility>
#include "Util.hpp"
#include "SpriteEffect.hpp"

using namespace std;
namespace Spr
{

class Sprite;

struct SpriteFileHeader
{
    char sign[12];
    char version[4];
    unsigned : 32; // reservado
    unsigned : 32; // reservado
    char compatver[4];
    unsigned : 32; // reservado
    unsigned : 32; // reservado
    unsigned int sprOffset;
    unsigned int nSprites;
    unsigned int palOffset;
    unsigned int nPalettes;
    unsigned int ldataOffset;
    unsigned int ldataSize;
    unsigned int tdataOffset;
    unsigned int tdataSize;
    unsigned : 32; // reservado
    unsigned : 32; // reservado
    char unused[436];
} __attribute__((packed));

typedef enum
{
    SPR_FMT_RAW = 0,
    SPR_FMT_INVALID,
    SPR_FMT_RLE8,
    SPR_FMT_RLE5,
    SPR_FMT_LZ5,
    SPR_FMT_PNG = 12
} SPRITE_DATA_FORMAT;

typedef enum
{
    SPR_FLAG_TDATA = 1,
} SPRITE_FLAGS;

struct SpriteNodeInfo
{
    short group;
    short index;
    short width;
    short height;
    short xaxis;
    short yaxis;
    short linked;
    char format;
    char depth;
    unsigned int offset;
    unsigned int size;
    short palno;
    unsigned short flags;
} __attribute__((packed));


class SpriteLoader
{
public:
    SpriteLoader();
    ~SpriteLoader();

    bool open(string filename);
    void close();
    const Sprite *get(int group, int index);
private:
    SpriteFileHeader header;
    void loadSpritesFromFile();
    void loadSpriteData(Sprite *spr, SpriteNodeInfo *sprnode, int thisIndex);
    ALLEGRO_FILE *memFile;
    unsigned char *buffer;
    unsigned int bufferSize;
    map<pair<int, int>, Sprite*> mapSprites;
    static Sprite defaultSpr;

};

class Sprite
{
public:
    Sprite();
    ~Sprite();
    void draw(int posX, int posY, const Effect& effect) const;
    int thisIndex;
    short group;
    short index;
    short xaxis;
    short yaxis;
    short palno;
    bool isLinked;
    ALLEGRO_BITMAP *bitmap;
    ImplementObjectCounter();
};

}

#endif //__SFFLOADER_HPP
