syn keyword MyTypes
\ ugeneric_t
\ ugeneric_kv_t
\ ubst_t
\ ugeneric_cmp_t
\ ugraph_t
\ uheap_t
\ ulist_iterator_t
\ ulist_t
\ umemchunk_t
\ uqueue_t
\ ustack_t
\ uvector_t
\ udict_backend_t
\ udict_t
\ udict_iterator_t
\ uhtbl_t
\ uhtbl_iterator_t
\ ulist_t
\ ulist_iterator_t
\ ulist_item_t
\ uhtbl_t
\ uhtbl_record_t
\ uhtbl_iterator_t
\ void_cmp_t
\ void_hasher_t
\ void_s8r_t

syn keyword MyAsserts
\ UASSERT_INPUT
\ UASSERT_MSG
\ UASSERT

syn keyword MyConstants
\ TRUE
\ FALSE
\ UDICT_BACKEND_DEFAULT
\ UDICT_BACKEND_BST_PLAIN
\ UDICT_BACKEND_HTBL
\ UDICT_BACKEND_BST_RB
\ G_ERROR_T
\ G_NULL_T
\ G_PTR_T
\ G_STR_T
\ G_CSTR_T
\ G_INT_T
\ G_REAL_T
\ G_SIZE_T
\ G_BOOL_T
\ G_VECTOR_T
\ G_DICT_T
\ G_MEMCHUNK_T

hi link MyTypes Type
hi link MyCode Statement
hi link MyConstants Constant
hi link MyAsserts Operator
