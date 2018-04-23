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

hi link MyTypes Type
hi link MyCode Statement
hi link MyConstants Constant
hi link MyAsserts Operator
