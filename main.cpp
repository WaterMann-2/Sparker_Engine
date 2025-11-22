#include <iostream>

#include <SpRenderer/RendererCore.h>

int main(int argc, char* args[]) {
    SpRenderer::RendererCore renderer;

    renderer.start("Sparker Engine");

    while ( !renderer.shouldClose() ) {
        renderer.endFrame();
    }

    renderer.stop();
    return 0;
}
