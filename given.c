/*Nombre de la tarea: vbc
Archivos esperados: *.c *.h
Funciones permitidas: malloc, calloc, realloc, free, printf, isdigit, write
---------------------------------------------------------- ----------------------

Escribe un programa que imprima el resultado de una expresión matemática dada como
argumento.
Debes manejar las operaciones + * y los paréntesis.
No es necesario que manejes los espacios en blanco de la expresión.
Todos los valores de la expresión estarán entre 0 y 9, ambos inclusive.
En caso de símbolo inesperado o paréntesis inadecuado,
imprimirá «Unexpected token “%c”\n» y saldrá con el código 1 (si el
símbolo es el final de la entrada, imprimirá: «Unexpected end of input\n»).
En caso de fallo de la llamada al sistema, simplemente saldrá con el código 1.

En este directorio encontrarás el comienzo del código que debes escribir.

Por ejemplo, esto debería funcionar:
$> ./vbc “1” | cat -e
1$
$> ./vbc “2+3” | cat -e
5$
$> ./vbc “3*4+5” | cat -e
17$
$> ./vbc “3+4*5” | cat -e
23$
$> ./vbc “(3+4)*5” | cat -e
35$
$> ./vbc “(((((2+2)*2+2)*2+2)*2+2)*2+2)*2” | cat -e
188$
$> ./vbc “1+” | cat -e
Fin inesperado de la entrada$
$> ./vbc “1+2)” | cat -e
Token inesperado “)”$
$> ./vbc “1+2+3+4+5” | cat -e
15$
$> ./vbc '2*4+9+3+2*1+5+1+6+6*1*1+8*0+0+5+0*4*9*5*8+9*7+5*1+3+1+4*5*7*3+0*3+4*8+8+8+4*0*5*3+5+4+5* 7+9+6*6+7+9*2*6*9+2+1*3*7*1*1*5+1+2+7+4+3*4*2+0+4*4*2*2+6+7*5+9+0+8*4+6*7+5+4*4+2+5*5+1+6+3* 5*9*9+7*4*3+7+4*9+3+0+1*8+1+2*9*4*5*1+0*1*9+5*3*5+9*6+5*4+5+5*8* 6*4*9*2+0+0+1*5*3+6*8*0+0+2*3+7*5*6+8+6*6+9+3+7+0*0+5+2*8+2*7*2+3+9*1*4*8*7*9+2*0+1*6*4*2+8*
 8*3*1+8+2*4+8*3+8*3+9*5+2*3+9*5*6*4+3*6*6+7+4*8+0+2+9*8*0*6*8*1*2*7+0*5+6*5+0*2+7+2+3+8*7+6+1*3+5+4* 5*4*6*1+4*7+9*0+4+9*8+7+5+6+2+6+1+1+1*6*0*9+7+6*2+4*4+1*6*2*9+3+0+0*1*8+4+6*2+6+2*7+7+0*9+6+2*1+6* 5*2*3*5*2*6*4+2*9*2*4*5*2*2*3+8+8*3*2*3+0*5+9*6+8+3*1+6*9+8+9*2*0+2' | cat -e
94305$
$> ./vbc “(1)” | cat -e
1$
$> ./vbc “(((((((3)))))))” | cat -e
3$
$> ./vbc “(1+2)*3” | cat -e
9$
$> ./vbc “((6*6+7+5+8)*(1+0+4*8+7)+2)+4*(1+2)” | cat -e
2254$
$> ./vbc “((1+3)*12+(3*(2+6))” | cat -e
Token inesperado “2”$*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
** Cada nodo representa:
** - VAL   : un número (0-9)
** - ADD   : suma (+)
** - MULTI : multiplicación (*)
**
** Para ADD y MULTI:
**   l = hijo izquierdo
**   r = hijo derecho
** Para VAL:
**   val = valor numérico, l/r = NULL
*/
typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

/*
** Reserva memoria para un nodo en el heap y copia dentro el contenido de 'n'.
*/
node    *new_node(node n)
{
    node    *ret;

    ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;          /* Copiamos la estructura completa */
    return (ret);
}

/*
** destroy_tree:
** Libera recursivamente el árbol completo.
**
** Regla de oro:
** - Si libero un nodo operador, primero debo liberar sus hijos
**   porque el nodo "padre" apunta a ellos.
**
** Orden de liberación: izquierda -> derecha -> nodo
** (post-order)
*/
void    destroy_tree(node *n)
{
    /*
    ** (1) Caso base: si n es NULL, no hay nada que liberar.
    **
    ** Dibujito:
    **   n = NULL
    **   (árbol vacío)
    */
    if (!n)
        return ;

    /*
    ** (2) Si NO es una hoja (VAL), entonces tiene hijos (l y r).
    **
    **
    **         [ADD] o [MULTI]
    **          /         \
    **      (izq)        (der)
    **
    ** Primero libero (izq) y (der)
    */
    if (n->type != VAL)
    {
        /*
        ** (2.1) Liberar subárbol izquierdo
        **
        ** 
        **         [n]
        **         /
        **     [subárbol izq]  <- entro aquí
        */
        destroy_tree(n->l);

        /*
        ** (2.2) Liberar subárbol derecho
        **
        ** 
        **         [n]
        **           \
        **         [subárbol der] <- entro aquí
        */
        destroy_tree(n->r);
    }

    /*
    ** (3) Finalmente, libero el nodo actual.
    **
    ** 
    **   ya liberé hijos (si existían)
    **   ahora puedo hacer:
    **       free(n);
    **
    ** Importante:
    ** - Después de free(n), "n" queda inválido.
    ** - Por eso siempre se hace al final.
    */
    free(n);
}


/*
** Imprime un error de parseo:
** - Si c existe: token inesperado
** - Si c es '\0': fin inesperado de input
*/
void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

/*
** accept:
** Si el carácter actual es 'c', lo consume avanzando el cursor y devuelve 1.
** Si no coincide, devuelve 0 y NO consume nada.
*/
int accept(char **s, char c)
{
    if (**s == c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

/*
** expect:
** Igual que accept, pero si no coincide, imprime error y devuelve 0.
*/
int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

/* Forward declaration: parse_sum se usa dentro de parse_factor (por paréntesis) */
node *parse_sum(char **s);

/*
** parse_factor:
** factor := DIGITO | '(' sum ')'
**
** - Si hay un dígito: crea VAL(dígito)
** - Si hay '(': parsea una suma completa dentro y exige ')'
** - Si no: error
*/
node *parse_factor(char **s)
{
    node    n;
    node    *inner;

    /* Caso 1: dígito */
    if (isdigit(**s))
    {
        n.type = VAL;
        n.val = **s - '0';
        n.l = NULL;
        n.r = NULL;
        (*s)++;                 /* Consumir el dígito */
        return (new_node(n));
    }

    /* Caso 2: paréntesis */
    if (accept(s, '('))
    {
        inner = parse_sum(s);   /* Parsear lo que hay dentro del paréntesis */
                                /*vuelve al nivel de menor prioridad*/
        if (!inner)
            return (NULL);

        /* Exigir el ')' de cierre */
        if (!expect(s, ')'))
        {
            destroy_tree(inner);
            return (NULL);
        }
        return (inner);
    }

    /* Caso 3: token inválido */
    unexpected(**s);
    return (NULL);
}

/*
** parse_product:
** product := factor ( '*' factor )*
**
** Construye MULTI encadenando:
** 3*4*5 -> MULTI( MULTI(3,4), 5 )
*/
node *parse_product(char **s)
{
    node    *left;
    node    *right;
    node    n;

    left = parse_factor(s);
    if (!left)
        return (NULL);

    while (accept(s, '*'))
    {
        right = parse_factor(s);
        if (!right)
        {
            destroy_tree(left);
            return (NULL);
        }

        n.type = MULTI;
        n.val = 0;
        n.l = left; //nodo acumulado o anterior
        n.r = right; //nuevo nodo
        //node n = {MULTI, 0, left, right};
        left = new_node(n);
        if (!left)
        {
            destroy_tree(right);
            return (NULL);
        }
    }
    return (left);
}

/*
** parse_sum:
** sum := product ( '+' product )*
**
** Construye ADD encadenando:
** 3+4+5 -> ADD( ADD(3,4), 5 )
**
** Importante: como aquí se parsea product, la multiplicación queda con prioridad.
*/
node *parse_sum(char **s)
{
    node    *left;
    node    *right;
    node    n;

    left = parse_product(s);
    if (!left)
        return (NULL);

    while (accept(s, '+'))
    {
        right = parse_product(s);
        if (!right)
        {
            destroy_tree(left);
            return (NULL);
        }

        n.type = ADD;
        n.val = 0;
        n.l = left; //nodo acumulado
        n.r = right; //nodo nuevo

        left = new_node(n);
        if (!left)
        {
            destroy_tree(right);
            return (NULL);
        }
    }
    return (left);
}

/*
** parse_expr:
** Punto de entrada del parseo.
** - Llama al nivel más alto (sum)
** - Verifica que se consumió toda la cadena (cursor llega a '\0')
*/
node *parse_expr(char *s)
{
    char    *cursor;
    node    *ret;

    cursor = s;
    ret = parse_sum(&cursor);
    if (!ret)
        return (NULL);

    if (*cursor != '\0')
    {
        unexpected(*cursor);
        destroy_tree(ret);
        return (NULL);
    }
    return (ret);
}

/*
** eval_tree:
** Evalúa recursivamente el AST:
** - ADD   -> izquierda + derecha
** - MULTI -> izquierda * derecha
** - VAL   -> devuelve val
*/
/*
** eval_tree:
** Evalúa recursivamente el AST.
**
** Idea clave:
** - Un nodo operador (ADD / MULTI) no “tiene un número” en sí.
**   Su resultado sale de evaluar (recursivamente) sus hijos.
** - Un nodo VAL sí tiene el número en tree->val.
**
** Ejemplo de árbol (2 + 3) * 4:
**
**            [MULTI]
**            /     \
**         [ADD]    [VAL=4]
**         /   \
**     [VAL=2] [VAL=3]
**
** eval_tree(MULTI) = eval_tree(ADD) * eval_tree(VAL=4)
**                  = (eval_tree(2) + eval_tree(3)) * 4
**                  = (2 + 3) * 4
**                  = 20
*/
int eval_tree(node *tree)
{
    /*
    ** (1) Caso: nodo ADD
    **
    ** 
    **          [ADD]
    **          /   \
    **     (izq ?)  (der ?)
    **
    ** Primero evalúo izquierda => me da un número
    ** Luego evalúo derecha    => me da un número
    ** Y devuelvo (izq + der)
    */
    if (tree->type == ADD)
        return (eval_tree(tree->l) + eval_tree(tree->r));

    /*
    ** (2) Caso: nodo MULTI
    **
    **
    **         [MULTI]
    **          /   \
    **     (izq ?)  (der ?)
    **
    ** Igual que ADD, pero multiplicando:
    ** devuelvo eval(izq) * eval(der)
    */
    if (tree->type == MULTI)
        return (eval_tree(tree->l) * eval_tree(tree->r));

    /*
    ** (3) Caso: nodo VAL (hoja)
    **
    ** 
    **        [VAL=7]
    **         (no hijos)
    **
    ** Aquí NO hay nada que calcular.
    ** El resultado del subárbol es directamente su val.
    */
    if (tree->type == VAL)
        return (tree->val);

    /*
    ** (4) Caso “defensivo” (no debería pasar si el parser construye bien)
    ** Si llega un tipo desconocido, devuelve 0 para no reventar.
    */
    return (0);
}

int main(int argc, char **argv)
{
    node    *tree;

    if (argc != 2)
        return (1);

    tree = parse_expr(argv[1]);
    if (!tree)
        return (1);

    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return (0);
}
