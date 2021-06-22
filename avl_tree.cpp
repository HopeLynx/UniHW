#include <stdio.h>
#include <stdlib.h>

int max(int x,int y){
    return (x>y) ? x:y;
}

typedef int key_t;
struct avlnode;
typedef struct avlnode *avltree;
struct avlnode {
    key_t key; //ключ
    avltree left; //левое поддерево
    avltree right; //правое поддерево
    // int balance; показательбаланса
    int height; //высота поддерева
};
avltree makeempty (avltree t); //удалить дерево
avltree find (key_t x, avltree t); //поиск поключу
avltree findmin (avltree t); //минимальный ключ
avltree findmax (avltree t); //максимальный ключ
avltree insert (key_t x, avltree t); //вставить узел
avltree remove (key_t x, avltree t); //исключить узел

avltree makeempty (avltree t) {
    if (t != NULL) {
        makeempty (t->left);
        makeempty (t->right);
        free (t);
    }
    return NULL;
}
avltree find (key_t x, avltree t) {
    if (t == NULL || x == t->key)
        return t;
    if (x < t->key)
        return find (x, t->left);
    if (x > t->key)
        return find (x, t->right);
    // no- warn stuff
    return t;
}

avltree findmin (avltree t) {
    if (t == NULL)
        return NULL;
    else if (t->left == NULL)
        return t;
    else
        return findmin (t->left);
}
avltree findmax (avltree t) {
    if (t != NULL)
        while (t->right != NULL)
            t = t->right;
    return t;
}

static inline int height (avltree p) {
    return p ? p->height : 0;
}

static avltree SingleRotateWithLeft (avltree k2) {
    avltree k1;
    /* выполнение поворота */
    k1 = k2->left;
    k2->left = k1->right; /* k1 != NULL */
    k1->right = k2;
    /* корректировка высот переставленных узлов */
    k2->height = max (height (k2->left),height (k2->right)) + 1;
    k1->height = max (height (k1->left), k2->height) + 1;
    return k1; /* новый корень */
}

static avltree SingleRotateWithRight (avltree k1) {
    avltree k2;
    k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;
    k1->height = max (height (k1->left),height (k1->right)) + 1;
    k2->height = max (height (k2->right), k1->height) + 1;
    return k2; /* новый корень */
}

static avltree DoubleRotateWithLeft (avltree k3) {
    /* Поворот между K1 и K2 */
    k3->left = SingleRotateWithRight (k3->left);
    /* Поворот между K3 и K2 */
    return SingleRotateWithLeft (k3);
}

static avltree DoubleRotateWithRight (avltree k1) {
    /* Поворот между K3 и K2 */
    k1->right = SingleRotateWithLeft (k1->right);
    /* Поворот между K1 и K2 */
    return SingleRotateWithRight(k1);
}

avltree insert (key_t x, avltree t) {
    if (t == NULL) {
        /* создание дерева с одним узлом */
        t = malloc(sizeof (struct avlnode));
        if (!t)
            abort();
        t->key = x;
        t->height = 1;
        t->left = t->right = NULL;
    }
    else if (x < t->key) {
        t->left = insert(x, t->left);
        if (height(t->left) - height(t->right) == 2) {
            if (x < t->left->key)
                t = SingleRotateWithLeft(t);
            else
                t = DoubleRotateWithLeft(t);
        }
    }
    else if (x > t->key) {
        t->right = insert(x, t->right);
        if (height(t->right) - height(t->left) == 2) {
            if (x > t->right->key)
                t = SingleRotateWithRight (t);
            else
                t = DoubleRotateWithRight (t);
        }
    }
    /* иначе x уже в дереве */
    t->height = max (height(t->left),
                     height(t->right)) + 1;
    return t;
}

int balance_factor(avltree t){
    return height(t->right)-height(t->left);
}

void fixheight(avltree t){
    unsigned int hl = height(t->left);
    unsigned int hr = height(t->right);
    t->height = (hl>hr?hl:hr)+1;
}


avltree balance(avltree t){
    fixheight(t);
    if( balance_factor(t)==2 )
    {
        if( balance_factor(t->right) < 0 )
            t->right = SingleRotateWithRight(t->right);
        return SingleRotateWithLeft(t);
    }
    if( balance_factor(t)==-2 )
    {
        if( balance_factor(t->left) > 0  )
            t->left = SingleRotateWithLeft(t->left);
        return SingleRotateWithRight(t);
    }
    return t; // балансировка не нужна
}

avltree removemin(avltree t){
    if (t->left == 0) return t->right;
    t->left = removemin(t->left);
    return balance(t);
}

avltree remove1(key_t x,avltree t){
    if (t == NULL) perror("no tree");
    if (x < t->key) t->left = remove1(x,t->left);
    else if (x > t->key) t->right = remove1(x,t->right);
    else {
        avltree q = t->left;
        avltree r = t->right;
        free(t);
        if (r == NULL) return q;
        avltree min = findmin(r);
        min->right = removemin(r);
        min->left = q;
        return balance(min);
    }
    return balance(t);
}

int main()
{
    printf("Hello World!\n");
    return 0;
}
