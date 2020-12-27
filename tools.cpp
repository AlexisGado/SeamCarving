#include "tools.h"


// Renvoie le gradient donne par la bibliotheque Imagine
Image<byte> horizontalGradientTest(const Image<byte> &I) {
    int w = I.width();
    int h = I.height();
    Image<byte> grad(w, h);
    for (int x=0; x<w; x++) {
        for (int y=0; y<h; y++)
            grad(x,y) = gradient(I, Coords<2> (x, y))[0];
    }
    return grad;
}


// Renvoie le gradient de l'image I
// Si I est une image couleur, la conversion en byte est automatique
Image<byte> gradient(const Image<byte> &I) {
    int w = I.width();
    int h = I.height();
    Image<byte> grad(w, h);
    byte dx, dy;

    // Interieur
    for (int x=1; x<w-1; x++) {
        for (int y=1; y<h-1; y++) {
            dx = byte(abs(I(x+1, y)-I(x-1, y)));
            dy = byte(abs(I(x, y+1)-I(x, y-1)));
            grad(x, y) = (dx + dy)/2;
        }
    }

    // Bords horizontaux
    for (int x=1; x<w-1; x++) {
        dx = byte(abs(I(x+1, 0)-I(x-1, 0))/2);
        dy = byte(abs(I(x, 1)-I(x, 0)));
        grad(x, 0) = dx + dy;
        dx = byte(abs(I(x+1, h-1)-I(x-1, h-1))/2);
        dy = byte(abs(I(x, h-1)-I(x, h-2)));
        grad(x, h-1) = dx + dy;
    }

    // Bords verticaux
    for (int y=1; y<h-1; y++) {
        dx = byte(abs(I(1, y)-I(0, y)));
        dy = byte(abs(I(0, y+1)-I(0, y-1))/2);
        grad(0, y) = dx + dy;
        dx = byte(abs(I(w-1, y)-I(w-2, y)));
        dy = byte(abs(I(w-1, y+1)-I(w-1, y-1))/2);
        grad(w-1, y) = dx + dy;
    }

    // Coins
    grad(0, 0) = byte(abs(I(1, 0)-I(0, 0)) + abs(I(0, 1)-I(0, 0)));
    grad(0, h-1) = byte(abs(I(1, h-1)-I(0, h-1)) + abs(I(0, h-1)-I(0, h-2)));
    grad(w-1, 0) = byte(abs(I(w-1, 0)-I(w-2, 0)) + abs(I(w-1, 1)-I(w-1, 0)));
    grad(w-1, h-1) = byte(abs(I(w-1, h-1)-I(w-2, h-1)) + abs(I(w-1, h-1)-I(w-1, h-2)));
    return grad;
}


// Carte du gradient horizontal (seulement selon cette direction)
// C'est cette version que nous utilisons
Image<byte> horizontalGradient(const Image<byte> &I) {
    int w = I.width();
    int h = I.height();
    Image<byte> grad(w, h);

    // Interieur et bords horizontaux
    for (int x=1; x<w-1; x++) {
        for (int y=0; y<h; y++)
            grad(x, y) = byte(abs(I(x+1, y)-I(x-1, y))/2);
    }

    // Bords verticaux
    for (int y=1; y<h-1; y++) {
        // ligne x = 0
        grad(0, y) = byte(abs(I(1, y)-I(0, y)));
        // ligne x = w-1
        grad(w-1, y) = byte(abs(I(w-1, y)-I(w-2, y)));
    }

    // Coins
    grad(0, 0) = byte(abs(I(1, 0)-I(0, 0)));
    grad(0, h-1) = byte(abs(I(1, h-1)-I(0, h-1)));
    grad(w-1, 0) = byte(abs(I(w-1, 0)-I(w-2, 0)) );
    grad(w-1, h-1) = byte(abs(I(w-1, h-1)-I(w-2, h-1)));
    return grad;
}


// Renvoie la transposee de I
Image<Color> transpose(const Image<Color> &I) {
    int w = I.width();
    int h = I.height();
    Image<Color> transpose(h, w);
    for(int x=0; x<w; x++) {
        for(int y=0; y<h; y++)
            transpose(y,x) = I(x,y);
    }
    return transpose;
}

Image<int> transpose(const Image<int> &I) {
    int w = I.width();
    int h = I.height();
    Image<int> transpose(h, w);
    for(int x=0; x<w; x++) {
        for(int y=0; y<h; y++)
            transpose(y,x) = I(x,y);
    }
    return transpose;
}


// Renvoie l'abscisse du plus petit element de la derniere ligne
int indexMinLastLine(const Image<int> &grad) {
    int h = grad.height();
    int w = grad.width();

    int min = grad(0,h-1);
    int pos = 0;

    for (int x=1; x<w; x++) {
        if (min > grad(x,h-1)) {
            min = grad(x,h-1);
            pos = x;
        }
    }

    return pos;
}


// Renvoie l'abscisse du plus petit element parmi (x-1,y), (x,y) et (x+1,y)
int indexMin(const Image<int> &grad, int x, int y) {
    int w = grad.width();

    // Bord gauche
    if (x==0) {
        if (grad(x,y) < grad(x+1,y))
            return x;
        else
            return x+1;
    }

    // Bord droit
    if (x==w) {
        if (grad(x,y) < grad(x-1,y))
            return x;
        else
            return x-1;
    }

    int min = grad(x-1,y);
    int pos = x-1;

    for(int dx=0; dx<2; dx++) {
        if (min > grad(x+dx,y)) {
            min = grad(x+dx,y);
            pos = x+dx;
        }
    }
    return pos;
}


// Modifie la liste seam pour y mettre le plus court chemin,
// l'ordonnee 0 est dans l'element front, l'ordonnee h-1 dans l'element back
// Cette fonction suit un principe de programmation dynamique détaillé dans le rapport
void bestPath(list<int> &seam, Image<int> &grad) {
    int w = grad.width();
    int h = grad.height();

    for (int y=1; y<h; y++) { // On remplit d'abord la carte des gradients par le haut
        for (int x=1; x<w-1; x++)
            grad(x,y) += min(grad(x-1,y-1), min(grad(x,y-1),grad(x+1,y-1)));
        grad(0,y) += min(grad(0,y-1), grad(1,y-1));
        grad(w-1,y) += min(grad(w-2,y-1), grad(w-1,y-1));
    }

    seam.push_front(indexMinLastLine(grad)); // On trouve la fin du meilleur chemin

    for (int y=h-2; y>=0; y--) { // On remplit ensuite en remontant l'image
        int xprec = seam.front();
        seam.push_front(indexMin(grad,xprec,y));
    }
}


// Modifie le gradient pour appliquer une protection ou des suppressions forcees
void applyMask(const Image<int> &mask, Image<int> &grad) {
    int w = mask.width();
    int h = mask.height();
    for (int x=0; x<w; x++) {
        for (int y=0; y<h; y++) {
            if (mask(x,y)==1)
                grad(x,y) += 255*h; // 255*h correspond a la valeur maximum qui peut normalement etre atteinte en bas de l'image et assure donc que le cheminne passe pas par la (sauf si rien d'autre n'est disponible)
            else if (mask(x,y)==-1)
                grad(x,y) -= 255*h; // de meme, un chemin passera forcement par ce point
        }
    }
}


// Permet de selectionner les zones de l'image à proteger ou supprimer
Image<int> interface(const Image<Color> &I) {
    int w = I.width();
    int h = I.height();
    Image<int> mask(w,h);

    for (int x=0; x<w; x++) {
        for (int y=0; y<h; y++)
            mask(x,y)=0;
    }

    Window W = openWindow(w+250,h);
    setActiveWindow(W);
    display(I);
    int taille = 9;
    drawString(w+3,20,"Maintenir clic gauche",RED,taille);
    drawString(w+3,35,"pour effacer une zone",RED,taille);
    drawString(w+3,60,"Maintenir clic droit",GREEN,taille);
    drawString(w+3,75,"pour proteger une zone",GREEN,taille);
    drawString(w+3,100,"Appuyer sur p/m pour",BLACK,taille);
    drawString(w+3,115,"changer la taille du trait",BLACK,taille);
    drawString(w+3,h-30,"Terminer : t",BLACK,taille);
    Event ev;
    int k = 2;
    do {
        getEvent(1000,ev);
        if (ev.key=='p') { // agrandit la taille du "stylo"
            k++;
            if (k>100)
                k=100;
        }
        if (ev.key=='m') { // reduit la taille
           k--;
           if (k<0)
                k=0;
        }
        if (ev.type==EVT_BUT_ON && ev.button == 1) {
            while (ev.type != EVT_BUT_OFF) {
                for (int i=-k; i<=k; i++) {
                    for (int j=-k; j<=k; j++) {
                        if (0<=ev.pix.x()+i && ev.pix.x()+i<w && 0<=ev.pix.y()+j && ev.pix.y()+j<h) {
                            drawPoint(ev.pix.x()+i,ev.pix.y()+j,RED); // ce qu'on veut effacer
                            mask(ev.pix.x()+i,ev.pix.y()+j) = -1;
                        }
                    }
                }
                getEvent(0,ev);
            }
        }
        if (ev.type==EVT_BUT_ON && ev.button == 3) {
            while (ev.type != EVT_BUT_OFF) {
                for (int i=-k; i<=k; i++) {
                    for (int j=-k; j<=k; j++) {
                        if (0<=ev.pix.x()+i && ev.pix.x()+i<w && 0<=ev.pix.y()+j && ev.pix.y()+j<h) {
                            drawPoint(ev.pix.x()+i,ev.pix.y()+j,GREEN); // ce qu'on veut proteger
                            mask(ev.pix.x()+i,ev.pix.y()+j) = 1;
                        }
                    }
                }
                getEvent(0,ev);
            }
        }
    } while (ev.type!=EVT_KEY_ON || ev.key!='t'); // On termine avec t
    closeWindow(W);
    return mask;
}


// Affiche le chemin seam sur l'image I
void displaySeam(const list<int> &seam) {
    list<int> seamCopy = seam;
    int index = 0;
    while (!seamCopy.empty()) {
        fillRect(seamCopy.front(), index, 1, 1, RED);
        seamCopy.pop_front();
        index++;
    }
}


//Cette fonction determine le diametre size de la zone à supprimer pour pouvoir réduire juste assez notre image
//Il renvoie également la direction dans laquelle il est preferable de reduire (nombre de suppression minimal)
// Attention : cette fonction n'est utile que si la zone est supprimer est assez connexe (un point dans chaque coin supprimera toute l'image)
string deleteSize(const Image<int>& masque, int& size){
    int w = masque.width();
    int h = masque.height();
    int sizeH = -1 ,sizeV = -1;

    for (int x =0; x<w;x++){
        bool pasfini = true;
        for (int y =0;y<h;y++){
            if (masque(x,y)==-1){
                sizeH = x;
                pasfini = false;
                break;
            }
        }
        if (!pasfini){
            break;
        }
    }

    for (int y =0; y<h;y++){
        bool pasfini = true;
        for (int x =0;x<w;x++){
            if (masque(x,y)==-1){
                sizeV = y;
                pasfini = false;
                break;
            }
        }
        if (!pasfini){
            break;
        }
    }

    for (int x =w-1; x>=0;x--){
        bool pasfini = true;
        for (int y =h-1;y>=0;y--){
            if (masque(x,y)==-1){
                sizeH = x - sizeH +1;
                pasfini = false;
                break;
            }
        }
        if (!pasfini){
            break;
        }
    }

    for (int y =h-1; y>=0;y--){
        bool pasfini = true;
        for (int x =w-1;x>=0;x--){
            if (masque(x,y)==-1){
                sizeV = y - sizeV +1;
                pasfini = false;
                break;
            }
        }
        if (!pasfini){
            break;
        }
    }

    if (sizeV<sizeH){
        size = sizeV;
        return "Vertical";
    }
    else{
        size = sizeH;
        return "Horizontal";
    }

}
