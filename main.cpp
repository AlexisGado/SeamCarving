// Seam Carving for content aware image resizing
// Authors: Lucie FAYETTE, Alexis GADONNEIX, Alice GRIBONVAL, Paul-Eloi MANGION, Louis REINE

#include "resizing.h"
#include "tools.h"

int main() { // Suivre les cout lors de l'éxecution (quelques exemples)

    Image<int> maskVide;
    maskVide.fill(0);

    Image<Color> I0;
    load(I0,srcPath("images/arbres.jpg"));


    Window W0 = openWindow(I0.width(), I0.height());
    setActiveWindow(W0);
    display(I0);


    cout<<"Cliquez pour réduire l'image à un format carré"<<endl;
    anyClick();
    Image<Color> I1 = I0.clone();

    reduceHorizontally(I1,I1.width()-I1.height(),maskVide);

    Window W1 = openWindow(I1.width(), I1.height());
    setActiveWindow(W1);
    display(I1);


    cout<<"Cliquez de nouveau pour agrandir l'image"<<endl;
    anyClick();
    closeWindow(W1);

    Image<Color> I2 = I0.clone();

    enlargeHorizontally(I2,300,maskVide);

    Window W2 = openWindow(I2.width(), I2.height());
    setActiveWindow(W2);
    display(I2);

    cout<<"Les résultats sont globalement satisfaisants mais on remarque que les troncs à gauche et le soleil sont affectés. Tentons de protéger ces zones"<<endl;
    anyClick();

    closeWindow(W2);

    cout<<"Selectionnez les zones à protéger en coloriant avec le clic droit. p et m permettent d'ajuster la taille du trait. t termine."<<endl;

    Image<int> mask1 = interface(I0);
    Image<Color> I3 = I0.clone();

    Image<int> mask1c = mask1.clone();

    reduceHorizontally(I3,I3.width()-I3.height(),mask1c);

    Window W3 = openWindow(I3.width(), I3.height());
    setActiveWindow(W3);
    display(I3);

    cout<<"ca fonctionne en reduction (cliquez)"<<endl;
    anyClick();
    closeWindow(W3);

    Image<Color> I4 = I0.clone();

    enlargeHorizontally(I4,300,mask1);

    Window W4 = openWindow(I4.width(), I4.height());
    setActiveWindow(W4);
    display(I4);

    cout<<"et en agrandissement."<<endl;
    cout<<"Voyons maintenant la suppression (cliquez)"<<endl;
    anyClick();
    closeWindow(W4);
    closeWindow(W0);

    Image<Color> I5;
    load(I5,srcPath("images/acteurs.jpg"));



    cout<<"Essayez de supprimer un des acteurs en le coloriant avec le clic gauche. Vous pouvez proteger ses voisins pour un meilleur resultat"<<endl;
    Image<int> mask2 = interface(I5);

    Window W5 = openWindow(I5.width(), I5.height());
    setActiveWindow(W5);
    display(I5);

    int size;

    string sens = deleteSize(mask2,size);

    if (sens == "Horizontal"){
        I5 = reduceHorizontally(I5,size, mask2);
    }
    else{
        I5 = reduceVertically(I5,size, mask2);
    }

    Window W6 = openWindow(I5.width(), I5.height());
    setActiveWindow(W6);
    display(I5);

    cout<<"l'image a ete reduite, on peut maintenant la remettre a sa taille d'origine"<<endl;
    anyClick();
    closeWindow(W6);

    if (sens == "Horizontal"){
        I5 = enlargeHorizontally(I5, size, mask2);
    }
    else{
        I5 = enlargeVertically(I5,size,mask2);
    }


    Window W7 = openWindow(I5.width(), I5.height());
    setActiveWindow(W7);
    display(I5);

    anyClick();
    closeWindow(W5);
    closeWindow(W7);

    endGraphics();
    return 0;
}

// QUELQUES REMARQUES SUPPLEMENTAIRES

// - D'autres images dans le dossier "images" sont disponibles pour tester les fonctions
// - les fonctions de reduction et d'agrandissement ne fonctionnent pas si on les applique avec une valeur N plus grande que la taille de l'image
// - Attention a ne pas utiliser la fonction delete size si la zone a supprimer n'est pas du tout connexe

