#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "entity.hpp"

Entity::Entity(Vector2f ppos, SDL_Texture* ptexture) :pos(ppos), texture(ptexture) {
    currentFrame.x = 0;
    currentFrame.y = 0;
    currentFrame.w = 16;
    currentFrame.h = 16;
}

Vector2f& Entity::getPos() {
    return pos;
}

void Entity::setX(float px) {
    pos.x = px;
}

void Entity::setY(float py) {
    pos.y = py;
}

void Entity::setXY(float px, float py) {
    setXY(Vector2f(px, py));
}

void Entity::setXY(Vector2f ppos) {
    pos = ppos;
}

SDL_Texture* Entity::getTexture() {
    return texture;
}

SDL_Rect Entity::getCurrentFrame() {
    return currentFrame;
}

void Entity::setCurrentFrame(SDL_Rect rect) {
    currentFrame = rect;
}

TextEntity::TextEntity(Vector2f ppos, SDL_Texture* ptexture, Font* pfont) :pos(ppos), texture(ptexture), font(pfont) {
}

Vector2f& TextEntity::getPos() {
    return pos;
}

void TextEntity::setX(float px) {
    pos.x = px;
    Uint16 size = textEntity.size();
    for(Uint16 i = 0; i < size; i++) {
        textEntity[i].setX(px + (i * 3));
    }
}

void TextEntity::setY(float py) {
    pos.y = py;
    Uint16 size = textEntity.size();
    for(Uint16 i = 0; i < size; i++) {
        textEntity[i].setY(py);
    }
}

void TextEntity::setXY(float px, float py) {
    setXY(Vector2f(px, py));
}

void TextEntity::setXY(Vector2f ppos) {
    pos = ppos;
    Uint16 size = textEntity.size();
    for(Uint16 i = 0; i < size; i++) {
        textEntity[i].setXY(ppos.x + (i * 3) , ppos.y);
    }
}

void TextEntity::buildTextEntity(string ptext) {
    Uint16 length = ptext.length();
    bool sameLenght = text.length() == length;
    char c;
    text = ptext;
    if(sameLenght) {
        for(Uint16 i = 0; i < length; i++) {
            c = ptext[i];
            changeAt(i, c);
        }
    }
    else {
        textEntity.clear();
        for(Uint16 i = 0; i < length; i++) {
            Entity e = Entity(Vector2f(pos.x + (i * 3), pos.y), texture);
            c = ptext[i];
            textEntity.push_back(e);
            textEntity[i].setCurrentFrame(font->letters[int(c)]);
        }
    }
    
}

void TextEntity::changeAt(Uint16 indx, char c) {
    if(indx < Uint16(text.length())) {
        text[indx] = c;
        textEntity[indx].setCurrentFrame(font->letters[int(c)]);
    }
    else {
        text += c;
        textEntity.push_back(Entity(Vector2f(pos.x + (textEntity.size() * 3), pos.y), texture));
        textEntity[textEntity.size() - 1].setCurrentFrame(font->letters[int(c)]);
    }
}

void TextEntity::changeAt(Uint16 indx, Entity e) {
    if(indx < Uint16(textEntity.size())) {
        text[indx] = '?';
        textEntity[indx] = e;
    }
    else {
        text += '?';
        textEntity.push_back(e);
    }
}

vector<Entity>* TextEntity::getTextEntity() {
    return &textEntity;
}

TextEntity& TextEntity::operator = (string ptext) {
    this->buildTextEntity(ptext);
    return *this;
}

TextEntity& TextEntity::operator += (string ptext) {
    this->text += ptext;
    Uint16 lenght = this->text.length(), size = this->textEntity.size();
    char c;
    while(size < lenght) {
        Entity e = Entity(Vector2f(pos.x + (size * 3), pos.y), texture);
        c = text[size];
        this->textEntity.push_back(e);
        this->textEntity[size].setCurrentFrame(this->font->letters[int(c)]);
        size = this->textEntity.size();
    }
    return *this;
}

TextEntity& TextEntity::operator += (Entity e) {
    this->text += '?';
    e.setXY(pos.x + (textEntity.size() * 3), pos.y);
    this->textEntity.push_back(e);
    return *this;
}

Button::Button(Vector2f ppos, HitBox2d phitbox, SDL_Texture* pnormal, SDL_Texture* ppressed)
    :Entity(ppos, pnormal), hitbox(phitbox), normal(pnormal), pressed(ppressed) {}

void Button::action(void (*func)(int)) {
    func(2);
}

void Button::normal() {
}

HitBox2d* Button::getHitBox() {
    return &hitbox;
}