#include "bst.h"

#include "asserts.h"
#include "mem.h"
#include "stack.h"
#include "string_utils.h"
#include <inttypes.h>

typedef enum {
    UBST_NODE_BLACK,
    UBST_NODE_RED,
} ubst_color_t;

struct ubst_node {
    ugeneric_t k;
    ugeneric_t v;
    struct ubst_node *left;
    struct ubst_node *right;
    ubst_color_t color;
};
typedef struct ubst_node ubst_node_t;
typedef bool (*ubst_node_cb_t)(ubst_node_t *node, void *data);

struct ubst_opaq {
    uvoid_handlers_t void_handlers;
    bool is_data_owner;
    ubst_node_t *root;
    ubst_balancing_mode_t balancing_mode;
    size_t size;
};

struct ubst_iterator_opaq {
    const ubst_t *bst;
    ustack_t *stack;
    ubst_node_t *node;
};

static ubst_balancing_mode_t _default_balancing_mode = UBST_NO_BALANCING;

void ubst_set_default_balancing_mode(ubst_balancing_mode_t mode)
{
    UASSERT_INPUT(mode > UBST_DEFAULT_BALANCING);
    UASSERT_INPUT(mode < UBST_BALANCING_MODES_COUNT);
    _default_balancing_mode = mode;
}

ubst_node_t **_lookup(ubst_t *b, ubst_node_t **root, ugeneric_t k)
{
    ubst_node_t **node = root;

    while (*node)
    {
        int cmp = ugeneric_compare_v(k, (*node)->k, b->void_handlers.cmp);
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

static ubst_node_t **_get_min(ubst_node_t **root)
{
    ubst_node_t **pos = root;

    while ((*pos)->left)
    {
        pos = &(*pos)->left;
    }

    return pos;
}

static ubst_node_t **_get_max(ubst_node_t **root)
{
    ubst_node_t **pos = root;

    while ((*pos)->right)
    {
        pos = &(*pos)->right;
    }

    return pos;
}

static ubst_node_t **_get_inorder_predecessor(ubst_t *b, ubst_node_t **node)
{
    ubst_node_t **predecessor = NULL;

    if ((*node)->left)
    {
        predecessor = _get_max(&(*node)->left);
    }
    else
    {
        ugeneric_t k = (*node)->k;
        ubst_node_t **pos = &b->root;
        while (*pos)
        {
            int cmp = ugeneric_compare_v(k, (*pos)->k, b->void_handlers.cmp);
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

static ubst_node_t **_get_inorder_successor(ubst_t *b, ubst_node_t **node)
{
    ubst_node_t **successor = NULL;

    if ((*node)->right)
    {
        successor = _get_min(&(*node)->right);
    }
    else
    {
        ugeneric_t k = (*node)->k;
        ubst_node_t **pos = &b->root;
        while (*pos)
        {
            int cmp = ugeneric_compare_v(k, (*pos)->k, b->void_handlers.cmp);
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

void _ubst_nodes_destroy(ubst_t *b, ubst_node_t *node)
{
    if (node)
    {
        _ubst_nodes_destroy(b, node->left);
        _ubst_nodes_destroy(b, node->right);
        if (b->is_data_owner)
        {
            ugeneric_destroy_v(node->k, b->void_handlers.dtr);
            ugeneric_destroy_v(node->v, b->void_handlers.dtr);
        }
        ufree(node);
    }
}

bool _iterate_kv(const ubst_node_t *node, ubst_traverse_mode_t mode,
                 ugeneric_kv_iter_t cb, void *data)
{
    if (node)
    {
        if (mode == UBST_PREORDER)
        {
            if (cb(node->k, node->v, data)) return true;
            if (_iterate_kv(node->left, mode, cb, data)) return true;
            if (_iterate_kv(node->right, mode, cb, data)) return true;
        }
        else if (mode == UBST_POSTORDER)
        {
            if (_iterate_kv(node->left, mode, cb, data)) return true;
            if (_iterate_kv(node->right, mode, cb, data)) return true;
            if (cb(node->k, node->v, data)) return true;
        }
        else if (mode == UBST_INORDER)
        {
            if (_iterate_kv(node->left, mode, cb, data)) return true;
            if (cb(node->k, node->v, data)) return true;
            if (_iterate_kv(node->right, mode, cb, data)) return true;
        }
    }

    return false;
}

bool _iterate_nodes(ubst_node_t *node, ubst_traverse_mode_t mode,
                    ubst_node_cb_t cb, void *data)
{
    if (node)
    {
        if (mode == UBST_PREORDER)
        {
            if (cb(node, data)) return true;
            if (_iterate_nodes(node->left, mode, cb, data)) return true;
            if (_iterate_nodes(node->right, mode, cb, data)) return true;
        }
        else if (mode == UBST_POSTORDER)
        {
            if (_iterate_nodes(node->left, mode, cb, data)) return true;
            if (_iterate_nodes(node->right, mode, cb, data)) return true;
            if (cb(node, data)) return true;
        }
        else if (mode == UBST_INORDER)
        {
            if (_iterate_nodes(node->left, mode, cb, data)) return true;
            if (cb(node, data)) return true;
            if (_iterate_nodes(node->right, mode, cb, data)) return true;
        }
    }

    return false;
}

static inline bool _is_black(ubst_node_t *node)
{
    return !node || (node->color == UBST_NODE_BLACK);
}

static inline bool _is_red(ubst_node_t *node)
{
    return node && (node->color == UBST_NODE_RED);
}

static inline void _set_red(ubst_node_t *node)
{
    node->color = UBST_NODE_RED;
}

static inline void _set_black(ubst_node_t *node)
{
    node->color = UBST_NODE_BLACK;
}

static ubst_node_t *_rotate_left_once(ubst_node_t *node)
{
    ubst_node_t *t = node->right;

    node->right = t->left;
    t->left = node;
    _set_black(t);
    _set_red(node);

    return t;
}

static ubst_node_t *_rotate_right_once(ubst_node_t *node)
{
    ubst_node_t *t = node->left;

    node->left = t->right;
    t->right = node;
    _set_black(t);
    _set_red(node);

    return t;
}

static ubst_node_t *_rotate_left_twice(ubst_node_t *node)
{
    node->right = _rotate_right_once(node->right);
    return _rotate_left_once(node);
}

static ubst_node_t *_rotate_right_twice(ubst_node_t *node)
{
    node->left = _rotate_left_once(node->left);
    return _rotate_right_once(node);
}

static ubst_node_t *_make_new_node(ugeneric_t k, ugeneric_t v)
{
    ubst_node_t *n = umalloc(sizeof(*n));
    n->k = k;
    n->v = v;
    n->left = NULL;
    n->right = NULL;
    n->color = UBST_NODE_RED;

    return n;
}

static void _put_not_balanced(ubst_t *b, ugeneric_t k, ugeneric_t v)
{
    ubst_node_t **node = _lookup(b, &b->root, k);
    if (*node)
    {
        /* Update case. */
        ugeneric_destroy_v((*node)->k, b->void_handlers.dtr);
        ugeneric_destroy_v((*node)->v, b->void_handlers.dtr);
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
static void _put_red_black(ubst_t *b, ugeneric_t k, ugeneric_t v)
{
    // If we rotate the whole tree around the root we need
    // to store the link to a new root somewhere (as old root gets
    // rotated). Right link of the of sentinel node below is this place.
    ubst_node_t sentinel_root = {0};
    sentinel_root.right = b->root;

    ubst_node_t *x = b->root;           // current node
    ubst_node_t *p = NULL;              // current node parent
    ubst_node_t *g = NULL;              // current node grandpa
    ubst_node_t *gg = &sentinel_root;   // current node great-grandpa
    ubst_node_t **npos = &b->root;      // new node insertion position
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
            ubst_node_t **gg_link = (gg->right == g) ? &(gg->right) : &(gg->left);
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
        int cmp = ugeneric_compare_v(k, x->k, b->void_handlers.cmp);
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
            ugeneric_destroy_v(p->k, b->void_handlers.dtr);
            ugeneric_destroy_v(p->v, b->void_handlers.dtr);
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

static ugeneric_t _pop_red_black(ubst_t *b, ugeneric_t k, ugeneric_t vdef)
{
    (void)b;
    (void)k;
    (void)vdef;
    UABORT("not implemented");
}

static void _put_splay(ubst_t *b, ugeneric_t k, ugeneric_t v)
{
    (void)b;
    (void)k;
    (void)v;
    UABORT("not implemented");
}

static ugeneric_t _pop_not_balanced(ubst_t *b, ugeneric_t k, ugeneric_t vdef)
{
    ubst_node_t **pos = &b->root;
    ubst_node_t **n = NULL;
    ugeneric_t ret = vdef;

    while (*pos)
    {
        /* Look up for the node to be deleted */
        int cmp = ugeneric_compare_v(k, (*pos)->k, b->void_handlers.cmp);
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
                ugeneric_swap(&(*n)->k, &(*pos)->k);
                ugeneric_swap(&(*n)->v, &(*pos)->v);
                pos = n;
            }
            ret = (*pos)->v;
            if (b->is_data_owner)
            {
                ugeneric_destroy_v((*pos)->k, b->void_handlers.dtr);
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
                ubst_node_t *t = *pos;
                *pos = (*pos)->right;
                ufree(t);
            }
            else if (!(*pos)->right)
            {
                ubst_node_t *t = *pos;
                *pos = (*pos)->left;
                ufree(t);
            }
            else
            {
                UABORT("Internal error");
            }
        }
    }

    return ret;
}

static ugeneric_t _pop_splay(ubst_t *b, ugeneric_t k, ugeneric_t vdef)
{
    (void)b;
    (void)k;
    (void)vdef;
    UABORT("not implemented");
}

ubst_t *ubst_create_ext(ubst_balancing_mode_t mode)
{
    UASSERT_INPUT(mode >= UBST_DEFAULT_BALANCING);
    UASSERT_INPUT(mode < UBST_BALANCING_MODES_COUNT);

    ubst_t *b = umalloc(sizeof(*b));

    b->root = NULL;
    b->size = 0;
    b->is_data_owner = true;
    memset(&b->void_handlers, 0, sizeof(b->void_handlers));

    if (mode == UBST_DEFAULT_BALANCING)
    {
        b->balancing_mode = _default_balancing_mode;
    }
    else
    {
        b->balancing_mode = mode;
    }

    return b;
}

ubst_t *ubst_create(void)
{
    return ubst_create_ext(UBST_DEFAULT_BALANCING);
}

void ubst_put(ubst_t *b, ugeneric_t k, ugeneric_t v)
{
    UASSERT_INPUT(b);

    switch (b->balancing_mode)
    {
        case UBST_NO_BALANCING:
            _put_not_balanced(b, k, v);
            break;
        case UBST_RB_BALANCING:
            _put_red_black(b, k, v);
            break;
        case UBST_SPLAY_BALANCING:
            _put_splay(b, k, v);
            break;
        default:
            UABORT("internal error");
    }
}

ugeneric_t ubst_pop(ubst_t *b, ugeneric_t k, ugeneric_t vdef)
{
    UASSERT_INPUT(b);

    ugeneric_t ret;
    switch (b->balancing_mode)
    {
        case UBST_NO_BALANCING:
            ret = _pop_not_balanced(b, k, vdef);
            break;
        case UBST_RB_BALANCING:
            ret = _pop_red_black(b, k, vdef);
            break;
        case UBST_SPLAY_BALANCING:
            ret = _pop_splay(b, k, vdef);
            break;
        default:
            UABORT("internal error");
    }

    return ret;
}

bool ubst_remove(ubst_t *b, ugeneric_t k)
{
    ugeneric_t r = ubst_pop(b, k, G_ERROR(""));
    return G_IS_ERROR(r);

}

ugeneric_t ubst_get(ubst_t *b, ugeneric_t k, ugeneric_t vdef)
{
    UASSERT_INPUT(b);
    ubst_node_t *node = *_lookup(b, &b->root, k);
    return node ? node->v : vdef;
}

bool ubst_has_key(const ubst_t *b, ugeneric_t k)
{
    UASSERT_INPUT(b);
    ubst_t *t = (ubst_t *)b; // cast away const qualifier
    return *_lookup(t, &t->root, k);
}

ugeneric_t ubst_get_min(ubst_t *b)
{
    UASSERT_INPUT(b);
    return (*_get_min(&b->root))->v;
}

ugeneric_t ubst_get_max(ubst_t *b)
{
    UASSERT_INPUT(b);
    return (*_get_max(&b->root))->v;
}

size_t ubst_get_size(ubst_t *b)
{
    UASSERT_INPUT(b);
    return b->size;
}

bool ubst_is_empty(ubst_t *b)
{
    UASSERT_INPUT(b);
    return b->size == 0;
}

void ubst_destroy(ubst_t *b)
{
    if (b)
    {
        _ubst_nodes_destroy(b, b->root);
        ufree(b);
    }
}

void ubst_clear(ubst_t *b)
{
    UASSERT_INPUT(b);
    _ubst_nodes_destroy(b, b->root);
    b->root = NULL;
    b->size = 0;
}

void ubst_traverse(const ubst_t *b, ubst_traverse_mode_t mode,
                   ugeneric_kv_iter_t iter, void *data)
{
    UASSERT_INPUT(b);
    (void)_iterate_kv(b->root, mode, iter, data);
}

ugeneric_t ubst_get_inorder_predecessor(ubst_t *b, ugeneric_t k, ugeneric_t vdef)
{
    UASSERT_INPUT(b);

    ubst_node_t **node = _lookup(b, &b->root, k);
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

ugeneric_t ubst_get_inorder_successor(ubst_t *b, ugeneric_t k, ugeneric_t vdef)
{
    UASSERT_INPUT(b);

    ubst_node_t **node = _lookup(b, &b->root, k);
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

typedef struct {
    const ubst_t *b;
    size_t nodes_left;
    ubuffer_t *buf;
} _serialize_data_t;

bool _serialize(ubst_node_t *node, void *data)
{
    _serialize_data_t *d = data;

    d->nodes_left -= 1;
    ugeneric_serialize_v(node->k, d->buf, d->b->void_handlers.s8r);
    ubuffer_append_data(d->buf, ": ", 2);
    ugeneric_serialize_v(node->v, d->buf, d->b->void_handlers.s8r);
    if (d->nodes_left)
    {
        ubuffer_append_data(d->buf, ", ", 2);
    }

    return false;
}

void ubst_serialize(const ubst_t *b, ubuffer_t *buf)
{
    UASSERT_INPUT(b);
    UASSERT_INPUT(buf);

    ubuffer_append_byte(buf, '{');
    _serialize_data_t d = {.b = b, .nodes_left = b->size, .buf = buf};
    _iterate_nodes(b->root, UBST_INORDER, _serialize, &d);
    ubuffer_append_byte(buf, '}');
}

char *ubst_as_str(const ubst_t *b)
{
    UASSERT_INPUT(b);

    ubuffer_t buf = {0};
    ubst_serialize(b, &buf);
    ubuffer_null_terminate(&buf);

    return buf.data;
}

int ubst_fprint(const ubst_t *b, FILE *out)
{
    UASSERT_INPUT(b);
    UASSERT_INPUT(out);

    char *str = ubst_as_str(b);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

ubst_iterator_t *ubst_iterator_create(const ubst_t *b)
{
    UASSERT_INPUT(b);

    ubst_iterator_t *bi = umalloc(sizeof(*bi));
    bi->bst = b;
    bi->stack = ustack_create();
    ustack_drop_data_ownership(bi->stack);
    bi->node = b->root;
    return bi;
}

bool ubst_is_balanced(const ubst_t *b)
{
    UASSERT_INPUT(b);
    UABORT("not implemented");
}

ugeneric_kv_t ubst_iterator_get_next(ubst_iterator_t *bi)
{
    UASSERT_INPUT(bi);
    ubst_node_t *n = bi->node;

    UASSERT_MSG(bi->bst->size, "container is empty");
    UASSERT_MSG(bi->node || !ustack_is_empty(bi->stack), "iteration is done");

    if (n)
    {
        if (n->left)
        {
            while (n->left)
            {
                ustack_push(bi->stack, G_PTR(n));
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
    else if (!ustack_is_empty(bi->stack))
    {
        n = G_AS_PTR(ustack_pop(bi->stack));
        bi->node = n->right;
    }
    else
    {
        UABORT("internal error");
    }

    return (ugeneric_kv_t){.k = n->k, .v = n->v};
}

bool ubst_iterator_has_next(const ubst_iterator_t *bi)
{
    UASSERT_INPUT(bi);
    return bi->node || !ustack_is_empty(bi->stack);
}

void ubst_iterator_reset(ubst_iterator_t *bi)
{
    UASSERT_INPUT(bi);
    bi->node = bi->bst->root;
    ustack_clear(bi->stack);
}

void ubst_iterator_destroy(ubst_iterator_t *bi)
{
    if (bi)
    {
        ustack_destroy(bi->stack);
        ufree(bi);
    }
}

uvector_t *ubst_get_items(const ubst_t *b, udict_items_kind_t kind, bool deep)
{
    UASSERT_INPUT(b);
    (void)deep; //TODO: implement it

    ubst_iterator_t *bi = ubst_iterator_create(b);
    uvector_t *v = uvector_create();
    uvector_reserve_capacity(v, b->size);
    while (ubst_iterator_has_next(bi))
    {
        ugeneric_kv_t item = ubst_iterator_get_next(bi);
        switch (kind)
        {
            case UDICT_KEYS:
                uvector_append(v, item.k);
                break;
            case UDICT_VALUES:
                uvector_append(v, item.v);
                break;
            case UDICT_KV:
                uvector_append(v, item.k);
                uvector_append(v, item.v);
                break;
            default:
                UABORT("internal error");
        }
    }
    ubst_iterator_destroy(bi);

    uvector_drop_data_ownership(v);
    uvector_set_void_comparator(v, b->void_handlers.cmp); // vector sort should use original cmp
    uvector_set_void_serializer(v, b->void_handlers.s8r);
    uvector_shrink_to_size(v);

    return v;
}

typedef struct {
    size_t nullcnt;
    FILE *out;
    bool dump_values;
    ubst_balancing_mode_t balancing_mode;
} _dump2dot_data_t;

bool _dump2dot(ubst_node_t *node, void *data)
{
    _dump2dot_data_t *d = data;

    char *kstr = ugeneric_as_str_v(node->k, NULL);
    char *vstr = d->dump_values ? ugeneric_as_str_v(node->v, NULL) : NULL;
    char *str = ustring_fmt("\"%08" PRIxPTR "\"", node);

    char *lstr = NULL;
    if (node->left)
    {
        lstr = ustring_fmt("\"%08" PRIxPTR "\"", (uintptr_t)node->left);
    }
    else if (d->balancing_mode == UBST_RB_BALANCING)
    {
        fprintf(d->out, "    nullnode%zu [style = filled, fillcolor = black, label = null];\n", d->nullcnt);
        lstr = ustring_fmt("nullnode%zu", d->nullcnt++);
    }

    char *rstr = NULL;
    if (node->right)
    {
        rstr = ustring_fmt("\"%08" PRIxPTR "\"", (uintptr_t)node->right);
    }
    else if (d->balancing_mode == UBST_RB_BALANCING)
    {
        fprintf(d->out, "    nullnode%zu [style = filled, fillcolor = black, label = null];\n", d->nullcnt);
        rstr = ustring_fmt("nullnode%zu", d->nullcnt++);
    }

    fprintf(d->out, "    %s [style = filled, fillcolor = %s, fontcolor = %s, label = %s%s%s];\n",
            str,
            _is_black(node) ? "black" : "red",
            _is_black(node) ? "red" : "black",
            kstr,
            d->dump_values ? "->" : "",
            d->dump_values ? vstr : "");
    if (lstr)
    {
        fprintf(d->out, "    %s -> %s;\n", str, lstr);
    }
    if (rstr)
    {
        fprintf(d->out, "    %s -> %s;\n", str, rstr);
    }

    ufree(str); ufree(kstr); ufree(vstr); ufree(lstr); ufree(rstr);

    return false;
}

void ubst_dump_to_dot(const ubst_t *b, const char *name, bool dump_values, FILE *out)
{
    UASSERT_INPUT(b);

    fprintf(out, "digraph %s {\n", name);
    fprintf(out, "    label=\"%s\";\n", name);
    fprintf(out, "    labelloc=top;\n");

    _dump2dot_data_t d = {.nullcnt = 0, .out = out, .dump_values = dump_values, .balancing_mode = b->balancing_mode};
    _iterate_nodes(b->root, UBST_INORDER, _dump2dot, &d);
    fprintf(out, "}\n");
}
