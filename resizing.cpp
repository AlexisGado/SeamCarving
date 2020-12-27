#include "tools.h"
#include "resizing.h"


// Renvoie une copie de I a laquelle on a retire le chemin seam
// ATTENTION : vide la liste seam à chaque execution
// pour une Image<Color>
Image<Color> removeSeam(const Image<Color> &I, list<int> &seam ){
    int w = I.width();
    int h = I.height();
    Image<Color> newImage(w-1,h);

    //Remplissage de la nouvelle image
    for(int y=0; y<h; y++){
        int toRemove = seam.front();
        for(int x=0; x<w; x++){
            if(x<toRemove)
                newImage(x,y) = I(x,y);
            else if(x>toRemove)
                newImage(x-1,y) = I(x,y);
        }
        seam.pop_front();
    }
    return newImage;
}

// pour une Image<int> (utile pour le masque)
Image<int> removeSeam(const Image<int> &I, list<int> &seam ){
    int w = I.width();
    int h = I.height();
    Image<int> newImage(w-1,h);

    //Remplissage de la nouvelle image
    for(int y=0; y<h; y++){
        int toRemove = seam.front();
        for(int x=0; x<w; x++){
            if(x<toRemove)
                newImage(x,y) = I(x,y);
            else if(x>toRemove)
                newImage(x-1,y) = I(x,y);
        }
        seam.pop_front();
    }
    return newImage;
}


// Reduit I de N pixels de large
Image<Color> reduceHorizontally(Image<Color> &I, int N, Image<int> &mask) {
    list<int> seam;
    list<int> seamCopy;
    for (int i=0; i<N; i++) {
        Image<int> grad = horizontalGradient(I); //On calcule les gradients de l'image actuelle
        applyMask(mask,grad); // On applique un éventuel masque de protection
        bestPath(seam,grad); // On trouve le meilleur chemin
        seamCopy = seam; // On fait une copie puisque removeSeam détruit le seam à l'execution
        I = removeSeam(I, seam); // On reduit l'image
        mask = removeSeam(mask,seamCopy); // On reduit le masque
    }
    return I;
}


// Reduit I de N pixels de hauteur
// On applique simplement la fonction précédente à la transposée. Sans oublier de retransposer ensuite
Image<Color> reduceVertically(Image<Color> &I, int N, Image<int> &mask) {
    Image<Color> T;
    Image<int> maskT;
    T = transpose(I);
    maskT = transpose(mask);
    T = reduceHorizontally(T, N, maskT);
    I = transpose(T);
    mask = transpose(maskT);
    return I;
}


//Toutes les fonctions qui suivent servent à l'agrandissement d'une image.
//La méthode utilisée ne résulte pas en un code très clair. Nous la détaillons dans le rapport écrit.


// Determine les coordonnees initiales des pixels dans seam et les ordonne
void sortSeams(list<int> pixels[], int h, const list<int> &seam) {
    list<int> seamCopy(seam);
    for (int y=0; y<h; y++) {
        int x = seamCopy.front();
        seamCopy.pop_front();
        list<int>::iterator it = pixels[y].begin();
        while (it!=pixels[y].end() && x>=*it) {
            ++x;
            ++it;
        }
        pixels[y].insert(it, x);
    }
}


// Ajoute les pixels necessaires
Image<Color> addSeams(const Image<Color> &I, list<int> pixels[]){
    int w = I.width();
    int h = I.height();
    int N = pixels[0].size();
    Image<Color> newImage(w+N,h);

    // Copie de l'image de depart
    for (int x=0; x<w; x++) {
        for (int y=0; y<h; y++) {
            newImage(x, y) = I(x, y);
        }
    }

    for(int y=0; y<h; y++){
        list<int> pix = pixels[y];
        int iniPos = w-1;
        int newPos = w+N-1;
        list<int>::iterator it = pix.end();
        while(it!=pix.begin()){
            --it;
            while (*it<iniPos) {
                newImage(newPos,y) = I(iniPos,y);
                newPos--;
                iniPos--;
            }
           newImage(newPos,y) = I(iniPos,y);
           newPos--;
           newImage(newPos,y) = I(iniPos,y);
           newPos--;
           iniPos--;
        }
    }
    return newImage;
}


//idem pour une Image<int>
Image<int> addSeams(const Image<int> &mask, list<int> pixels[]){
    int w = mask.width();
    int h = mask.height();
    int N = pixels[0].size();
    Image<int> newImage(w+N,h);

    // Copie de l'image de depart
    for (int x=0; x<w; x++) {
        for (int y=0; y<h; y++) {
            newImage(x, y) = mask(x, y);
        }
    }

    for(int y=0; y<h; y++){
        list<int> pix = pixels[y];
        int iniPos = w-1;
        int newPos = w+N-1;
        list<int>::iterator it = pix.end();
        while(it!=pix.begin()){
            --it;
            while (*it<iniPos) {
                newImage(newPos,y) = mask(iniPos,y);
                newPos--;
                iniPos--;
            }
           newImage(newPos,y) = mask(iniPos,y);
           newPos--;
           newImage(newPos,y) = mask(iniPos,y);
           newPos--;
           iniPos--;
        }
    }
    return newImage;
}


// Elargit l'image I de N pixels
Image<Color> enlargeHorizontally(Image<Color> &I, int N, Image<int> &mask) {
    Image<Color> I0 = I.clone();
    Image<int> mask0 = mask.clone();

    list<int> seam;
    list<int> pixels[I.height()];

    for (int i=0; i<N; i++) {
        Image<int> grad = horizontalGradient(I);
        applyMask(mask, grad);
        bestPath(seam, grad);
        sortSeams(pixels, I.height(), seam);
        list<int> seamCopy = seam;
        I = removeSeam(I, seam);
        mask = removeSeam(mask, seamCopy);
    }
    I = addSeams(I0, pixels);
    mask = addSeams(mask0, pixels);
    return I;
}


// Augmente l'image I de N pixels de haut
Image<Color> enlargeVertically(Image<Color> &I, int N, Image<int> &mask) {
    Image<Color> T;
    Image<int> maskT;
    T = transpose(I);
    maskT = transpose(mask);
    T = enlargeHorizontally(T, N, maskT);
    I = transpose(T);
    mask = transpose(maskT);
    return I;
}
