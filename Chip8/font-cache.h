#ifndef __FONT_CACHE_H__
#define __FONT_CACHE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace FontCache
{
    extern const char *ASCII_TABLE;
    typedef enum
    {
        TextAlignment_Center,
        TextAlignment_Left,
        TextAlignment_Right
    } TextAlignment;

    typedef struct
    {
        SDL_Texture *Texture;
        TTF_Font *TTFFont;
        int TextureOffset[95];
    } font;

    bool FontIsLoaded(font Font);
    SDL_Surface *GetStringSurfaceForFont(font Font, const char *String);
    SDL_Texture *GetStringTextureForFont(SDL_Renderer *Renderer, font Font, const char *String);
    font LoadFont(SDL_Renderer *Renderer, const char *FontPath, int FontSize);
    void DestroyFont(font Font);
    void DrawSlowString(SDL_Renderer *Renderer, font Font, const char *String, int XPos, int YPos, SDL_Color Color, TextAlignment Alignment = TextAlignment_Left);
    void DrawString(SDL_Renderer *Renderer, font Font, const char *String, int XPos, int YPos, SDL_Color Color, TextAlignment Alignment = TextAlignment_Left);
};

#endif /* __FONT_CACHE_H__ */
