#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <cstdio>
#include <vector>
#include "SDL_Visor/SDL_Visor.hpp"
#include "Phyngine.cpp"

const int WIDTH = 3000;
const int HEIGHT = 4000;

////////////////////////////////////////////////
/////////////Declaración de funciones//////////
//////////////////////////////////////////////
void Draw_atoms(vsr::Screen* window,std::vector<vsr::Color>* Colors,std::vector<phys::RenderData> atoms);
void Draw_molecules(vsr::Screen* window, std::vector<vsr::Color>* Colors, const std::vector<phys::Phymol*> mols);
int main(){
    ////////////////////COLORES/////////////////
    std::vector<vsr::Color> Colors;
    Colors.emplace_back(0,0,5,255);         // 0    Background
    Colors.emplace_back(255,255,255,255);   // 1    White
    Colors.emplace_back(0,0,255,255);       // 2    Blue
    Colors.emplace_back(255,0,0,255);       // 3


    ///////////////////VENTANA//////////////////
    vsr::Screen window("Hola mundo",WIDTH,HEIGHT,SDL_RENDERER_ACCELERATED);   



    phys::Phyatom Hidrogeno("H","Hidrogen",4e-7,1e-27,2,2);
    phys::Phyatom Oxigeno("O","Oxigen",-4e-7,8e-27,8,3);
    
    //Hidrogeno.apply_forces(+10,-10,1);
    Oxigeno.apply_forces(-10, 10, 1);
    Oxigeno.link(&Hidrogeno, 0);

    phys::Phyngine Engine;
    auto index = Engine.add(&Hidrogeno);
    Engine.move_atom(index,int(WIDTH/2)-50,int(HEIGHT/2));


    index = Engine.add(&Oxigeno);
    Engine.move_atom(index, int(WIDTH/2) + 50, int(HEIGHT/2));

    phys::Phymol mol("Aguant",&Oxigeno);

    
    Engine.add(&mol);


    while (window.Handle_events()) {
        Engine.tick(0.03);
        SDL_Delay(2); 
        auto render_data = Engine.render_data ();    
        
        //Draw_atoms(&window,&Colors,render_data);
        Draw_molecules(&window,&Colors,Engine.get_molecules());

        window.Present_renderer(); 
        window.Clean_screen(Colors.at(0));
    }

return 0;
}

void Draw_atoms(vsr::Screen* window,std::vector<vsr::Color>* Colors,std::vector<phys::RenderData> atoms){
    for (auto atom :atoms) {
        window->Draw_filled_circle(atom.x, atom.y ,atom.r,Colors->at(atom.color));
    }
}

////////////////////////////////////////////////
/////  función pública que sí dibuja  /////////
////////////////////////////////////////
void Draw_molecules(vsr::Screen* window,
    std::vector<vsr::Color>* Colors,
    const std::vector<phys::Phymol*> mols)
{
std::vector<phys::RenderData>  atoms;
std::vector<phys::RenderBonds> bonds;

for (auto mol : mols) {

    if (auto root = mol->getRoot())
    collect_branch(root, atoms, bonds);

}

/* 1. enlaces primero ─ quedan “debajo” de los átomos */
for (auto const& b : bonds)
window->Draw_line_pos(b.px, b.py, b.cx, b.cy, Colors->at(b.c));

/* 2. átomos encima */
for (auto const& a : atoms)
window->Draw_filled_circle(a.x, a.y, a.r, Colors->at(a.color));

}
