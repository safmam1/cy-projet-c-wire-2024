#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  Structure pour un noeud d'arbre AVL
typedef struct AVLNode {
    char station_id[20];
    long capacity;
    long consumption;
    struct AVLNode *left, *right;
    int height;
} AVLNode;

// Fonctions utilitaires pour l'arbre AVL
int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(AVLNode *node) {
    return node ? node->height : 0;
}

// Créer un nouveau nœud AVL
AVLNode *createNode(const char *id, long capacity, long consumption) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    strcpy(node->station_id, id);
    node->capacity = capacity;
    node->consumption = consumption;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Rotation à droite pour rééquilibrer l'arbre
AVLNode *rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

// Rotation à gauche pour rééquilibrer l'arbre
AVLNode *leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

// Obtenir le facteur d'équilibre d'un nœud
int getBalance(AVLNode *node) {
    return node ? height(node->left) - height(node->right) : 0;
}

// Insérer un élément dans l'arbre AVL
AVLNode *insert(AVLNode *node, const char *id, long capacity, long consumption) {
    if (!node) return createNode(id, capacity, consumption);

    if (strcmp(id, node->station_id) < 0)
        node->left = insert(node->left, id, capacity, consumption);
    else if (strcmp(id, node->station_id) > 0)
        node->right = insert(node->right, id, capacity, consumption);
    else {
        node->capacity += capacity;
        node->consumption += consumption;
        return node;
    }

    node->height = max(height(node->left), height(node->right)) + 1;

    int balance = getBalance(node);

 // Rééquilibrage de l'arbre
    if (balance > 1 && strcmp(id, node->left->station_id) < 0)
        return rightRotate(node);

    if (balance < -1 && strcmp(id, node->right->station_id) > 0)
        return leftRotate(node);

    if (balance > 1 && strcmp(id, node->left->station_id) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && strcmp(id, node->right->station_id) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Parcourir l'arbre en ordre infixe et écrire les données
void inorderTraversal(AVLNode *root, FILE *output) {
    if (root) {
        inorderTraversal(root->left, output);
        fprintf(output, "%s,%ld,%ld\n", root->station_id, root->capacity, root->consumption);
        inorderTraversal(root->right, output);
    }
}

// Libérer la mémoire de l'arbre AVL
void freeTree(AVLNode *root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    if (!input || !output) {
        perror("File error");
        return 1;
    }

    AVLNode *root = NULL;
    char line[256], station_id[20];
    long capacity, consumption;

while (fgets(line, sizeof(line), input)) {
    // Traiter chaque ligne du fichier CSV
    printf("Processing line: %s", line); // Debug raw line
    char hvb[20], hva[20], lv[20], company[20], individual[20];
    char capacity_str[20], consumption_str[20];

    sscanf(line, "%19[^,],%19[^,],%19[^,],%19[^,],%19[^,],%19[^,],%19[^,],%19[^,\n]",
           station_id, hvb, hva, lv, company, individual, capacity_str, consumption_str);

    // Analyser la capacité
    if (strcmp(capacity_str, "-") != 0) {
        capacity = atol(capacity_str);
    } else {
        capacity = 0;  // Par défaut à 0 si le champ est manquant
    }

   // Analyser la consommation
    if (strcmp(consumption_str, "-") != 0) {
        consumption = atol(consumption_str);
    } else {
        consumption = 0;  // Par défaut à 0 si le champ est manquant
    }

    printf("Parsed - Station ID: %s, Capacity: %ld, Consumption: %ld\n", station_id, capacity, consumption);

    root = insert(root, station_id, capacity, consumption);
}





    fprintf(output, "Station ID,Capacity (kWh),Consumption (kWh)\n");
    inorderTraversal(root, output);

    freeTree(root);
    fclose(input);
    fclose(output);

    return 0;
}
