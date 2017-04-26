#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include "stack.h"
#include "mem.h"
#include "bst.h"
#include "string_utils.h"

typedef enum {
    BST_NODE_BLACK,
    BST_NODE_RED,
} bst_color_t;

struct bst_node {
    generic_t k;
    generic_t v;
    struct bst_node *left;
    struct bst_node *right;
    bst_color_t color;
};
typedef struct bst_node bst_node_t;

struct bst_opaq {
    bst_node_t *root;
    bst_balancing_mode_t balancing_mode;
    size_t size;
    bool is_data_owner;
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
};

struct bst_iterator_opaq {
    const bst_t *bst;
    stack_t *stack;
    bst_node_t *node;
};

typedef bool (*bst_node_cb_t)(bst_node_t *node, void *data);

static bst_balancing_mode_t _default_balancing_mode = BST_NO_BALANCING;

void bst_set_default_balancing_mode(bst_balancing_mode_t mode)
{
    ASSERT_INPUT(mode > BST_DEFAULT_BALANCING);
    ASSERT_INPUT(mode < BST_BALANCING_MODES_COUNT);
    _default_balancing_mode = mode;
}

bst_node_t **_lookup(bst_t *b, bst_node_t **root, generic_t k)
{
    bst_node_t **node = root;

    while (*node)
    {
        int cmp = generic_compare(k, (*node)->k, b->cmp);
        if (cmp < 0)
        {
            node = &(*node)->left;
        }
        else if (cmp > 0)
        {
            node = &(*node)->right;
        }
        else
        {
            break;
        }
    }

    return node;
}

static bst_node_t **_get_min(bst_node_t **root)
{
    bst_node_t **pos = root;

    while ((*pos)->left)
    {
        pos = &(*pos)->left;
    }

    return pos;
}

static bst_node_t **_get_max(bst_node_t **root)
{
    bst_node_t **pos = root;

    while ((*pos)->right)
    {
        pos = &(*pos)->right;
    }

    return pos;
}

static bst_node_t **_get_inorder_predecessor(bst_t *b, bst_node_t **node)
{
    bst_node_t **predecessor = NULL;

    if ((*node)->left)
    {
        predecessor = _get_max(&(*node)->left);
    }
    else
    {
        generic_t k = (*node)->k;
        bst_node_t **pos = &b->root;
        while (*pos)
        {
            int cmp = generic_compare(k, (*pos)->k, b->cmp);
            if (cmp < 0)
            {
                pos = &(*pos)->left;
            }
            else if (cmp > 0)
            {
                predecessor = pos;
                pos = &(*pos)->right;
            }
            else
            {
                break;
            }
        }
    }

    return predecessor;
}

static bst_node_t **_get_inorder_successor(bst_t *b, bst_node_t **node)
{
    bst_node_t **successor = NULL;

    if ((*node)->right)
    {
        successor = _get_min(&(*node)->right);
    }
    else
    {
        generic_t k = (*node)->k;
        bst_node_t **pos = &b->root;
        while (node)
        {
            int cmp = generic_compare(k, (*pos)->k, b->cmp);
            if (cmp < 0)
            {
                successor = pos;
                pos = &(*pos)->left;
            }
            else if (cmp > 0)
            {
                pos = &(*pos)->right;
            }
            else
            {
                break;
            }
        }
    }

    return successor;
}

void _bst_nodes_destroy(bst_t *b, bst_node_t *node)
{
    if (node)
    {
        _bst_nodes_destroy(b, node->left);
        _bst_nodes_destroy(b, node->right);
        if (b->is_data_owner)
        {
            generic_destroy(node->k, b->dtr);
            generic_destroy(node->v, b->dtr);
        }
        ufree(node);
    }
}

void _iterate_kv(const bst_node_t *node, bst_traverse_mode_t mode,
                 generic_kv_iter_t cb, void *data)
{
    if (node)
    {
        if (mode == BST_PREORDER)
        {
            if (cb(node->k, node->v, data)) return;
            _iterate_kv(node->left, mode, cb, data);
            _iterate_kv(node->right, mode, cb, data);
        }
        else if (mode == BST_POSTORDER)
        {
            _iterate_kv(node->left, mode, cb, data);
            _iterate_kv(node->right, mode, cb, data);
            if (cb(node->k, node->v, data)) return;
        }
        else if (mode == BST_INORDER)
        {
            _iterate_kv(node->left, mode, cb, data);
            if (cb(node->k, node->v, data)) return;
            _iterate_kv(node->right, mode, cb, data);
        }
    }
}

void _iterate_nodes(bst_node_t *node, bst_traverse_mode_t mode,
                    bst_node_cb_t cb, void *data)
{
    if (node)
    {
        if (mode == BST_PREORDER)
        {
            if (cb(node, data)) return;
            _iterate_nodes(node->left, mode, cb, data);
            _iterate_nodes(node->right, mode, cb, data);
        }
        else if (mode == BST_POSTORDER)
        {
            _iterate_nodes(node->left, mode, cb, data);
            _iterate_nodes(node->right, mode, cb, data);
            if (cb(node, data)) return;
        }
        else if (mode == BST_INORDER)
        {
            _iterate_nodes(node->left, mode, cb, data);
            if (cb(node, data)) return;
            _iterate_nodes(node->right, mode, cb, data);
        }
    }
}

static inline bool _is_black(bst_node_t *node)
{
    return !node || (node->color == BST_NODE_BLACK);
}

static inline bool _is_red(bst_node_t *node)
{
    return node && (node->color == BST_NODE_RED);
}

static inline void _set_red(bst_node_t *node)
{
    node->color = BST_NODE_RED;
}

static inline void _set_black(bst_node_t *node)
{
    node->color = BST_NODE_BLACK;
}

static bst_node_t *_rotate_left_once(bst_node_t *node)
{
    bst_node_t *t = node->right;

    node->right = t->left;
    t->left = node;
    _set_black(t);
    _set_red(node);

    return t;
}

static bst_node_t *_rotate_right_once(bst_node_t *node)
{
    bst_node_t *t = node->left;

    node->left = t->right;
    t->right = node;
    _set_black(t);
    _set_red(node);

    return t;
}

static bst_node_t *_rotate_left_twice(bst_node_t *node)
{
    node->right = _rotate_right_once(node->right);
    return _rotate_left_once(node);
}

static bst_node_t *_rotate_right_twice(bst_node_t *node)
{
    node->left = _rotate_left_once(node->left);
    return _rotate_right_once(node);
}

static bst_node_t *_make_new_node(generic_t k, generic_t v)
{
    bst_node_t *n = umalloc(sizeof(*n));
    n->k = k;
    n->v = v;
    n->left = NULL;
    n->right = NULL;
    n->color = BST_NODE_RED;

    return n;
}

static void _put_not_balanced(bst_t *b, generic_t k, generic_t v)
{
    bst_node_t **node = _lookup(b, &b->root, k);
    if (*node)
    {
        /* Update case. */
        generic_destroy((*node)->k, b->dtr);
        generic_destroy((*node)->v, b->dtr);
        (*node)->k = k;
        (*node)->v = v;
    }
    else
    {
        /* Insert case. */
        *node = _make_new_node(k, v);
        b->size += 1;
    }
}

/*
 * Top-down nonrecursive updating (insert/update or delete) of red-black tree
 * inspired by Mark Allen Weiss "Data Structures and Algorithm Analysis in C++"
 * (4th edition, ch. 12.2.2) and Julienne Walker's brilliant implementation
 * provided at http://www.eternallyconfuzzled.com
 */
static void _put_red_black(bst_t *b, generic_t k, generic_t v)
{
    // If we rotate the whole tree around the root we need
    // to store the link to a new root somewhere (as old root gets
    // rotated). Right link of the of sentinel node below is this place.
    bst_node_t sentinel_root = {.right = b->root};

    bst_node_t *x = b->root;           // current node
    bst_node_t *p = NULL;              // current node parent
    bst_node_t *g = NULL;              // current node grandpa
    bst_node_t *gg = &sentinel_root;   // current node great-grandpa
    bst_node_t **npos = &b->root;      // new node insertion position
    unsigned int path = 0;
    bool inserted = false;

    for (;;)
    {
        if (!x)
        {
            *npos = x = _make_new_node(k, v);
            inserted = true;
        }

        if (_is_red(x->left) && _is_red(x->right))
        {
            _set_red(x);
            _set_black(x->left);
            _set_black(x->right);
        }

        // Handle red violation caused either by re-coloring above
        // or by inserting a new red node at x.
        if (_is_red(p) && _is_red(x))
        {
            // Address of a link in great grand parent to be
            // updated for reattachment process after rotation.
            bst_node_t **gg_link = (gg->right == g) ? &(gg->right) : &(gg->left);
            switch (path & 0xff)
            {
                case 0x00: *gg_link = _rotate_right_once(g);  break; // LL
                case 0x01: *gg_link = _rotate_right_twice(g); break; // LR
                case 0x10: *gg_link = _rotate_left_twice(g);  break; // RL
                case 0x11: *gg_link = _rotate_left_once(g);   break; // RR
            }
        }

        if (inserted)
        {
            // As a new node was inserted there is no need for the logic below.
            // However the logic above to address possible red violation still
            // needs to be applied, so that's why breaking the loop only here.
            break;
        }

        // Move pointers down the tree.
        if (g != NULL)
        {
            gg = g;
        }
        g = p;
        p = x;
        int cmp = generic_compare(k, x->k, b->cmp);
        if (cmp < 0)
        {
            npos = &x->left;
            x = x->left;
            path = (path << 4);
        }
        else if (cmp > 0)
        {
            npos = &x->right;
            x = x->right;
            path = (path << 4) | 0x01;
        }
        else
        {
            // Found the node to be updated, update and get out of here.
            generic_destroy(p->k, b->dtr);
            generic_destroy(p->v, b->dtr);
            p->k = k;
            p->v = v;
            break;
        }
    }

    // If right link in sentinel node is NULL it means the tree was empty
    // before this call and no rotations around the root was possible,
    // nothing to restore.
    if (sentinel_root.right)
    {
        b->root = sentinel_root.right;
    }

    _set_black(b->root);

    if (inserted)
    {
        b->size += 1;
    }
}

static generic_t _pop_red_black(bst_t *b, generic_t k, generic_t vdef)
{
    (void)b;
    (void)k;
    (void)vdef;
    ABORT("not implemented");
}

static void _put_splay(bst_t *b, generic_t k, generic_t v)
{
    (void)b;
    (void)k;
    (void)v;
    ABORT("not implemented");
}

static generic_t _pop_not_balanced(bst_t *b, generic_t k, generic_t vdef)
{
    bst_node_t **pos = &b->root;
    bst_node_t **n = NULL;
    generic_t ret = vdef;

    while (*pos)
    {
        /* Look up for the node to be deleted */
        int cmp = generic_compare(k, (*pos)->k, b->cmp);
        if (cmp < 0)
        {
            pos = &(*pos)->left;
        }
        else if (cmp > 0)
        {
            pos = &(*pos)->right;
        }
        else
        {
            /* Case 1: both child nodes are present, the trickiest. */
            if ((*pos)->left && (*pos)->right)
            {
                n = _get_inorder_predecessor(b, pos);
                generic_swap(&(*n)->k, &(*pos)->k);
                generic_swap(&(*n)->v, &(*pos)->v);
                pos = n;
            }
            ret = (*pos)->v;
            if (b->is_data_owner)
            {
                generic_destroy((*pos)->k, b->dtr);
            }
            b->size -= 1;

            /* Case 2: no child nodes, just delete the node. */
            if (!(*pos)->left && !(*pos)->right)
            {
                ufree(*pos); // free node
                *pos = NULL; // clear pointer in the parent node
            }
            /* Case 3: one child */
            else if (!(*pos)->left)
            {
                bst_node_t *t = *pos;
                *pos = (*pos)->right;
                ufree(t);
            }
            else if (!(*pos)->right)
            {
                bst_node_t *t = *pos;
                *pos = (*pos)->left;
                ufree(t);
            }
            else
            {
                ABORT("Internal error");
            }
        }
    }

    return ret;
}

static generic_t _pop_splay(bst_t *b, generic_t k, generic_t vdef)
{
    (void)b;
    (void)k;
    (void)vdef;
    ABORT("not implemented");
}

bst_t *bst_create_ext(bst_balancing_mode_t mode)
{
    ASSERT_INPUT(mode >= BST_DEFAULT_BALANCING);
    ASSERT_INPUT(mode < BST_BALANCING_MODES_COUNT);

    bst_t *b = umalloc(sizeof(*b));

    b->root = NULL;
    b->size = 0;
    b->is_data_owner = true;

    if (mode == BST_DEFAULT_BALANCING)
    {
        b->balancing_mode = _default_balancing_mode;
    }
    else
    {
        b->balancing_mode = mode;
    }

    return b;
}

bst_t *bst_create(void)
{
    return bst_create_ext(BST_DEFAULT_BALANCING);
}

void bst_take_data_ownership(bst_t *b)
{
    ASSERT_INPUT(b);
    b->is_data_owner = true;
}

void bst_drop_data_ownership(bst_t *b)
{
    ASSERT_INPUT(b);
    b->is_data_owner = false;
}

void bst_put(bst_t *b, generic_t k, generic_t v)
{
    ASSERT_INPUT(b);

    switch (b->balancing_mode)
    {
        case BST_NO_BALANCING:
            _put_not_balanced(b, k, v);
            break;
        case BST_RB_BALANCING:
            _put_red_black(b, k, v);
            break;
        case BST_SPLAY_BALANCING:
            _put_splay(b, k, v);
        default:
            ABORT("internal error");
    }
}

generic_t bst_pop(bst_t *b, generic_t k, generic_t vdef)
{
    ASSERT_INPUT(b);

    generic_t ret;
    switch (b->balancing_mode)
    {
        case BST_NO_BALANCING:
            ret = _pop_not_balanced(b, k, vdef);
            break;
        case BST_RB_BALANCING:
            ret = _pop_red_black(b, k, vdef);
            break;
        case BST_SPLAY_BALANCING:
            ret = _pop_splay(b, k, vdef);
        default:
            ABORT("internal error");
    }

    return ret;

}

generic_t bst_get(bst_t *b, generic_t k, generic_t vdef)
{
    ASSERT_INPUT(b);
    bst_node_t *node = *_lookup(b, &b->root, k);
    return node ? node->v : vdef;
}

bool bst_has_key(const bst_t *b, generic_t k)
{
    ASSERT_INPUT(b);
    bst_t *t = (bst_t *)b; // cast away const qualifier
    return *_lookup(t, &t->root, k);
}

generic_t bst_get_min(bst_t *b)
{
    ASSERT_INPUT(b);
    return (*_get_min(&b->root))->v;
}

generic_t bst_get_max(bst_t *b)
{
    ASSERT_INPUT(b);
    return (*_get_max(&b->root))->v;
}

size_t bst_get_size(bst_t *b)
{
    ASSERT_INPUT(b);
    return b->size;
}

bool bst_is_empty(bst_t *b)
{
    ASSERT_INPUT(b);
    return b->size == 0;
}

void bst_destroy(bst_t *b)
{
    if (b)
    {
        _bst_nodes_destroy(b, b->root);
        ufree(b);
    }
}

void bst_clear(bst_t *b)
{
    ASSERT_INPUT(b);
    _bst_nodes_destroy(b, b->root);
    b->root = NULL;
    b->size = 0;
}

void bst_traverse(const bst_t *b, bst_traverse_mode_t mode,
                  generic_kv_iter_t cb, void *data)
{
    ASSERT_INPUT(b);
    _iterate_kv(b->root, mode, cb, data);
}

generic_t bst_get_inorder_predecessor(bst_t *b, generic_t k, generic_t vdef)
{
    ASSERT_INPUT(b);

    bst_node_t **node = _lookup(b, &b->root, k);
    if (*node)
    {
        node = _get_inorder_predecessor(b, node);
        if (node)
        {
            return (*node)->v;
        }
    }

    return vdef;
}

generic_t bst_get_inorder_successor(bst_t *b, generic_t k, generic_t vdef)
{
    ASSERT_INPUT(b);

    bst_node_t **node = _lookup(b, &b->root, k);
    if (*node)
    {
        node = _get_inorder_successor(b, node);
        if (node)
        {
            return (*node)->v;
        }
    }

    return vdef;
}

void bst_set_destroyer(bst_t *b, void_dtr_t dtr)
{
    ASSERT_INPUT(b);
    b->dtr = dtr;
}

void bst_set_comparator(bst_t *b, void_cmp_t cmp)
{
    ASSERT_INPUT(b);
    b->cmp = cmp;
}

typedef struct {
    size_t nodes_left;
    buffer_t *buf;
} _serialize_data_t;

bool _serialize(bst_node_t *node, void *data)
{
    _serialize_data_t *d = data;

    d->nodes_left -= 1;
    generic_serialize(node->k, d->buf, NULL);
    buffer_append_data(d->buf, ": ", 2);
    generic_serialize(node->v, d->buf, NULL);
    if (d->nodes_left)
    {
        buffer_append_data(d->buf, ", ", 2);
    }

    return false;
}

void bst_serialize(const bst_t *b, buffer_t *buf)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(buf);

    buffer_append_byte(buf, '{');
    _serialize_data_t d = {.nodes_left = b->size, .buf = buf};
    _iterate_nodes(b->root, BST_INORDER, _serialize, &d);
    buffer_append_byte(buf, '}');
}

char *bst_as_str(const bst_t *b)
{
    ASSERT_INPUT(b);

    buffer_t buf = {0};
    bst_serialize(b, &buf);
    buffer_null_terminate(&buf);

    return buf.data;
}

int bst_print(const bst_t *b)
{
    ASSERT_INPUT(b);
    return bst_fprint(b, stdout);
}

int bst_fprint(const bst_t *b, FILE *out)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(out);

    char *str = bst_as_str(b);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

bst_iterator_t *bst_iterator_create(const bst_t *b)
{
    ASSERT_INPUT(b);

    bst_iterator_t *bi = umalloc(sizeof(*bi));
    bi->bst = b;
    bi->stack = stack_create();
    bi->node = b->root;
    return bi;
}

bool bst_is_balanced(const bst_t *b)
{
    ASSERT_INPUT(b);
    ABORT("not implemented");
}

generic_kv_t bst_iterator_get_next(bst_iterator_t *bi)
{
    ASSERT_INPUT(bi);
    bst_node_t *n = bi->node;

    ASSERT_MSG(bi->bst->size, "container is empty");
    ASSERT_MSG(bi->node || !stack_is_empty(bi->stack), "iteration is done");

    if (n)
    {
        if (n->left)
        {
            while (n->left)
            {
                stack_push(bi->stack, G_PTR(n));
                n = n->left;
            }
            bi->node = n->right;
        }
        else if (n->right)
        {
            bi->node = n->right;
        }
        else
        {
            bi->node = NULL;
        }
    }
    else if (!stack_is_empty(bi->stack))
    {
        n = G_AS_PTR(stack_pop(bi->stack));
        bi->node = n->right;
    }
    else
    {
        ABORT("internal error");
    }

    return (generic_kv_t){.k = n->k, .v = n->v};
}

bool bst_iterator_has_next(const bst_iterator_t *bi)
{
    ASSERT_INPUT(bi);
    return bi->node || !stack_is_empty(bi->stack);
}

void bst_iterator_reset(bst_iterator_t *bi)
{
    bi->node = bi->bst->root;
    ASSERT(stack_is_empty(bi->stack));
}

void bst_iterator_destroy(bst_iterator_t *bi)
{
    if (bi)
    {
        stack_destroy(bi->stack);
        ufree(bi);
    }
}

typedef struct {
    size_t nullcnt;
    FILE *out;
    bool dump_values;
} _dump2dot_data_t;

bool _dump2dot(bst_node_t *node, void *data)
{
    _dump2dot_data_t *d = data;

    char *kstr = generic_as_str(node->k, NULL);
    char *vstr = d->dump_values ? generic_as_str(node->v, NULL) : NULL;
    char *str = string_fmt("\"%08" PRIxPTR "\"", node);

    char *lstr;
    if (node->left)
    {
        lstr = string_fmt("\"%08" PRIxPTR "\"", (uintptr_t)node->left);
    }
    else
    {
        fprintf(d->out, "    nullnode%zu [style = filled, fillcolor = black, label = null];\n", d->nullcnt);
        lstr = string_fmt("nullnode%zu", d->nullcnt++);
    }

    char *rstr;
    if (node->right)
    {
        rstr = string_fmt("\"%08" PRIxPTR "\"", (uintptr_t)node->right);
    }
    else
    {
        fprintf(d->out, "    nullnode%zu [style = filled, fillcolor = black, label = null];\n", d->nullcnt);
        rstr = string_fmt("nullnode%zu", d->nullcnt++);
    }

    fprintf(d->out, "    %s [style = filled, fillcolor = %s, fontcolor = %s, label = %s%s%s];\n",
            str,
            _is_black(node) ? "black" : "red",
            _is_black(node) ? "red" : "black",
            kstr,
            d->dump_values ? "->" : "",
            d->dump_values ? vstr: "");
    fprintf(d->out, "    %s -> %s;\n", str, lstr);
    fprintf(d->out, "    %s -> %s;\n", str, rstr);

    ufree(str); ufree(kstr); ufree(vstr); ufree(lstr); ufree(rstr);

    return false;
}
void bst_dump_to_dot(const bst_t *b, const char *name, bool dump_values, FILE *out)
{
    ASSERT_INPUT(b);

    fprintf(out, "digraph %s {\n", name);
    fprintf(out, "    label=\"%s\";\n", name);
    fprintf(out, "    labelloc=top;\n");

    _dump2dot_data_t d = {.out = out, .nullcnt = 0, .dump_values = dump_values};
    _iterate_nodes(b->root, BST_INORDER, _dump2dot, &d);
    fprintf(out, "}\n");
}

void rotate_left(bst_t *b)
{
    b->root = _rotate_left_once(b->root);
}
void rotate_right(bst_t *b)
{
    b->root = _rotate_right_once(b->root);
}
