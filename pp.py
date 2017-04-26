class GenericsPrettyPrinter:
    ''' Pretty printing of generic_t instances.
    '''
    def __init__(self, val):
        self.val = val

    def to_string (self):
        t = self.val['type']
        v = self.val['value']
#        print(str(t))
#        print(str(v))

        if t['type'] >= 11: # magic constant comes from generic.h
            return "G_MEMCHUNK{.data = %s, .size = %s}" % (v['ptr'], t['size'] - 11)
        else:
            return str(t['type'])[:-2] + "{" + {
                "G_ERROR_T": str(v['err']),
                "G_NULL_T":  "",
                "G_PTR_T":   str(v['ptr']),
                "G_STR_T":   str(v['str']),
                "G_CSTR_T":  str(v['cstr']),
                "G_INT_T":   str(v['integer']),
                "G_REAL_T":  str(v['real']),
                "G_CHR_T":   str(v['chr']),
                "G_SIZE_T":  str(v['size']),
                "G_BOOL_T":  str(v['boolean']),
                "G_VECTOR_T": str(v['ptr']),
                "G_DICT_T":   str(v['ptr']),
            }.get(str(t['type']), "unknown") + "}"

def print_generic_t(val):
    if str(val.type) == 'generic_t':
        return GenericsPrettyPrinter(val)
    return None

gdb.pretty_printers.append(print_generic_t)
