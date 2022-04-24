#include <iostream>

#include "entity.hpp"
#include "renderwindow.hpp"
#include "utils.hpp"

using namespace std;

RenderWindow::RenderWindow(const char* title, int width, int height, Uint32 flags, Logger* plogger, Settings* psettings, const char* icon)
    :Window(NULL), Renderer(NULL), logger(plogger), settings(psettings) {
    //Initializing the Window
    Window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    //Check for errors
    cout << logger->getStringTime();
    if(Window == NULL) {
        cout << logger->error << "Window Init FAILED! SDL_ERROR: " << SDL_GetError() << logger->reset << endl;
    }
    else {
        cout << logger->success << "Window Init SUCCEEDED" << logger->reset << endl;
    }
    SDL_Surface* iconSurface = IMG_Load(icon);
    SDL_SetWindowIcon(Window, iconSurface);
    //Initializing the Renderer
    Renderer = SDL_CreateRenderer(Window, -1, flags);
    SDL_SetRenderDrawColor(Renderer, 50, 50, 50, 255);
}

SDL_Texture* RenderWindow::loadTexture(const char* filePath) {
    //Initializing texture
    SDL_Texture* texture = NULL;
    //Check for errors
    texture = IMG_LoadTexture(Renderer, filePath);
    cout << logger->getStringTime();
    if(texture == NULL) {
        cout << logger->error << "Texture Load FAILED! SDL_ERROR: " << SDL_GetError() << logger->reset << endl;
    }
    else {
        cout << logger->success << "Texture " << filePath << " Init SUCCEEDED" << logger->reset << endl;
    }
    return texture;
}

void RenderWindow::clear() {
    SDL_RenderClear(Renderer);
}

void RenderWindow::render(Entity& entity) {
    SDL_Rect src;
    src.x = entity.getCurrentFrame().x;
    src.y = entity.getCurrentFrame().y;
    src.w = entity.getCurrentFrame().w;
    src.h = entity.getCurrentFrame().h;
    SDL_Rect dst;
    dst.x = entity.getPos().x * settings->win.scale * 4;
    dst.y = entity.getPos().y * settings->win.scale * 4;
    dst.w = entity.getCurrentFrame().w * settings->win.scale * 4;
    dst.h = entity.getCurrentFrame().h * settings->win.scale * 4;

    SDL_RenderCopy(Renderer, entity.getTexture(), &src, &dst);
}

void RenderWindow::renderGui(Entity& entity) {
    SDL_Rect src;
    src.x = entity.getCurrentFrame().x;
    src.y = entity.getCurrentFrame().y;
    src.w = entity.getCurrentFrame().w;
    src.h = entity.getCurrentFrame().h;
    SDL_Rect dst;
    dst.x = entity.getPos().x * settings->win.scale * 4;
    dst.y = entity.getPos().y * settings->win.scale * 4;
    dst.w = entity.getCurrentFrame().w * settings->win.guiScale * 2;
    dst.h = entity.getCurrentFrame().h * settings->win.guiScale * 2;

    SDL_RenderCopy(Renderer, entity.getTexture(), &src, &dst);
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
    dst.x = button.getPos().x * settings->win.scale * 4;
    dst.y = button.getPos().y * settings->win.scale * 4;
    dst.w = button.getCurrentFrame().w * settings->win.guiScale * 2;
    dst.h = button.getCurrentFrame().h * settings->win.guiScale * 2;

    SDL_RenderCopy(Renderer, button.getTexture(), &src, &dst);
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
    dst.w = entity.getCurrentFrame().w * settings->win.guiScale * 2;
    dst.h = entity.getCurrentFrame().h * settings->win.guiScale * 2;

    SDL_RenderCopy(Renderer, entity.getTexture(), &src, &dst);
}

void RenderWindow::display() {
    SDL_RenderPresent(Renderer);
}

void RenderWindow::cleanUp() {
    SDL_DestroyWindow(Window);
}