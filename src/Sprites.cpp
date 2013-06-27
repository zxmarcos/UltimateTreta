//=============================================================================
// Módulo capaz de carregar arquivos de sprites do MUGEN SFFv2.
// Por questões de simplicidade e necessidade , esse módulo só carrega SFF com
// sprites codificados utilizam PNG puro, recurso somente disponível no padrão,
// MUGEN 1.1 que não saiu ainda, porém o FF3 já suporta salvar arquivos com
// com esse tipo de formato.
// Marcos Medeiros
//=============================================================================
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <map>
#include <vector>
#include <utility>
#include <fstream>
#include <cmath>
#include <iostream>
#include <functional>
#include <string>
#include "Display.hpp"
#include "Sprites.hpp"
#include "Util.hpp"
#include "Console.hpp"

using namespace std;

namespace Spr
{

InitObjectCounter(Sprite);

SpriteLoader::SpriteLoader()
{
}

SpriteLoader::~SpriteLoader()
{
    cout << "Liberando sprites..." << endl;
    int nsprites = Sprite::getObjectCounter();
    for (auto i = mapSprites.begin(); i != mapSprites.end(); i++)
    {
        delete i->second;
    }
    nsprites = (nsprites - Sprite::getObjectCounter());

    cout << nsprites << " sprites desalocados com sucesso!" << endl;
}

bool SpriteLoader::open(string name)
{
    buffer = NULL;
    bufferSize = 0;
    //Console::print("SpriteLoader: carregando sprites: %s", name.c_str());

    int nsprites = Sprite::getObjectCounter();

    cout << "Carregando arquivo de sprites: " << name << endl;
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

    // Lê o cabeçalho do arquivo
    al_fread(memFile, &header, sizeof(SpriteFileHeader));
    loadSpritesFromFile();

    double elapsed = al_get_time() - startTime;
    cout << "Arquivo carregado com sucesso em " << elapsed << "s" << endl;
    nsprites = (Sprite::getObjectCounter() - nsprites);
    cout << nsprites << " sprites alocados com sucesso!" << endl;
    //Console::print("-- Sprites: %d", nsprites);
    //Console::print("SpriteLoader: ok");

    delete [] buffer;
    al_fclose(memFile);

    return true;
}


void SpriteLoader::loadSpritesFromFile()
{
    vector<Sprite*> sprList;
    al_fseek(memFile, header.sprOffset, ALLEGRO_SEEK_SET);
    for (unsigned i = 0; i < header.nSprites; i++)
    {
        // cálcula o endereço físico do sprite
        unsigned spriteNodeAddr = header.sprOffset + (i * sizeof(struct SpriteNodeInfo));

        al_fseek(memFile, spriteNodeAddr, ALLEGRO_SEEK_SET);
        Sprite *spr = new Sprite();
        if (!spr)
        {
            cerr << "Falta de memória em " << __FILE__ << __LINE__ << endl;
            exit(-1);
        }

        SpriteNodeInfo sprhead;
        al_fread(memFile, &sprhead, sizeof(SpriteNodeInfo));
        spr->group = sprhead.group;
        spr->index = sprhead.index;
        spr->palno = sprhead.palno;
        spr->xaxis = sprhead.xaxis;
        spr->yaxis = sprhead.yaxis;
        // se for uma imagem ligada, então apenas pegamos ela da nossa lista
        if (sprhead.linked || sprhead.format == 1) {
            spr->isLinked = true;
            spr->bitmap = sprList[sprhead.linked]->bitmap;
        }
        else
            loadSpriteData(spr, &sprhead, i);
        sprList.push_back(spr);
        mapSprites[make_pair(spr->group, spr->index)] = spr;
    }
}

void SpriteLoader::loadSpriteData(Sprite *spr, SpriteNodeInfo *sprnode, int thisIndex)
{
    unsigned int loffset = 0;
    if (sprnode->flags & SPR_FLAG_TDATA)
    {

        loffset = header.tdataOffset + sprnode->offset + 4;
    }
    else
    {
        loffset = header.ldataOffset + sprnode->offset + 4;
    }

    al_fseek(memFile, loffset, ALLEGRO_SEEK_SET);

    // Somente suporte ao formato PNG
    if (sprnode->format != SPR_FMT_PNG && sprnode->format != 11)
    {
        spr->bitmap = Display::createBitmap(1, 1);
        printf("[%d] %d Formato nao suportado (%d,%d)!\n", thisIndex, (unsigned)sprnode->format,
               sprnode->group, sprnode->index);
        return;
    }
    else
    {
        ALLEGRO_FILE *png = al_open_memfile(&buffer[loffset], sprnode->size, "rb");
        if (!png)
        {
            cerr << "Nao foi possivel criar o memFile do PNG" << endl;
            exit(-1);
        }
        spr->bitmap = al_load_bitmap_f(png, ".png");
        if (!spr->bitmap)
        {
            cerr << "Nao foi possivel carregar a imagem PNG" << endl;
            exit(-1);
        }
        al_fclose(png);
    }
}

const Sprite *SpriteLoader::get(int group, int index)
{
    return mapSprites[make_pair(group, index)];
}

void SpriteLoader::close()
{
}

Sprite::Sprite()
{
    isLinked = false;
    IncrementObjectCounter();
}

Sprite::~Sprite()
{
    if (bitmap && !isLinked)
    {
        al_destroy_bitmap(bitmap);
        bitmap = NULL;
    }
    DecrementObjectCounter();
}

void Sprite::draw(int posX, int posY, const Effect& effect) const
{
    if (!bitmap)
        return;
    int flip = (effect.getFlipHorizontal() ? ALLEGRO_FLIP_HORIZONTAL : 0) |
               (effect.getFlipVertical() ? ALLEGRO_FLIP_VERTICAL : 0);
    int width = al_get_bitmap_width(bitmap);
    int height = al_get_bitmap_height(bitmap);
    bool isParallax = effect.getParallax();

    if (effect.isAdditive())
        Display::setBlendMode(Display::Additive);

    // Se for invertido, então as posições no eixo x são inversas
    int r_xaxis = xaxis;
    int r_yaxis = yaxis;
    if (effect.getFlipHorizontal())
        r_xaxis = width - xaxis;
    if (effect.getFlipVertical())
        r_yaxis = height - yaxis;


    auto scaledDraw = [&](int px, int py)->void
    {
        float sx = effect.getScaleX();
        float sy = effect.getScaleY();

        al_draw_scaled_bitmap(bitmap,
            0,
            0,
            width,
            height,
            px - r_xaxis * sx,
            py - r_yaxis * sy,
            (float) width * sx,
            (float) height * sy,
            flip);
    };

    auto normalDraw = [&](int px, int py)->void
    {
        al_draw_bitmap(bitmap, px - r_xaxis, py + r_yaxis, flip);
    };
    auto tiledXDraw = [&](int px, int py, int xw, function<void(int,int)> foo)->void
    {
        int sx = px;
        if (sx == (640 + xw))
            sx = 0;
        while (sx < (640 + xw))
        {
            foo(sx, py);
            sx += xw;
        }
        sx = px - xw;
        while (sx >= (-xw))
        {
            foo(sx, py);
            sx -= xw;
        }
    };
    // Para desenharmos o efeito parallax temos uma rotina própria
    if (isParallax)
    {
        int w = al_get_bitmap_width(bitmap);
        int h = al_get_bitmap_height(bitmap);
        float sx = effect.getScaleX();
        float sy = effect.getScaleY();
        float dx = effect.getDisplaceX();
        float dy = effect.getDisplaceY();
        ALLEGRO_VERTEX vtx[5];

        //0,0
        vtx[0].x = posX - r_xaxis * sx;
        vtx[0].y = posY - r_yaxis * sy;
        vtx[0].color = al_map_rgb(255,255,255);
        vtx[0].z = 0;
        vtx[0].u = 0;
        vtx[0].v = 0;
        //1,0
        vtx[1].x = posX - r_xaxis * sx + w * sx;
        vtx[1].y = posY - r_yaxis * sy;
        vtx[1].color = al_map_rgb(255,255,255);
        vtx[1].z = 0;
        vtx[1].u = w;
        vtx[1].v = 0;
        //0,1
        vtx[2].x = posX - r_xaxis * sx + dx;
        vtx[2].y = posY - r_yaxis * sy + h * sy + dy;
        vtx[2].color = al_map_rgb(255,255,255);
        vtx[2].z = 0;
        vtx[2].u = 0;
        vtx[2].v = h;
        //1,1
        vtx[3].x = posX - r_xaxis * sx + w * sx + dx;
        vtx[3].y = posY - r_yaxis * sy + h * sy + dy;
        vtx[3].color = al_map_rgb(255,255,255);
        vtx[3].z = 0;
        vtx[3].u = w;
        vtx[3].v = h;
        al_draw_prim(&vtx, NULL, bitmap, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
    }
    else
    if (effect.isScaled())
    {
        if (effect.getTileX())
        {
            tiledXDraw(posX, posY, width * effect.getScaleX(), scaledDraw);
        }
        else
        {
            scaledDraw(posX, posY);
        }

    }
    else
    {
        if (effect.getTileX())
        {
            tiledXDraw(posX, posY, width, normalDraw);
        }
        else
        {
            normalDraw(posX, posY);
        }
    }
    if (effect.isAdditive())
        Display::setBlendMode(Display::Normal);

}

}


