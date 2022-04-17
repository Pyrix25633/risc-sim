#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "math.hpp"
#include "utils.hpp"

/**
 * @brief Class for basic entity
*/
class Entity {
    public:
        /**
         * @brief Constructor
         * @param ppos Position of the entity on the screen, type Vector2f
         * @param ptexture The texture for the entity, type SDL_Texture*
        */
        Entity(Vector2f ppos, SDL_Texture* ptexture);
        /**
         * @brief Constructor
         * @param ppos Position of the entity on the screen, type Vector2f
         * @param ptexture The texture for the entity, type SDL_Texture*
         * @param h Height
         * @param w Width
        */
        Entity(Vector2f ppos, SDL_Texture* ptexture, Uint8 h, Uint8 w);
        /**
         * @brief Function to get the position of the entity
         * @return The position, type Vector2f
        */
        Vector2f& getPos();
        /**
         * @brief Function to set x
         * @param px The x coordinate, type float
        */
        void setX(float px);
        /**
         * @brief Function to set y
         * @param py The y coordinate, type float
        */
        void setY(float py);
        /**
         * @brief Function to set x and y
         * @param px The x coordinate, type float
         * @param py The y coordinate, type float
        */
        void setXY(float px, float py);
        /**
         * @brief Function to set x and y
         * @param ppos The position, type Vector2f
        */
        void setXY(Vector2f ppos);
        /**
         * @brief Function to get the texture
         * @return The texture, type SDL_Texture*
        */
        SDL_Texture* getTexture();
        /**
         * @brief Function to get the current frame
         * @return The frame, type SDL_Rect
        */
        SDL_Rect getCurrentFrame();
        /**
         * @brief Function to set the current frame
         * @param rect The frame, type SDL_Rect
        */
        void setCurrentFrame(SDL_Rect rect);
    protected:
        Vector2f pos;
        SDL_Rect currentFrame;
        SDL_Texture* texture;
};

/**
 * @brief Class for a text entity
*/
class TextEntity {
    public:
        TextEntity(Vector2f ppos, SDL_Texture* ptexture, Font* pfont);
        /**
         * @brief Function to get the position of the entity
         * @return The position, type Vector2f
        */
        Vector2f& getPos();
        /**
         * @brief Function to set x
         * @param px The x coordinate, type float
        */
        void setX(float px);
        /**
         * @brief Function to set y
         * @param py The y coordinate, type float
        */
        void setY(float py);
        /**
         * @brief Function to set x and y
         * @param px The x coordinate, type float
         * @param py The y coordinate, type float
        */
        void setXY(float px, float py);
        /**
         * @brief Function to set x and y
         * @param ppos The position, type Vector2f
        */
        void setXY(Vector2f ppos);
        /**
         * @brief Function to build the text entity
         * @param ptext The text to build, type string
        */
        void buildTextEntity(string ptext);
        /**
         * @brief Function to change a character
         * @param indx The index to change the characer at, type Uint16
         * @param c The character to change with, type char
        */
        void changeAt(Uint16 indx, char c);
        /**
         * @brief Function to change an entity
         * @param indx The index to change the entity at, type Uint16
         * @param e The entity to change with, type Entity
        */
        void changeAt(Uint16 indx, Entity e);
        /**
         * @brief Function to get the text entity
         * @return The entity vector pointer
        */
        vector<Entity>* getTextEntity();
        TextEntity& operator = (string ptext);
        TextEntity& operator += (string ptext);
        TextEntity& operator += (Entity e);
    private:
        Vector2f pos;
        SDL_Texture* texture;
        vector<Entity> textEntity;
        Font* font;
        string text;
};

/**
 * @brief Button Entity
*/
class Button :public Entity {
    public:
        /**
         * @brief Constructor
         * @param ppos Position
         * @param phitbox Hitbox of the button
         * @param ptexture Texture
        */
        Button(Vector2f ppos, HitBox2d phitbox, SDL_Texture* pnormal, SDL_Texture* ppressed);
        void action(void (*func)(int));
        /**
         * @brief Function to switch the current texture with the normal one 
        */
        void changeNormal();
        /**
         * @brief Function to switch the current tecture with the pressed one
        */
        void changePressed();
        /**
         * @brief Function to get the HitBox
         * @returns The hitbox pointer
        */
        HitBox2d* getHitBox();
    private:
        HitBox2d hitbox;
        SDL_Texture* normal;
        SDL_Texture* pressed;
};