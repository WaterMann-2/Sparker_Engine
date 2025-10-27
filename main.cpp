#include <iostream>

#include "Renderer/RendererCore.h"
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    SpRenderer::RendererCore renderer;

    renderer.start("Sparker Engine");

    while ( !renderer.shouldClose() ) {
        renderer.endFrame();

    }

    renderer.stop();
    return 0;
}
