#include <iostream>

#include "entity.hpp"
#include "renderwindow.hpp"
#include "utils.hpp"

using namespace std;

RenderWindow::RenderWindow(const char* title, int width, int height, Uint32 flags, Logger* plogger, Settings* psettings, const char* icon)
    :window(NULL), renderer(NULL), logger(plogger), settings(psettings) {
    //Initializing the window
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    //Check for errors
    cout << logger->getStringTime();
    if(window == NULL) {
        cout << logger->error << "Window Initialization FAILED! SDL_ERROR: " << SDL_GetError() << logger->reset << endl;
    }
    else {
        cout << logger->success << "Window Initialized Succesfully" << logger->reset << endl;
    }
    SDL_Surface* iconSurface = NULL;
    iconSurface = IMG_Load(icon);
    cout << logger->getStringTime();
    if(iconSurface == NULL) {
        cout << logger->warning << "Icon Loading FAILED! SDL_ERROR: " << SDL_GetError() << logger->reset << endl;
    }
    else {
        cout << logger->success << "Icon " << icon << " Loaded Succesfully" << logger->reset << endl;
    }
    SDL_SetWindowIcon(window, iconSurface);
    //Initializing the Renderer
    renderer = SDL_CreateRenderer(window, -1, flags);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
}

SDL_Texture* RenderWindow::loadTexture(const char* filePath) {
    //Initializing texture
    SDL_Texture* texture = NULL;
    //Check for errors
    texture = IMG_LoadTexture(renderer, filePath);
    cout << logger->getStringTime();
    if(texture == NULL) {
        cout << logger->warning << "Texture Loading FAILED! SDL_ERROR: " << SDL_GetError() << logger->reset << endl;
    }
    else {
        cout << logger->success << "Texture " << filePath << " Loaded Succesfully" << logger->reset << endl;
    }
    return texture;
}

void RenderWindow::clear() {
    SDL_RenderClear(renderer);
}

void RenderWindow::renderGui(Entity& entity) {
    SDL_Rect src;
    src.x = entity.getCurrentFrame().x;
    src.y = entity.getCurrentFrame().y;
    src.w = entity.getCurrentFrame().w;
    src.h = entity.getCurrentFrame().h;
    SDL_Rect dst;
    dst.x = entity.getPos().x * settings->win.scale * scale;
    dst.y = entity.getPos().y * settings->win.scale * scale;
    dst.w = entity.getCurrentFrame().w * settings->win.scale * ((float)scale / 2);
    dst.h = entity.getCurrentFrame().h * settings->win.scale * ((float)scale / 2);

    SDL_RenderCopy(renderer, entity.getTexture(), &src, &dst);
}

void RenderWindow::renderText(TextEntity& textEntity) {
    vector<Entity>* p = textEntity.getTextEntity();
    for(Entity& e : *p) {
        renderGui(e);
    }
}

void RenderWindow::renderButton(Button& button) {
    SDL_Rect src;
    src.x = button.getCurrentFrame().x;
    src.y = button.getCurrentFrame().y;
    src.w = button.getCurrentFrame().w;
    src.h = button.getCurrentFrame().h;
    SDL_Rect dst;
    dst.x = button.getPos().x * settings->win.scale * scale;
    dst.y = button.getPos().y * settings->win.scale * scale;
    dst.w = button.getCurrentFrame().w * settings->win.scale * ((float)scale / 2);
    dst.h = button.getCurrentFrame().h * settings->win.scale * ((float)scale / 2);

    SDL_RenderCopy(renderer, button.getTexture(), &src, &dst);
}

void RenderWindow::renderCursor(Entity& entity) {
    SDL_Rect src;
    src.x = entity.getCurrentFrame().x;
    src.y = entity.getCurrentFrame().y;
    src.w = entity.getCurrentFrame().w;
    src.h = entity.getCurrentFrame().h;
    SDL_Rect dst;
    dst.x = entity.getPos().x;
    dst.y = entity.getPos().y;
    dst.w = entity.getCurrentFrame().w * settings->win.scale * ((float)scale / 2);
    dst.h = entity.getCurrentFrame().h * settings->win.scale * ((float)scale / 2);

    SDL_RenderCopy(renderer, entity.getTexture(), &src, &dst);
}

void RenderWindow::display() {
    SDL_RenderPresent(renderer);
}

void RenderWindow::cleanUp() {
    SDL_DestroyWindow(window);
}

void RenderWindow::calculateScale() {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    scale = min((int)(w / 180), (int)(h / 166));
    if(scale < 2) scale = 2;
}

Uint8 RenderWindow::getScale() {
    return scale;
}