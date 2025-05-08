#pragma once
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <vector>
#include <cmath>
#include <iostream>

/*
 * Phyngine – motor de física 2‑D (Refactor mayo‑2025)
 *
 *   • Position + Inertia  →  Kinetics
 *   • Kinetics encapsula posición, orientación y dinámica lineal/rotacional.
 *   • apply_forces() hace F→v→x en un solo paso semi‑trapecio.
 */

namespace phys {

//–––––––––––––––––––––––––––––––––––––––––––––––––
// Datos mínimos para renderizar
//–––––––––––––––––––––––––––––––––––––––––––––––––
struct RenderData{
    int x;      // px
    int y;      // px
    int r;      // radio px
    int color;  // índice / RGB 24‑bit, según tu visor
};

struct RenderBonds{
    int px;
    int py;
    int cx;
    int cy;
    int c;
};

//–––––––––––––––––––––––––––––––––––––––––––––––––
// Kinetics – dinámica elemental (normaliza masa = 1)
//–––––––––––––––––––––––––––––––––––––––––––––––––
class Kinetics{
    float  x {0}, y {0};          // posición
    float  a {0};                 // orientación (rad) – pendiente
    double vx{0}, vy{0};          // velocidad lineal (px/s)
    double av{0};                 // vel. angular – pendiente
    double fx_prev{0}, fy_prev{0};
public:
    Kinetics() = default;
    Kinetics(float px,float py):x(px),y(py){}

    /**
     * Integra la fuerza lineal durante dt (seg) usando masa=1.
     * torque se ignora de momento; extiéndelo igual que fx/fy.
     */
    void apply_forces(double fx,double fy,double /*torque*/,double dt)
    {
        // 1) F → v  (∫F dt)   — semi‑trapecio
        double vx_old = vx;
        double vy_old = vy;
        vx += (dt*0.5)*(fx_prev + fx);
        vy += (dt*0.5)*(fy_prev + fy);
        fx_prev = fx;
        fy_prev = fy;

        // 2) v → x  (∫v dt)   — semi‑trapecio
        x += (dt*0.5f)*static_cast<float>(vx_old + vx);
        y += (dt*0.5f)*static_cast<float>(vy_old + vy);
        std::cout << vx << " " << vy << std::endl;
    }

    // utilidades ---------------------------------------------------
    inline void teleport(float px,float py){ x = px; y = py; }
    inline float getX() const { return x; }
    inline float getY() const { return y; }
    inline float getA() const { return a; }
};

//–––––––––––––––––––––––––––––––––––––––––––––––––
// Phyatom – Atom lógico  ↔  Kinetics físico
//–––––––––––––––––––––––––––––––––––––––––––––––––
class Phyatom {
    //—————————— datos “químicos” / visuales ——————————
    static inline unsigned int id_counter = 1;
    std::string  symbol;
    std::string  name;
    double       electric_charge;   // Coulomb (o unidades reducidas)
    double       mass;              // masa relativa (por si la usas después)
    int          radius;            // píxeles
    int          color_id;          // paleta / RGB index
    unsigned int id;                // id único
    unsigned int molecule_id{0};    // opcional: id de la molécula padre

    //—————————— estado dinámico ——————————
    Kinetics kin;                   // posición + velocidades

    //—————————— enlaces químicos ——————————
    std::vector<Phyatom*> child;
    std::vector<float> child_angle;

public:
    //——— ctor: mismo orden que tu Atom original (+ pos inicial opcional) ——–
    Phyatom(const std::string&  symbol,
            const std::string&  name,
            double              electric_charge,
            double              mass,
            int                 radius,
            int                 color_id,
            float               px = 0.0f,
            float               py = 0.0f)
        : symbol(symbol), name(name),
          electric_charge(electric_charge),
          mass(mass), radius(radius),
          color_id(color_id), id(id_counter++), kin(px, py), 
          molecule_id(id_counter){
          id_counter++;
          }

    //—————————— Dinámica ——————————
    inline void apply_forces(double fx, double fy, double dt) {
        kin.apply_forces(fx, fy, 0.0, dt);
    }
    inline void move_to(float px, float py) { kin.teleport(px, py); }

    //—————————— Getters rápidos (compatibles con tu código viejo) ——————————
    inline void pos(int* ox, int* oy, int* oa) const {
        *ox = static_cast<int>(kin.getX());
        *oy = static_cast<int>(kin.getY());
        *oa = static_cast<int>(kin.getA());
    }
    inline double q()   const { return electric_charge; }
    inline int    col() const { return color_id; }
    inline int    id_u() const { return id; }
    inline int    r()   const { return radius; }
    inline double m()   const { return mass; }
    inline int    id_mol() const { return molecule_id;}
    inline std::vector<Phyatom*> getChildren(){return child;}

    // ———————————————————— setters ——————unsigned int id——————————————
    inline void set_molecule_id(unsigned int mol_id){molecule_id = mol_id;
    printf("Mol id ->%d\n\n",mol_id);}

    //—————————— utilidades extra (opcional) ——————————
    inline const std::string& get_symbol() const { return symbol; }
    inline const std::string& get_name()   const { return name;   }
    
    //—————————— comportamientos químicos ——————————
    void link(
        Phyatom* linking_atom,
        float angle){
        child.push_back(linking_atom);
        child_angle.push_back(angle);
    }

};
//–––––––––––––––––––––––––––––––––––––––––––––––––
//  Phymol - Wrapper: Estructura de átomos
//–––––––––––––––––––––––––––––––––––––––––––––––––
class Phymol{
    inline static unsigned int id_counter = 1;
    std::string molecule_name;
    unsigned int molecule_id;
    Phyatom* molecule_root;

public:
    Phymol(std::string mol_name,Phyatom* root){
        molecule_name = mol_name;
        molecule_root = root;
        molecule_id = id_counter;
        id_counter++;
        set_molecule_id_to_atoms(root);
    }
    // —————————— geters ——————————
    inline std::string name(){return molecule_name;}
    inline unsigned int id();
    inline Phyatom* getRoot(){return molecule_root;}
    inline void  position(int* ox,int* oy, int *oa){
        return molecule_root->pos(ox, oy, oa);
    }

    // —————————— molecule dinamics ——————————


    // —————————— helpers ——————————
private:

    void set_molecule_id_to_atoms(Phyatom* root){
        root->set_molecule_id(molecule_id);
        for (auto atom : root->getChildren()) {
            atom->set_molecule_id(molecule_id);
            set_molecule_id_to_atoms(atom);
        }
    }

};


//————————————————————————————————————————
//        Phyngine – gestor global
//————————————————————————————————————————
class Phyngine{
    const double Ke = 8.9875517923e9;           // k de Coulomb
    const double AMSTRONG = 1E-3;               // Conversion a Amstrongs
    std::vector<Phyatom*> atoms;                // universo atomos
    std::vector<Phymol*> molecules;

// Métodos privados del motor de física
private:
    // Calculo de la fuerza de culomb entre particulas con carga
static inline void coulomb_force(
        const Phyatom* a1,
        const Phyatom* a2,
        double* Fx, double* Fy,
        double Ke, double AMSTRONG)
    {
        int x1, y1, dummy1;
        int x2, y2, dummy2;
        a1->pos(&x1, &y1, &dummy1);
        a2->pos(&x2, &y2, &dummy2);

        double dx = static_cast<double>(x2 - x1) * AMSTRONG;
        double dy = static_cast<double>(y2 - y1) * AMSTRONG;

        double dist2    = dx*dx + dy*dy + 1e-4;      // ε evita /0
        double inv_dist = 1.0 / std::sqrt(dist2);

        double Fmag = Ke * a1->q() * a2->q() * inv_dist * inv_dist;

        *Fx = Fmag * dx * inv_dist;  // proyección en X
        *Fy = Fmag * dy * inv_dist;  // proyección en Y
    }
    
static inline void hook_force(
        const Phyatom* a1,
        const Phyatom* a2,
        double* Fx, double* Fy,
        double Ke      = 1.0,     // usa k reducido por defecto
        double scale   = 1e-3,    // px → Å, tu caso previo
        double eps2    = 1e-4     // softening
    )
    {
        int x1, y1, dummy;
        int x2, y2, dummy2;
        a1->pos(&x1, &y1, &dummy);
        a2->pos(&x2, &y2, &dummy2);

        // Vecor separación (en unidades físicas)
        double dx = static_cast<double>(x2 - x1) * scale;
        double dy = static_cast<double>(y2 - y1) * scale;

        double dist2    = dx*dx + dy*dy + eps2;
        double inv_dist = 1.0 / std::sqrt(dist2);

        double Fmag = Ke * a1->q() * a2->q() * inv_dist * inv_dist;

        *Fx = Fmag * dx * inv_dist;
        *Fy = Fmag * dy * inv_dist;
    }

public:
    // alta / edición ------------------------------------------------
    int add(Phyatom* a){ atoms.emplace_back(a); return static_cast<int>(atoms.size())-1; }
    int add(Phymol* m){molecules.emplace_back(m); return static_cast<int>(molecules.size()-1);}

    void move_atom(int idx,float px,float py){ atoms.at(idx)->move_to(px,py); }

    // bucle principal ----------------------------------------------
    void tick(double dt)
    {
        std::size_t n = atoms.size();
        for(std::size_t i=0;i<n;i++){
            for(std::size_t j=i+1;j<n;j++){

                double Fx,Fy;
                
                auto a1 = atoms[i];
                auto a2 = atoms[j];

                if (a1->id_mol() == a2->id_mol() && a1->id_mol() != 0) {continue;}

                coulomb_force(a1, a2, &Fx, &Fy, Ke, AMSTRONG);

                a1->apply_forces(-Fx ,-Fy  ,dt);
                a2->apply_forces(Fx ,Fy ,dt);
            }
        }
    }

    // datos para render --------------------------------------------
    std::vector<RenderData> render_data() const
    {
        std::vector<RenderData> out;
        int x,y,a;
        for(auto const& pa: atoms){
            pa->pos(&x,&y,&a);
            out.push_back({x,y,pa->r(),pa->col()});
        }
        return out;
    }
    std::vector<Phymol*> get_molecules(){return molecules;}

};

// FUNCIONES AUXILIARES PARA DIBUJAR

void collect_branch(Phyatom* parent,
    std::vector<RenderData>& atoms,
    std::vector<RenderBonds>& bonds)
{
    // 1. añade el átomo padre
    int x,y,a;

    parent->pos(&x, &y, &a);
    printf("%d,%d\n",x,y);

    int r = parent->r();
    int c = parent->col();

    atoms.push_back({ x,y,r,c});

    // 2. procesa hijos recursivamente
    for (auto child : parent->getChildren()) {
    /* si el motor aún NO posiciona al hijo,
    calcula aquí (comenta si ya está listo) */
    // double a   = child->getBondAngle();   // rad
    // int    len = child->getBondLength();  // px
    //  child->setPos(parent->getPosX() + cos(a)*len,
    //                parent->getPosY() + sin(a)*len);

    // bond → línea padre‑hijo
    int cx,cy;
    child->pos(&cx, &cy, &a);
    bonds.push_back({ x, y,cx, cy,
    1 /* blanco */ });

    collect_branch(child, atoms, bonds);
    }
}

} // namespace phys
