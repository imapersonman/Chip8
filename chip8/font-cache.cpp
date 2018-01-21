#include <iostream>
#include "font-cache.h"

#define FONTCACHE_DEBUG 1

const int ASCII_OFFSET = 32;

namespace FontCache
{
    const char *ASCII_TABLE = " !\"#$%&\'()*=,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    void FillTextureOffsets(font *Font)
    {
        int PreviousWidths = 0;

        for (int CharIndex = 0; CharIndex < 95; CharIndex++)
        {
            Font->TextureOffset[CharIndex] = PreviousWidths;

            int Width;
            char CurrentCharString[2] = { ASCII_TABLE[CharIndex], 0 };
            int MinX, MaxX, MinY, MaxY, Advance;
            TTF_GlyphMetrics(Font->TTFFont, ASCII_TABLE[CharIndex], &MinX, &MaxX, &MinY, &MaxY, &Advance);
            TTF_SizeText(Font->TTFFont, CurrentCharString, &Width, NULL);
            PreviousWidths += Advance;
        }
    }

    bool FontIsLoaded(font Font)
    {
        return(Font.Texture);
    }

    font LoadFont(SDL_Renderer *Renderer, const char *FontPath, int FontSize)
    {
        font Font = {};

        SDL_RWops *FontRWops = SDL_RWFromFile(FontPath, "rb");
        if (!FontRWops)
        {
#ifdef FONTCACHE_DEBUG
            std::cout << "Unable to load font from path " << FontPath << std::endl;
            std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
#endif
            return(Font);
        }

        TTF_Font *TTFFont = TTF_OpenFontRW(FontRWops, 1, FontSize);
        if (!TTFFont)
        {
#ifdef FONTCACHE_DEBUG
            std::cout << "Unable to open font from path " << FontPath << std::endl;
            std::cout << "SDL_Error: " << TTF_GetError() << std::endl;
#endif
            return(Font);
        }

        // It messes up spacing otherwise.  Doing this universally until I figure out how kerning works.
        TTF_SetFontKerning(TTFFont, 0);

        SDL_Surface *TTFFontSurface = TTF_RenderText_Solid(TTFFont, ASCII_TABLE, {255, 255, 255});
        if (!TTFFontSurface)
        {
#ifdef FONTCACHE_DEBUG
            std::cout << "Unable to create font surface from path " << FontPath << std::endl;
            std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
#endif
            return(Font);
        }

        SDL_Texture *TTFFontTexture = SDL_CreateTextureFromSurface(Renderer, TTFFontSurface);
        if (!TTFFontTexture)
        {
#ifdef FONTCACHE_DEBUG
            std::cout << "Unable to create font texture from path " << FontPath << std::endl;
            std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
#endif
            return(Font);
        }

        Font.TTFFont = TTFFont;
        Font.Texture = TTFFontTexture;
        FillTextureOffsets(&Font);

        return(Font);
    }

    SDL_Surface *GetStringSurfaceForFont(font Font, const char *String)
    {
        SDL_Surface *TTFFontSurface = TTF_RenderText_Solid(Font.TTFFont, String, {255, 255, 255});
        if (!TTFFontSurface)
        {
            std::cout << "Unable to create font surface." << std::endl;
            std::cout << "SDL_Error: " << TTF_GetError() << std::endl;
            return(NULL);
        }

        return(TTFFontSurface);
    }

    SDL_Texture *GetStringTextureForFont(SDL_Renderer *Renderer, font Font, const char *String)
    {
        SDL_Surface *TTFFontSurface = GetStringSurfaceForFont(Font, String);
        SDL_Texture *TTFFontTexture = SDL_CreateTextureFromSurface(Renderer, TTFFontSurface);
        if (!TTFFontTexture)
        {
            std::cout << "Unable to create font texture." << std::endl;
            std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
            return(NULL);
        }

        return (TTFFontTexture);
    }

    void DestroyFont(font Font)
    {
        SDL_DestroyTexture(Font.Texture);
        TTF_CloseFont(Font.TTFFont);

        Font.Texture = NULL;
        Font.TTFFont = NULL;
    }

    void DrawSlowString(SDL_Renderer *Renderer, font Font, const char *String, int XPos, int YPos, SDL_Color Color, TextAlignment Alignment)
    {
        SDL_Surface *StringSurface = TTF_RenderText_Solid(Font.TTFFont, String, Color);
        SDL_Texture *StringTexture = SDL_CreateTextureFromSurface(Renderer, StringSurface);
        int Width, Height;
        SDL_QueryTexture(StringTexture, NULL, NULL, &Width, &Height);
        SDL_Rect DestRect = { XPos, YPos, Width, Height };
        SDL_RenderCopy(Renderer, StringTexture, NULL, &DestRect);
        SDL_FreeSurface(StringSurface);
        SDL_DestroyTexture(StringTexture);
    }

    void DrawString(SDL_Renderer *Renderer, font Font, const char *String, int XPos, int YPos, SDL_Color Color, TextAlignment Alignment)
    {
        int CharIndex = 0;
        char CurrentChar = String[0];

        switch (Alignment)
        {
            case TextAlignment_Center:
            {
                int Width, Height;
                TTF_SizeText(Font.TTFFont, String, &Width, &Height);
                XPos -= Width / 2;
                YPos -= Height / 2;
            } break;

            case TextAlignment_Left:
            {
                // Default
            } break;

            case TextAlignment_Right:
            {
                int Width;
                TTF_SizeText(Font.TTFFont, String, &Width, NULL);
                XPos -= Width;
            } break;

            default:
            {
                // Luckily, this never has to be handled.
            } break;
        }

        SDL_SetTextureColorMod(Font.Texture, Color.r, Color.g, Color.b);

        while (CurrentChar != '\0' && CurrentChar >= ASCII_OFFSET && CurrentChar < 127)
        {
            char CurrentCharString[2] = { CurrentChar, 0 };
            int Width, Height;
            TTF_SizeText(Font.TTFFont, CurrentCharString, &Width, &Height);
            int Advance;
            TTF_GlyphMetrics(Font.TTFFont, CurrentChar, NULL, NULL, NULL, NULL, &Advance);

            int SrcXPos = Font.TextureOffset[CurrentChar - 32];

            SDL_Rect SrcRect = { SrcXPos , 0, Width, Height };
            SDL_Rect DestRect = { XPos, YPos, Width, Height };
            XPos += Advance;
            CurrentChar = String[++CharIndex];

            SDL_RenderCopy(Renderer, Font.Texture, &SrcRect, &DestRect);
        }
    }
}
